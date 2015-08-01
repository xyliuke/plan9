package logger

import (
	"log"
)

func LogInfo(msg string) {
	log.Println(msg)
}

func LogWarn(msg string) {
	log.Println(msg)
}

func LogFatal(msg string) {
	log.Println(msg)
}
