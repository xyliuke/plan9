package compress

import (
	"bytes"
	"io"
	"compress/flate"
)

/**
 *	压缩数据
 *	src 为源数据
 *	length 源数据长度
 *	返回压缩后的数据数组,数据长度
 */
func Compress(src []byte, length int) ([]byte) {
	realSrc := src[:length]
	var in bytes.Buffer
	w, err := flate.NewWriter(&in, flate.BestCompression)
	if err == nil {
		w.Write(realSrc)
		w.Close()
		return in.Bytes()
	}
	return nil
}

func DeCompress(src []byte, length int) ([]byte) {
	realSrc := src[:length]
	b := bytes.NewReader(realSrc)
	var out bytes.Buffer
	r := flate.NewReader(b)
	io.Copy(&out, r)
	return out.Bytes()
}
