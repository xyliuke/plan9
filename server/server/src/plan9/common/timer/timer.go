package timer

import (
	"time"
	"plan9/common/util"
)

//timer内部存储类
type timerUnit struct  {
	id string
	action func()
	ch chan bool
}

func newTimerUnit(action func()) *timerUnit {
	unit := new(timerUnit)
	unit.action = action
	unit.id = util.CreateID()
	unit.ch = make(chan bool)
	return unit
}

type Timer struct  {
	tick *time.Ticker
	c map[string]*timerUnit
}

var timer *Timer
func GetTimerInstance() *Timer {
	if timer == nil {
		timer = new(Timer)
	}
	return timer
}

//创建一个Timer,参数为Timer到期后的执行函数
func (self *Timer) CreateTimer(action func()) string {
	unit := newTimerUnit(action)
	return unit.id
}

//设置延迟时间,单位为毫秒
func (self *Timer) SetDelay(delaySeconds int)  {

}
