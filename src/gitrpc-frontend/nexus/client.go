package nexus

//go:generate protoc --version
//go:generate protoc -I. GitService.proto --go_out=plugins=grpc:.

import (
	context "context"

	"git.m/devapp/common"
	grpc_logrus "github.com/grpc-ecosystem/go-grpc-middleware/logging/logrus"
	"github.com/sirupsen/logrus"
	"google.golang.org/grpc"
)

//RPCClient ...
type RPCClient struct {
	connection *grpc.ClientConn
}

//NewGitRPCClient ...
func NewGitRPCClient() *RPCClient {
	return &RPCClient{}
}

//Connect ...
func (rpc *RPCClient) Connect() {
	var err error
	entry := logrus.NewEntry(common.Logger)
	grpc_logrus.ReplaceGrpcLogger(entry)

	grpc.EnableTracing = true

	// rpc.connection, err = grpc.Dial("passthrough:///unix:///home/rstat1/Apps/nexus-git/out-x86_64/nexus_git_service", grpc.WithInsecure(), grpc.WithBlock())
	rpc.connection, err = grpc.Dial("localhost:9001", grpc.WithInsecure(), grpc.WithBlock())
	if err != nil {
		panic(err)
	}
	common.LogDebug("", "", "connected...")
}

//InitRepository ...
func (rpc *RPCClient) InitRepository(ctx context.Context, in *InitRepositoryRequest, opts ...grpc.CallOption) (*GenericResponse, error) {
	client := NewGitServiceClient(rpc.connection)
	return client.InitRepository(ctx, in, opts...)
}

//ListKnownRefs ...
func (rpc *RPCClient) ListKnownRefs(ctx context.Context, in *ListRefsRequest, opts ...grpc.CallOption) (*ListRefsResponse, error) {
	client := NewGitServiceClient(rpc.connection)
	return client.ListKnownRefs(ctx, in, opts...)
}

//ListRefsForClone ...
func (rpc *RPCClient) ListRefsForClone(ctx context.Context, in *ListRefsRequest, opts ...grpc.CallOption) (*ListRefsResponse, error) {
	client := NewGitServiceClient(rpc.connection)
	return client.ListRefsForClone(ctx, in, opts...)
}

//UploadPack ...
func (rpc *RPCClient) UploadPack(ctx context.Context, in *UploadPackRequest, opts ...grpc.CallOption) (*UploadPackResponse, error) {
	client := NewGitServiceClient(rpc.connection)
	return client.UploadPack(ctx, in, opts...)
}

//UploadPackStream ...
func (rpc *RPCClient) UploadPackStream(ctx context.Context, in *UploadPackRequest, opts ...grpc.CallOption) (GitService_UploadPackStreamClient, error) {
	client := NewGitServiceClient(rpc.connection)
	return client.UploadPackStream(ctx, in, opts...)
}

//ReceivePack ...
func (rpc *RPCClient) ReceivePack(ctx context.Context, in *ReceivePackRequest, opts ...grpc.CallOption) (*GenericResponse, error) {
	client := NewGitServiceClient(rpc.connection)
	return client.ReceivePack(ctx, in, opts...)
}

//ReceivePackStream ...
func (rpc *RPCClient) ReceivePackStream(ctx context.Context, opts ...grpc.CallOption) (GitService_ReceivePackStreamClient, error) {
	client := NewGitServiceClient(rpc.connection)
	return client.ReceivePackStream(ctx, opts...)
}

//WriteReference ..
func (rpc *RPCClient) WriteReference(ctx context.Context, in *WriteReferenceRequest, opts ...grpc.CallOption) (*GenericResponse, error) {
	client := NewGitServiceClient(rpc.connection)
	return client.WriteReference(ctx, in, opts...)
}
