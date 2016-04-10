package compress

import (
	"testing"
	"fmt"
)

func Test_compress(t *testing.T)  {
	s := []byte("hello world");
	c := Compress(s, 10);
	fmt.Println(c, len(c))
	s1 := string(DeCompress(c, 1));
	fmt.Println(s1)
}
