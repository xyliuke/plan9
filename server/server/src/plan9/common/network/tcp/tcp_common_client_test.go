package tcp
import (
	"testing"
	"fmt"
)

type TT struct {
	a int
	b float32
}

var co TT = TT{a : 1, b : 2.9}

func Test_Client(t *testing.T)  {
	fmt.Println("hello world", co.a)
}
