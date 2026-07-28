package crawler

import (
	"errors"
	"fmt"
	"os"
	"strconv"
	"strings"
	"sync"
	_ "time"

	"github.com/abadojack/whatlanggo"
	"github.com/gocolly/colly"
)

var (
	c *colly.Collector
	maxURLS int 
	visitedSet *VisitedSet
	pageQueue *URLQueue
	documents *Documents
)

type URLQueue struct {
	urls []string
	mu   sync.Mutex
}

type VisitedSet struct {
	visited map[string]bool
	mu sync.Mutex
}

type Document struct {
	Index    uint
	URL      string
	Title    string
	Text     string
}

type Documents struct {
	documents []Document
	newIndex  uint
	mu 				sync.Mutex
}


func newDocuments() *Documents {
	return &Documents{
		documents: make([]Document, 0),
	}
}

func (docs *Documents)getIndex() uint {
	docs.mu.Lock()
	idx := docs.newIndex
	docs.newIndex += 1
	docs.mu.Unlock()
	return idx 
}

func newVisitedSet() (*VisitedSet) {
	return &VisitedSet{
		visited : make(map[string]bool),
	}
}

func newURLQueue() (*URLQueue) {
	return &URLQueue{
		urls : make([]string, 0),
	}
}

func (q *URLQueue)add(url string) {
	q.mu.Lock()
	q.urls = append(q.urls, url)
	q.mu.Unlock()
}

func (q *URLQueue)pop() (string, error){
	q.mu.Lock()
	defer q.mu.Unlock()
	if len(q.urls) == 0 {
		return "", errors.New("Queue is empty, can not pop!")
	}
	task := q.urls[0]
	q.urls = q.urls[1:]
	return task, nil
}

func (q *URLQueue)empty() bool {
	q.mu.Lock()
	isEmpty := len(q.urls) == 0
	q.mu.Unlock()
	return isEmpty
}

func (d *Documents)add(doc Document) {
	d.mu.Lock()
	d.documents = append(d.documents, doc)
	d.mu.Unlock()
}

func init() {
	visitedSet = newVisitedSet()
	maxURLS = 100
	pageQueue = newURLQueue()
	documents = newDocuments()

	c = colly.NewCollector()
	//c.Async = true

	c.OnHTML("title", func(e *colly.HTMLElement) {
		doc := e.Request.Ctx.GetAny("document").(*Document)
		fmt.Println("Page title: ", e.Text)
		doc.Title = e.Text
	})
	c.OnRequest(func(r *colly.Request) {
		fmt.Println("Crawling", r.URL)
		r.Ctx.Put("document", &Document{
			URL: r.URL.String(),
			Index: documents.getIndex(),
		})
	})
	// handle request errors
	c.OnError(func(e *colly.Response, err error) {
		fmt.Println("Request URL:", e.Request.URL, "failed with response:", e.StatusCode, "\nError:", err)
	})
	c.OnHTML("body", func(e *colly.HTMLElement) {
			doc := e.Request.Ctx.GetAny("document").(*Document)
			//e.DOM.Find("script, style, nav, footer, header, aside").Remove()
			text := strings.Join(strings.Fields(e.Text), " ")
			doc.Text = text
			//fmt.Println(text)
	})

	c.OnHTML("a[href]", func(e *colly.HTMLElement) {
		link := e.Request.AbsoluteURL(e.Attr("href"))
		//fmt.Println("Found link: ", link)

		visitedSet.mu.Lock()
		if link == "" || visitedSet.visited[link] {
			visitedSet.mu.Unlock()
			return
		}
		visitedSet.visited[link] = true
		visitedSet.mu.Unlock()
		pageQueue.add(link)
	})
	c.OnScraped(func(r *colly.Response) {
		doc := r.Ctx.GetAny("document").(*Document)
		if (!isEnglish(doc.Text)) {
			return
		}
		documents.add(*doc)
		fmt.Println("Document saved: ", doc.Title)
	})
}


func Crawl(page string) {
	pageQueue.add(page)
	for !pageQueue.empty() {
		link, err := pageQueue.pop()
		if err != nil {
			fmt.Println("Error:", err)
		}
		// visit the seed URL
		err = c.Visit(link)
		fmt.Println("Lenght of documnets: ", len(documents.documents))
		if err != nil {
				fmt.Println("Error visiting page:", err)
		}
		if len(documents.documents) >= 5 {
			for i, doc := range documents.documents {
				fmt.Println("Saving to disc:", i)
				save_to_disc(&doc)
			}
			documents.documents = make([]Document, 0) 
		}
	}
}

func isEnglish(text string) bool {
	info := whatlanggo.Detect(text)

	return info.Lang.Iso6391() == "en"
}

func save_to_disc(doc *Document) error {
	file, err := os.OpenFile("urls/" + strconv.FormatUint(uint64(doc.Index), 10), os.O_CREATE|os.O_WRONLY, 0644)
	if err != nil {
		fmt.Println("Error ocured while writing:", err)
		return err
	}
	defer file.Close()
	fmt.Fprintf(file, "Title: %s\n", doc.Title)
	fmt.Fprintf(file, "URL: %s\n", doc.URL)
	fmt.Fprintf(file, "Text: %s\n", doc.Text)
	return err
}
