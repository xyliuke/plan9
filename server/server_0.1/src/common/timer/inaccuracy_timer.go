package timer

import (
	"time"
	"container/list"
	"sync"
	"fmt"
)

var timerMap map[int64]*list.List = make(map[int64]*list.List)
var cancel_map map[int64]*timer_record = make(map[int64]*timer_record)
var once sync.Once

var earliestTime int64 = time.Now().Unix()

type timer_record struct {
	ch chan int64
	id int64
	parent *list.List
}

func new_timer_record(c chan int64, id int64) *timer_record {
	t := new(timer_record)
	t.ch = c
	t.id = id
	return t
}

/**
	创建一个精度为1秒的定时器,使用chan的方式使用
	chan 返回当前时间的秒数,如果返回值为0,表示timer被取消
 */
func NewInaccuracyTimer(ticker time.Duration) (int64, chan int64) {

	once.Do(func (){
		go runTimer()
	})

	t := time.Now().Add(ticker).Unix()
	l := timerMap[t]
	if l == nil {
		l = new(list.List)
		timerMap[t] = l
	}

	c := make(chan int64)
	id := getTimerID()
	r := new_timer_record(c, id)
	r.parent = l
	l.PushBack(r)

	cancel_map[id] = r

	if t < earliestTime {
		earliestTime = t
	}

	return id, c
}

func CancelInaccuracyTimer(id int64)  {
	cancel, ok := cancel_map[id]
	if ok {
		for e := cancel.parent.Front(); e != nil; e = e.Next() {
			tr, ok := e.Value.(*timer_record)
			if ok && tr.id == id {
				fmt.Println("find cancel id")
				go func() {
					tr.ch <- int64(0)
				}()
				cancel.parent.Remove(e)
				break
			}
		}
		cancel.parent = nil
		delete(cancel_map, id)
	}
}

func runTimer() {
	t := time.NewTicker(time.Second)
	for _ = range t.C {
		currentTime := time.Now().Unix()

		for i := earliestTime; i <= currentTime; i ++ {
			l := timerMap[i]
			if l != nil {
				delete(timerMap, i)
				for e := l.Front(); e != nil; e = e.Next() {
					ch, e := e.Value.(*timer_record)
					if e {
						ch.ch <- currentTime
					}
				}
			}
		}
		earliestTime = currentTime + 1
	}
}

/**
	使用callback的方式设置定时器
 */
func NewInaccuracyTimerCallback(time time.Duration, callbackID func(int64), callback func(int64)) {
	go func() {
		id, ch := NewInaccuracyTimer(time)
		callbackID(id)
		t := <- ch
		callback(t)
	}()
}

var id_ int64 = 0
func getTimerID() int64 {
	id_ ++
	return id_
}