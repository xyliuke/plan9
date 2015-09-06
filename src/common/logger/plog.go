package logger

import (
	"fmt"
	_ "io"
	"os"
	"time"
)

type Logger struct {
	file     *os.File
	fileName string
	day      int
}

var logPath = "./"

//log类型和对应文件的映射
var logFileMap = make(map[string]*os.File)

//log类型和对应文件名的映射
var logTypeFileMap = make(map[string]string)

var loggerMap = make(map[string]*Logger)

func AddLog(logType string, fileName string) {
	// logTypeFileMap[logType] = fileName
	logger, ok := loggerMap[logType]
	if ok {
		logger.fileName = fileName
	} else {
		logger := new(Logger)
		logger.fileName = fileName
		loggerMap[logType] = logger
	}
}

func (log Logger) LogI(msg string) {
	log.writeLog2File(msg, "info")
}

func LogW() {

}

func LogE() {

}

func PLog(logType string) *Logger {
	logger, ok := getWriteByLog(logType)
	if ok {
		return logger
	}
	return nil
}

func (log *Logger) writeLog2File(msg string, level string) {
	log.file.WriteString("[" + time.Now().String() + "]" + level + ":\t" + msg + "\n")
}

func getWriteByLog(logType string) (*Logger, bool) {
	logger, ok := loggerMap[logType]
	fmt.Println("log file map :", logger.file, ok)
	if ok {
		if logger.file == nil || logger.isShouldCreateNewLogFile() {
			file, err := os.OpenFile(getLogFileAbsoultPath(logger.fileName), os.O_RDWR|os.O_APPEND|os.O_CREATE, os.ModePerm)
			if err == nil {
				logger.file = file
				return logger, true
			} else {
				return logger, false
			}
		} else {
			return logger, true
		}
	} else {
		return logger, false
	}
}

func (log *Logger) isShouldCreateNewLogFile() bool {
	t := time.Now()
	if t.Day() != log.day {
		log.day = t.Day()
		return false
	}
	return true
}

func getLogFileAbsoultPath(fileName string) string {
	return logPath + time.Now().Format("2006-01-02") + "_" + fileName
}
