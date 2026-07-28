package main

/*
#cgo CFLAGS: -I../../include
#cgo LDFLAGS: -L../../build -lsearch -lstdc++

#include "search.h"
*/
import "C"
import (
	"fmt"
	"os"
	"main/crawler"
)

func main() {
	if len(os.Args) != 2 {
		fmt.Println("Wrong number of arguments!")
		return 
	}
	switch os.Args[1] {
	case "hello":
		C.hello_world()
	case "crawl":
		crawler.Crawl("https://en.wikipedia.org/wiki/Alan_Turing")
	default:
		fmt.Println("Wrong argument!")
	}
}
