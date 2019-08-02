package gitrpc

import (
	context "context"
	fmt "fmt"
	"io"
	"log"
	"net/http"
	"os"
	"runtime"
	"strings"

	"git.m/devapp/common"
	"git.m/gitrpc-frontend/nexus"
	"github.com/bargez/pktline"
	grpc "google.golang.org/grpc"
	"gopkg.in/src-d/go-billy.v4/osfs"
	"gopkg.in/src-d/go-git.v4"
	"gopkg.in/src-d/go-git.v4/plumbing"
	"gopkg.in/src-d/go-git.v4/plumbing/protocol/packp"
	"gopkg.in/src-d/go-git.v4/plumbing/protocol/packp/capability"
	"gopkg.in/src-d/go-git.v4/plumbing/transport"
	storage "gopkg.in/src-d/go-git.v4/storage"
	"gopkg.in/src-d/go-git.v4/storage/filesystem"
)

var (
	logger *log.Logger
)

//RepositoryInfo ...
type RepositoryInfo struct {
	RepoName string
	Storage  storage.Storer
	HeadRef  string
}

//GitServer ...
type GitServer struct {
	// dataStore   *data.DataStore
	// gitHost     *GitHost
	CurrentRepo RepositoryInfo
	empty       transport.AuthMethod
	rpc         *nexus.RPCClient
}

//NewGitServer Do I really have to explain everything to you?
func NewGitServer() *GitServer {
	logger = log.New(os.Stdout, "devcentral", log.Lshortfile)
	client := nexus.NewGitRPCClient()
	client.Connect()
	return &GitServer{
		rpc: client,
	}
}

//GitHTTPHandler Is the name not descriptive enough?
func (server *GitServer) GitHTTPHandler(resp http.ResponseWriter, request *http.Request) common.APIResponse {
	repoName := server.getRepoName(request.URL.Path)
	if repoName != "" {
		enc := pktline.NewEncoder(resp)
		s, e := filesystem.NewStorage(osfs.New("/home/rstat1/Apps/test/new-server"))
		if e != nil {
			panic(e)
		}
		server.CurrentRepo = RepositoryInfo{
			RepoName: repoName,
			Storage:  s,
		}

		if request.Method == "GET" {
			server.getRequestHandler(request, resp, enc)
		} else if request.Method == "POST" {
			server.postRequestHandler(request, resp, enc, repoName)
		}
	}
	return common.APIResponse{}
}

func (server *GitServer) listRefs(resp http.ResponseWriter, enc *pktline.Encoder) {
	if repo, err := git.PlainOpen("/home/rstat1/Apps/test/new-server"); err == nil {
		refList := packp.NewAdvRefs()
		refList.Capabilities.Add(capability.OFSDelta)
		refList.Capabilities.Add(capability.DeleteRefs)
		refList.Capabilities.Add(capability.Agent, "devapp/0.0.1")
		refList.Capabilities.Add(capability.ReportStatus)

		refs, _ := repo.References()

		err = refs.ForEach(func(ref *plumbing.Reference) error {
			refList.AddReference(ref)
			return nil
		})
		refList.Encode(resp)
	} else {
		common.LogError("", fmt.Errorf("listrefs error: %s", err.Error()))
	}
}
func (server *GitServer) getRepoName(requestURL string) string {
	urlParts := strings.Split(requestURL, "/")
	return urlParts[2]
}

// func (server *GitServer) decodeWantsAndHaves(details io.ReadCloser) ([]plumbing.Hash, []plumbing.Hash) {
// 	var line []byte
// 	var lineParts []string
// 	var wants, haves []plumbing.Hash

// 	decode := pktline.NewDecoder(details)

// 	for {
// 		if err := decode.Decode(&line); err != nil {
// 			return wants, haves
// 		} else if len(line) == 0 {
// 			continue
// 		} else {
// 			line = line[:len(line)-1]
// 		}
// 		lineParts = strings.Split(string(line), " ")

// 		if lineParts[0] == "want" {
// 			println(fmt.Sprintf("want %s\n", string(line)))
// 			wants = append(wants, plumbing.NewHash(lineParts[1]))
// 		} else if lineParts[0] == "have" {
// 			println(fmt.Sprintf("have %s", string(line)))
// 			haves = append(haves, plumbing.NewHash(lineParts[1]))
// 		}
// 	}
// 	return nil, nil
// }
func (server *GitServer) getRequestHandler(request *http.Request, resp http.ResponseWriter, enc *pktline.Encoder) {
	serviceName := request.URL.Query().Get("service")
	resp.Header().Add("Content-Type", fmt.Sprintf("application/x-%s-advertisement", serviceName))
	resp.WriteHeader(200)

	enc.Encode([]byte(fmt.Sprintf("# service=%s\n", serviceName)))
	enc.Encode(nil)

	common.LogDebug("", "", serviceName)
	switch serviceName {
	// case "git-upload-pack":
	// 		server.listRefsForClone(resp, enc)
	// 		break
	case "git-receive-pack":
		server.listRefs(resp, enc)
		break
	}
}
func (server *GitServer) postRequestHandler(request *http.Request, resp http.ResponseWriter, enc *pktline.Encoder, repoName string) {
	arr := strings.Split(request.URL.Path, "/")
	switch arr[3] {
	case "git-upload-pack":

		break
	case "git-receive-pack":
		common.LogDebug("pack size", request.Header.Get("Content-Length"), "hi")
		server.packUpload(request.Body, enc)
		request.Body.Close();
		runtime.GC();
		// server.dataStore.UpdateProjectMRU(repoName)
		break
	}
}

