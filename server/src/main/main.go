package main

import (
	"fmt"
	"io"
	"net/http"
	"runtime"
)

func op(rw http.ResponseWriter, req *http.Request) {
	// fmt.Println("hi")

	// req.ParseForm()

	// fmt.Print("url:\t")
	// fmt.Println(req.URL.Path)

	// fmt.Print("form:\t")
	// fmt.Println(req.Form)

	// for k, v := range req.Form {
	// fmt.Println("key:", k, "value:", v)
	// }

	// io.WriteString(rw, "hi!\n")
}

func handler_hi(rw http.ResponseWriter, req *http.Request) {
	go op(rw, req)
}

func handler_hello(rw http.ResponseWriter, req *http.Request) {
	fmt.Println("hello")

	fmt.Print("scheme:\t")
	fmt.Println(req.URL.Scheme)

	fmt.Print("form:\t")
	fmt.Println(req.Form)

	fmt.Print("postform:\t")
	fmt.Println(req.PostForm)

	fmt.Print("trailer:\t")
	fmt.Println(req.Trailer)

	fmt.Print("url:\t")
	fmt.Println(req.URL.Path)
	fmt.Println(req.URL.RawPath)
	fmt.Println(req.URL.Query().Get("a"))
	fmt.Println(req.URL.Query().Get("b"))

	io.WriteString(rw, "hello\n")
}

func main() {
	fmt.Println("设置", runtime.NumCPU(), "核")
	runtime.GOMAXPROCS(runtime.NumCPU())
	http.HandleFunc("/hello", handler_hello)
	http.HandleFunc("/hi", handler_hi)
	http.ListenAndServe(":8080", nil)
}
