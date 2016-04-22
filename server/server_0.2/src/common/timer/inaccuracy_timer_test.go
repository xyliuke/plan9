package timer

import (
	"fmt"
	"testing"
	"time"
)

func Test_Timer(t *testing.T) {

	//for i := 0; i < 10; i ++ {
	//	id, t1 := NewInaccuracyTimer(time.Second * 5)
	//	CancelInaccuracyTimer(id)
	//	xx := <- t1
	//	fmt.Println("run t1", xx, " id ", id)
	//if i == 1 {

	//}
	//}

	//for i := 0; i < 10; i ++ {
	NewInaccuracyTimerCallback(time.Second, func(id int64) {
		CancelInaccuracyTimer(id)
	}, func(time int64) {
		fmt.Println("run t2 ", time)
	})
	fmt.Println("ddddddddd")

	//}

}
