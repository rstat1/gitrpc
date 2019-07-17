package main

import (
	"net/http"

	"git.m/devapp/common"
	"git.m/gitrpc-frontend/gitrpc"
	"github.com/husobee/vestigo"
)

var (
	server *gitrpc.GitServer
)

func main() {
	common.CommonProcessInit(false, false)
	server = gitrpc.NewGitServer()
	router := vestigo.NewRouter()
	common.LogInfo("", "", "starting git listener...")
	router.Handle("/git/*", ValidateRequest(common.Nothing, gitrouter))
	err := http.ListenAndServe("localhost:8080", router)
	if err != nil {
		common.LogError("", err)
	}
}
func auth(user, pass string) (bool, error) {
	return true, nil
}
func gitrouter(resp http.ResponseWriter, request *http.Request) {
	server.GitHTTPHandler(resp, request)
}

//ValidateRequest ...
func ValidateRequest(validator func(*http.Request) common.APIResponse, handler func(http.ResponseWriter, *http.Request)) http.HandlerFunc {
	return http.HandlerFunc(func(writer http.ResponseWriter, request *http.Request) {
		if resp := validator(request); resp.Status == "success" {
			handler(writer, request)
		} else {
			common.WriteAPIResponseStruct(writer, resp)
		}
	})
}
