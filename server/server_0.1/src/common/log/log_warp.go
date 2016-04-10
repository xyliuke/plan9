package log

import (
	"log"
	"os"
	"time"
	"fmt"
)

var logger_ *log.Logger = nil
var logger_network_ *log.Logger = nil
var logger_common_ *log.Logger = nil
var logger_other_ *log.Logger = nil

var logfile_ *os.File = nil
var logfile_network_ *os.File = nil
var logfile_common_ *os.File = nil
var logfile_other_ *os.File = nil
var day int = -1
var path_ string = "./"
var log_level int = LOG_DEBUG
var log_terminal_enable bool = true
var log_file_enable bool = false

const (
	LOG_DEBUG = 0
	LOG_INFO = 1
	LOG_WARN = 2
	LOG_ERROR = 3
)


func D(args ... interface{})  {
	if log_level <= LOG_DEBUG{
		print(" [debug] ", args)
	}
}

func I(args ... interface{})  {
	if log_level <= LOG_INFO {
		print(" [info]  ", args)
	}
}

func W(args ... interface{})  {
	if log_level <= LOG_WARN {
		print(" [warn]  ", args)
	}
}

func E(args ... interface{})  {
	if log_level <= LOG_ERROR {
		print(" [error] ", args)
	}
}


func D_NET(args ... interface{})  {
	if log_level <= LOG_DEBUG{
		print_network(" [debug] ", args)
	}
}

func I_NET(args ... interface{})  {
	if log_level <= LOG_INFO {
		print_network(" [info]  ", args)
	}
}

func W_NET(args ... interface{})  {
	if log_level <= LOG_WARN {
		print_network(" [warn]  ", args)
	}
}

func E_NET(args ... interface{})  {
	if log_level <= LOG_ERROR {
		print_network(" [error] ", args)
	}
}



func D_COM(args ... interface{})  {
	if log_level <= LOG_DEBUG{
		print_common(" [debug] ", args)
	}
}

func I_COM(args ... interface{})  {
	if log_level <= LOG_INFO {
		print_common(" [info]  ", args)
	}
}

func W_COM(args ... interface{})  {
	if log_level <= LOG_WARN {
		print_common(" [warn]  ", args)
	}
}

func E_COM(args ... interface{})  {
	if log_level <= LOG_ERROR {
		print_common(" [error] ", args)
	}
}



func D_OTH(args ... interface{})  {
	if log_level <= LOG_DEBUG{
		print_other(" [debug] ", args)
	}
}

func I_OTH(args ... interface{})  {
	if log_level <= LOG_INFO {
		print_other(" [info]  ", args)
	}
}

func W_OTH(args ... interface{})  {
	if log_level <= LOG_WARN {
		print_other(" [warn]  ", args)
	}
}

func E_OTH(args ... interface{})  {
	if log_level <= LOG_ERROR {
		print_other(" [error] ", args)
	}
}










func print(args ... interface{})  {
	if log_file_enable {
		logger().Println(args)
	}
	printToTerminal(args)
}

func print_network(args ... interface{})  {
	if log_file_enable {
		logger_network().Println(args)
	}
	printToTerminal(args)
}

func print_common(args ... interface{})  {
	if log_file_enable {
		logger_common().Println(args)
	}
	printToTerminal(args)
}

func print_other(args ... interface{})  {
	if log_file_enable {
		logger_other().Println(args)
	}
	printToTerminal(args)
}

func printToTerminal(args ... interface{})  {
	if log_terminal_enable {
		log.Println(args)
	}
}

func logger() *log.Logger {
	if logger_ == nil {
		logger_ = log.New(getLogFile(), "", log.Ldate | log.Ltime | log.Lmicroseconds);
		log.SetFlags(log.Ldate | log.Ltime | log.Lmicroseconds)
	}
	return logger_
}

func logger_network() *log.Logger {
	if logger_network_ == nil {
		logger_network_ = log.New(getLogNetworkFile(), "", log.Ldate | log.Ltime | log.Lmicroseconds);
		log.SetFlags(log.Ldate | log.Ltime | log.Lmicroseconds)
	}
	return logger_network_
}

func logger_common() *log.Logger {
	if logger_common_ == nil {
		logger_common_ = log.New(getLogCommonFile(), "", log.Ldate | log.Ltime | log.Lmicroseconds);
		log.SetFlags(log.Ldate | log.Ltime | log.Lmicroseconds)
	}
	return logger_common_
}

func logger_other() *log.Logger {
	if logger_other_ == nil {
		logger_other_ = log.New(getLogOtherFile(), "", log.Ldate | log.Ltime | log.Lmicroseconds);
		log.SetFlags(log.Ldate | log.Ltime | log.Lmicroseconds)
	}
	return logger_other_
}

func SetLogFolder(path string) {
	path_ = path
	f := path + time.Now().Format("2006-01-02") + ".log"
	day = time.Now().Day()
	logfile,err := os.OpenFile(f, os.O_RDWR | os.O_CREATE | os.O_APPEND, 0666);
	if err != nil {
		fmt.Printf("%s\r\n",err.Error());
		os.Exit(-1)
	}
	logfile_ = logfile

	f1 := path + time.Now().Format("2006-01-02") + "_net.log"
	logfile1, err1 := os.OpenFile(f1, os.O_RDWR | os.O_CREATE | os.O_APPEND, 0666);
	if err1 != nil {
		fmt.Printf("%s\r\n",err.Error());
		os.Exit(-1)
	}
	logfile_network_ = logfile1

	f2 := path + time.Now().Format("2006-01-02") + "_common.log"
	logfile2, err2 := os.OpenFile(f2, os.O_RDWR | os.O_CREATE | os.O_APPEND, 0666);
	if err2 != nil {
		fmt.Printf("%s\r\n",err.Error());
		os.Exit(-1)
	}
	logfile_common_ = logfile2

	f3 := path + time.Now().Format("2006-01-02") + "_other.log"
	logfile3, err3 := os.OpenFile(f3, os.O_RDWR | os.O_CREATE | os.O_APPEND, 0666);
	if err3 != nil {
		fmt.Printf("%s\r\n",err.Error());
		os.Exit(-1)
	}
	logfile_other_ = logfile3
}

func getLogFile() *os.File {
	if logfile_ == nil || time.Now().Day() != day{
		day = time.Now().Day()
		SetLogFolder(path_)
	}
	return logfile_
}

func getLogNetworkFile() *os.File {
	if logfile_network_ == nil || time.Now().Day() != day{
		day = time.Now().Day()
		SetLogFolder(path_)
	}
	return logfile_network_
}

func getLogCommonFile() *os.File {
	if logfile_common_ == nil || time.Now().Day() != day{
		day = time.Now().Day()
		SetLogFolder(path_)
	}
	return logfile_common_
}

func getLogOtherFile() *os.File {
	if logfile_other_ == nil || time.Now().Day() != day{
		day = time.Now().Day()
		SetLogFolder(path_)
	}
	return logfile_other_
}

func SetLogLevel(level int)  {
	log_level = level
}

func EnableOutputTerminal(enable bool)  {
	log_terminal_enable = enable
}

func EnableOutputFile(enable bool)  {
	log_file_enable = enable
}