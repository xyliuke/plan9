package http

import (
	"net/http"
)

type http_client struct {
}

func (h *http_client) Get(url string) (string, bool) {
	resp, err := http.Get(url)
	if err != nil {
		return "", false
	}
	
	return , true
}
