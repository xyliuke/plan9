package logger

import (
	"fmt"
	"testing"
	"time"
)

func Test_WriteLog(t *testing.T) {
	// SetLogPath("./../")
	// AddLog("info", "app_info.log")
	// PLog("info").LogI("msg test test")
	// PLog("info").LogI("msg test test 12323213")
	// PLog("info").LogW("msg test test 12323213")
	// PLog("info").LogE("msg test test 12323213")
}

func say(s string) {
	for i := 0; i < 5; i++ {
		time.Sleep(100 * time.Millisecond)
		fmt.Println(s)
	}
}
