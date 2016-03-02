package config

import (
	"testing"
	"time"
	"fmt"
)

func Test_Timer(t *testing.T)  {
	//timer := time.NewTimer(time.Second * 5)
	//select {
	//case <-timer.C:
	//	fmt.Println("timer run")
	//}

	i := 0
	ticker := time.NewTicker(time.Second * 1)
	for {
		select {
		case <-ticker.C:
			fmt.Println("ticker run")
			i ++
		}
		if i > 4 {
			ticker.Stop()
			break
		}
	}
}