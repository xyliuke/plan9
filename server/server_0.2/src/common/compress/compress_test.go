package compress

import (
	"fmt"
	"testing"
)

func Test_compress(t *testing.T) {
	s := []byte("hello")
	fmt.Println("ori", s)
	c := Compress(s, 5)
	fmt.Println(c, len(c))
	s1 := string(DeCompress(c, len(c)))
	fmt.Println(s1)

	dc := []byte{120, 218, 203, 72, 205, 201, 201, 7, 0, 6, 44, 2, 21}
	fmt.Println("hello:",DeCompress(dc, len(dc)))
}
