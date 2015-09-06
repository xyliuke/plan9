package logger

import (
	"testing"
)

func Test_WriteLog(t *testing.T) {
	AddLog("info", "info.log")
	PLog("info").LogI("msg test test")
	PLog("info").LogI("msg test test 12323213")
}
