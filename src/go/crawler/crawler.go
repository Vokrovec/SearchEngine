package main

import (
	"fmt"
	"io"
	"net/http"
)

func fetch(url string) (string, error) {
	resp, err := http.Get(url)
	if err != nil {
		return "", err
	}
	defer resp.Body.Close()

	data, err := io.ReadAll(resp.Body)
	if err != nil {
		return "", err
	}

	return string(data), nil
}

func main() {
	html, err := fetch("www.wikipedia.org")

	if err != nil {
		panic(err)
	}

	fmt.Println(html)
}
