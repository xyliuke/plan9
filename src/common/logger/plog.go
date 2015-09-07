package logger

import (
	"bytes"
	"fmt"
	"os"
	"strconv"
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

func (log Logger) LogW(msg string) {
	log.writeLog2File(msg, "warn")
}

func (log Logger) LogE(msg string) {
	fmt.Println("loge")
	log.writeLog2File(msg, "error")
}

func PLog(logType string) *Logger {
	logger, ok := getWriteByLog(logType)
	if ok {
		return logger
	}
	fmt.Println("Plog", logger)
	return nil
}

func (log *Logger) writeLog2File(msg string, level string) {
	t := time.Now()
	var buf bytes.Buffer
	buf.WriteString("[")
	buf.WriteString(t.Format("2006-01-02 15:04:05."))
	buf.WriteString(strconv.Itoa(t.Nanosecond()))
	buf.WriteString("]")
	buf.WriteString(level)
	buf.WriteString(":\t")
	buf.WriteString(msg)
	buf.WriteString("\n")
	buf.WriteTo(log.file)
}

func getWriteByLog(logType string) (*Logger, bool) {
	logger, ok := loggerMap[logType]
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
