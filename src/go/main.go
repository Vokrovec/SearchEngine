package main

/*
#cgo CFLAGS: -I../../include
#cgo LDFLAGS: -L../../build -lsearch -lstdc++

#include "search.h"
#include <stdlib.h> 
*/
import "C"
import (
	"fmt"
	"os"
	"main/crawler"
	"unsafe"
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
	case "tokenize":
		inputFolderName := C.CString("./urls")
		outputFolderName := C.CString("./tokenized")
		defer C.free(unsafe.Pointer(inputFolderName))
		defer C.free(unsafe.Pointer(outputFolderName))
		C.tokenize_folder(inputFolderName, outputFolderName)
	default:
		fmt.Println("Wrong argument!")
	}
}
