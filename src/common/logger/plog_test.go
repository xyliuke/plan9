package logger

import (
	"testing"
)

func Test_WriteLog(t *testing.T) {
	AddLog("info", "app_info.log")
	PLog("info").LogI("msg test test")
	PLog("info").LogI("msg test test 12323213")
	PLog("info").LogW("msg test test 12323213")
	PLog("info").LogE("msg test test 12323213")
}