func (server *GitServer) packUpload(reader io.ReadCloser, enc *pktline.Encoder) {
	var lines [][]byte
	var lineStr string
	var lineParts []string
	var parsedRefNames []string
	var parsedRefs map[string]string
	decoder := pktline.NewDecoder(reader)
	parsedRefs = make(map[string]string)
	if err := decoder.DecodeUntilFlush(&lines); err == nil {
		for _, line := range lines {
			lineStr = string(line)
			common.LogDebug("", "", lineStr)
			lineParts = strings.Split(lineStr, " ")
			if len(lineParts) < 3 {
				common.LogWarn("", "", "less than 3 parts")
				continue
			}
			refName := strings.TrimSuffix(lineParts[2], string("\x00"))
			parsedRefNames = append(parsedRefNames, refName)
			parsedRefs[refName] = lineParts[1]
		}
		if server.unbufferedWriteStream(reader, enc) == true {
			server.writeReferences(enc, parsedRefNames, parsedRefs)
		}
	}
}

// func (server *GitServer) unbufferedWriteStream(reader io.ReadCloser, enc *pktline.Encoder) bool {
// 	var pakToWrite []byte
// 	var wroteToStream bool
// 	if stream, err := server.rpc.ReceivePackStream(context.Background(), grpc.FailFast(true)); err != nil {
// 		enc.Encode([]byte(fmt.Sprintf("unpack %s\n", err.Error())))
// 		enc.Encode(nil)
// 		common.LogError("", err)
// 		return false
// 	} else {
// 		chunkSize := 2 * 1024 * 1024
// 		pak := make([]byte, chunkSize)

// 		for {
// 			if n, _ := reader.Read(pak); n != 0 {
// 				if n < chunkSize {
// 					pakToWrite = pak[0:n]
// 				} else if n == chunkSize {
// 					pakToWrite = pak
// 				}
// 				if err := stream.Send(&nexus.ReceivePackRequest{RepoName: "new-server", Data: pakToWrite}); err != nil {
// 					enc.Encode([]byte(fmt.Sprintf("unpack %s\n", err.Error())))
// 					enc.Encode(nil)
// 					common.LogError("", err)
// 					reader.Close()
// 				} else {

// 					if resp, err := stream.Recv(); err != nil || resp.Success == false {
// 						common.LogError("", err)
// 						return false
// 					}
// 					wroteToStream = true
// 				}

// 			} else {
// 				break
// 			}
// 		}
// 		if e := stream.CloseSend(); err != nil {
// 			enc.Encode([]byte(fmt.Sprintf("unpack %s\n", e.Error())))
// 			enc.Encode(nil)
// 			return false
// 		}
// 		if wroteToStream {
// 			enc.Encode([]byte("unpack ok\n"))
// 			enc.Encode(nil)
// 		} else {
// 			return false
// 		}
// 	}
// 	return true
// }
func (server *GitServer) unbufferedWriteStream(reader io.ReadCloser, enc *pktline.Encoder) bool {
	var err error
	var pakToWrite []byte
	var wroteToStream bool
	var stream nexus.GitService_ReceivePackStreamClient
	chunkSize := 2 * 1024 * 1024
	pak := make([]byte, chunkSize)
	for {
		if n, _ := reader.Read(pak); n != 0 {
			if n < chunkSize {
				pakToWrite = pak[0:n]
			} else if n == chunkSize {
				pakToWrite = pak
			}
			if stream == nil {
				common.LogDebug("", "", "connecting..")
				stream, err = server.rpc.ReceivePackStream(context.Background(), grpc.FailFast(true))
				if err != nil {
					enc.Encode([]byte(fmt.Sprintf("unpack %s\n", err.Error())))
					enc.Encode(nil)
					common.LogError("", err)
					return false
				}
			}
			wroteToStream = server.writeToStream(stream, pakToWrite, "new-server", enc, false)

		} else {
			break
		}
	}
	pak = nil
	if wroteToStream {
		common.LogDebug("", "", "stream done wrting ender")
		if !server.writeToStream(stream, nil, "new-server", enc, true) {
			return false
		}
		enc.Encode([]byte("unpack ok\n"))
		enc.Encode(nil)
	} else {
		common.LogWarn("", "", "didn't writeToStream")
		return false
	}
	if e := stream.CloseSend(); e != nil {
		enc.Encode([]byte(fmt.Sprintf("unpack %s\n", e.Error())))
		enc.Encode(nil)
		return false
	}

	return true
}

