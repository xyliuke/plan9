package main

import (
	_ "bufio"
	_ "fmt"
//	"io"
//	"log"
//	"net"
//	"os"
	// "net/http"
//	 "runtime"
	"server/tcp"
)

func main() {

	t := tcp.CreateTcpServer(0)
	t.Listen()

//	fmt.Println(c)
//	listener, err := net.Listen("tcp", ":8888")
//	if err != nil {
//		fmt.Println("listen error", err)
//	}
//
//	defer listener.Close()
//	for {
//		conn, err := listener.Accept()
//		if err != nil {
//			fmt.Println("Error accepting: ", err)
//			os.Exit(1)
//		}
//		//logs an incoming message
//		fmt.Printf("Received message %s -> %s \n", conn.RemoteAddr(), conn.LocalAddr())
//		// Handle connections in a new goroutine.
//		go handleRequest(conn)
//		// go doServerStuff(conn)
//	}
}

//func handleRequest(conn net.Conn) {
//	defer conn.Close()
//	for {
//		io.Copy(conn, conn)
//	}
//}

//处理客户端消息
//func doServerStuff(conn net.Conn) {
//	nameInfo := make([]byte, 512) //生成一个缓存数组
//	_, err := conn.Read(nameInfo)
//	checkError(err)
//
//	for {
//		buf := make([]byte, 512)
//		_, err := conn.Read(buf) //读取客户机发的消息
//		flag := checkError(err)
//		if flag == 0 {
//			break
//		}
//		fmt.Println(string(buf)) //打印出来
//	}
//}

//检查错误
//func checkError(err error) int {
//	if err != nil {
//		if err.Error() == "EOF" {
//			fmt.Println("用户退出了")
//			return 0
//		}
//		log.Fatal("an error!", err.Error())
//		return -1
//	}
//	return 1
//}
