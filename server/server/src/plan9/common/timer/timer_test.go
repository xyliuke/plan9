package timer

import (
	"testing"
	//"time"
	"fmt"
)

func Test_Timer(t *testing.T)  {
	//ch := make(chan string)

	//for i := 0; i < 10; i++ {
	//	go func(index int) {
	//		fmt.Println("begin", index)
	//		b := <-ch
	//		fmt.Println("end", index, b)
	//	}(i)
	//}

	//time.Sleep(1 * time.Second)
	//
	//close(ch)

	var f func() = func() {
		fmt.Println("lambda")
	}
	f()
}
