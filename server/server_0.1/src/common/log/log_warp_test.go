package log

import (
	"testing"
	"time"
)

func Test_Log(t *testing.T)  {
	//SetLogFolder("./")
	//EnableOutputTerminal(false)
	SetLogLevel(LOG_WARN)
	I("123a", 2, 3.2, 13333)
	E("123a", 2, 3.2)
	W(1,2,3,4,5)
	//Logger().Println("123a", 2, 3.2)
	//Logger().Println("123a 1 2", 2, 3.2)
	//log.Println("dddddxxxxx")
	I(time.Now().Format("2006-01-02"), time.Now().Hour())


	E_NET("hello world")
	E_COM("hello world")
	E_OTH("hello world")
}
