package main

/*
#cgo CFLAGS: -I../../include
#cgo LDFLAGS: -L../../build -lsearch -lstdc++

#include "search.h"
*/
import "C"

func main() {
	C.hello_world()
}
