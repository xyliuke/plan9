package config

import (
	"testing"
	"fmt"
	"server/protocol"
)

func Test_config(t *testing.T) {
	config := new(MinorConfigEntity)
	config.clientID = 1
	config.majorAddress = "127.0.0.1:9001"
	config.serverPort = 9002
	config.microServerType = protocol.PROTOCOL_SESSION_SERVER_TYPE
	config.microServerNum = 2
	NewMinorConfig(config).Run()

	NewMajorConfig(9001).Run()

	ch := make(chan int)
	<- ch
}

func Test_Protocol(t *testing.T) {
	fmt.Println("minor 2 major identify byte:", Minor2MajorIdentify(12, 0x10))
	fmt.Println("minor 2 major identify string:", string(Minor2MajorIdentify(12, 0x10)))


	fmt.Println("minor 2 major update identify byte:", Minor2MajorUpdateMicroServerNum (12, 3, []string{"127.0.0.1:9999", "127.0.0.1:9998"}))
	fmt.Println("minor 2 major update identify string:", string(Minor2MajorUpdateMicroServerNum (12, 3, []string{"127.0.0.1:9999", "127.0.0.1:9998"})))
}