func (server *GitServer) writeToStream(stream nexus.GitService_ReceivePackStreamClient, data []byte, repoName string, enc *pktline.Encoder, done bool) bool {
	if err := stream.Send(&nexus.ReceivePackRequest{RepoName: "new-server", Data: data, Done: done}); err != nil {
		enc.Encode([]byte(fmt.Sprintf("unpack %s\n", err.Error())))
		enc.Encode(nil)
		common.LogError("", err)
		return false
	} else {
		if resp, err := stream.Recv(); err != nil || resp.Success == false {
			common.LogError("", err)
			return false
		}
		return true
	}
}
func (server *GitServer) unbufferedWriteNoStream(reader io.ReadCloser, enc *pktline.Encoder) bool {
	var pakToWrite []byte
	var wroteToStream bool
	chunkSize := 2 * 1024 * 1024
	pak := make([]byte, chunkSize)
	for {
		if n, _ := reader.Read(pak); n != 0 {
			if n < chunkSize {
				pakToWrite = pak[0:n]
			} else if n == chunkSize {
				pakToWrite = pak
			}
			msg := nexus.ReceivePackRequest{RepoName: "new-server", Data: pakToWrite}
			if r, err := server.rpc.ReceivePack(context.Background(), &msg, grpc.FailFast(true)); err != nil || r.GetSuccess() == false {
				enc.Encode([]byte(fmt.Sprintf("unpack %s\n", err.Error())))
				enc.Encode(nil)
				common.LogError("", err)
				return false
			}
			wroteToStream = true
		} else {
			common.LogDebug("", "", "reached the end")
			break
		}
	}
	if wroteToStream {
		common.LogInfo("", "", "finished write")
		enc.Encode([]byte("unpack ok\n"))
		enc.Encode(nil)
	} else {
		return false
	}
	reader.Close()
	return true
}

func (server *GitServer) bufferedWrite(reader io.ReadCloser, enc *pktline.Encoder) bool {
	// pak, err := ioutil.ReadAll(reader)
	// if err != nil {
	// 	panic(err)
	// }
	// var chunk []byte
	// if len(pak) > 0 {
	// 	chunkSize := 64 * 1024
	// 	if stream, err := server.rpc.ReceivePackStream(context.Background(), grpc.FailFast(true)); err == nil {
	// 		for currentByte := 0; currentByte < len(pak); currentByte += chunkSize {
	// 			if currentByte+chunkSize > len(pak) {
	// 				chunk = pak[currentByte:len(pak)]
	// 			} else {
	// 				chunk = pak[currentByte : currentByte+chunkSize]
	// 			}
	// 			if err := stream.Send(&nexus.ReceivePackRequest{RepoName: "new-server", Data: chunk}); err != nil {
	// 				common.LogError("", err)
	// 				reader.Close()
	// 				break
	// 			}
	// 		}
	// 		reader.Close()
	// 		resp, err := stream.CloseAndRecv()
	// 		common.LogDebug("resp", resp, "response")
	// 		if err != nil || resp.Success == false {
	// 			common.LogError("", err)
	// 		} else {
	// 		}
	// 		enc.Encode([]byte("unpack ok\n"))
	// 		enc.Encode(nil)
	// 		return true
	// 	}
	// 	enc.Encode([]byte(fmt.Sprintf("unpack %s\n", err.Error())))
	// 	enc.Encode(nil)
	// 	return false
	// }
	return false
}
func (server *GitServer) writeReferences(enc *pktline.Encoder, parsedRefNames []string, parsedRefs map[string]string) bool {
	for _, ref := range parsedRefNames {
		_, e2 := server.rpc.WriteReference(context.Background(), &nexus.WriteReferenceRequest{
			RepoName: "new-server", //server.CurrentRepo.RepoName,
			RefName:  strings.TrimSuffix(ref, string([]byte{0})),
			RefRev:   parsedRefs[ref],
		})
		if common.LogError("", e2) != nil {
			enc.Encode([]byte(fmt.Sprintf("ng %s %s\n", strings.TrimSuffix(ref, string([]byte{0})), e2.Error())))
			return false
		} else {
			enc.Encode([]byte(fmt.Sprintf("ok %s\n", strings.TrimSuffix(ref, string([]byte{0})))))
		}
	}
	return true
}

/*
 */
