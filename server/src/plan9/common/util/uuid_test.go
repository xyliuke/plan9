package util

import (
	"fmt"
	"testing"
)

func Test_CreateID(t *testing.T) {
	var id string
	for i := 0; i < 1; i ++ {
		id = CreateID()

		fmt.Println(id)
	}
}

func Test_CreateDataID(t *testing.T)  {
	for i := 0; i < 100; i ++ {
		fmt.Println(CreateDataID(0x01))
	}
}

func Benchmark_CreateID(b *testing.B) {
	var id string
	for i := 0; i < 50000; i++ {
		id = CreateID()
	}
	fmt.Println(id)
}

func Benchmark_CreateID2(b *testing.B)  {

}

func Test_UUID(t *testing.T) {
	id := NewUUID()
	fmt.Println("uuid : ", id)
}
