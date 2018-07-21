// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: GitService.proto

#include "GitService.pb.h"
#include "GitService.grpc.pb.h"

#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/channel_interface.h>
#include <grpcpp/impl/codegen/client_unary_call.h>
#include <grpcpp/impl/codegen/method_handler_impl.h>
#include <grpcpp/impl/codegen/rpc_service_method.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/impl/codegen/sync_stream.h>
namespace nexus {

static const char* GitService_method_names[] = {
  "/nexus.GitService/ListKnownRefs",
  "/nexus.GitService/ListRefsForClone",
  "/nexus.GitService/UploadPack",
  "/nexus.GitService/ReceivePack",
};

std::unique_ptr< GitService::Stub> GitService::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< GitService::Stub> stub(new GitService::Stub(channel));
  return stub;
}

GitService::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel)
  : channel_(channel), rpcmethod_ListKnownRefs_(GitService_method_names[0], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_ListRefsForClone_(GitService_method_names[1], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_UploadPack_(GitService_method_names[2], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_ReceivePack_(GitService_method_names[3], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status GitService::Stub::ListKnownRefs(::grpc::ClientContext* context, const ::nexus::ListRefsRequest& request, ::nexus::ListRefsResponse* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_ListKnownRefs_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::nexus::ListRefsResponse>* GitService::Stub::AsyncListKnownRefsRaw(::grpc::ClientContext* context, const ::nexus::ListRefsRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::nexus::ListRefsResponse>::Create(channel_.get(), cq, rpcmethod_ListKnownRefs_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::nexus::ListRefsResponse>* GitService::Stub::PrepareAsyncListKnownRefsRaw(::grpc::ClientContext* context, const ::nexus::ListRefsRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::nexus::ListRefsResponse>::Create(channel_.get(), cq, rpcmethod_ListKnownRefs_, context, request, false);
}

::grpc::Status GitService::Stub::ListRefsForClone(::grpc::ClientContext* context, const ::nexus::ListRefsRequest& request, ::nexus::ListRefsResponse* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_ListRefsForClone_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::nexus::ListRefsResponse>* GitService::Stub::AsyncListRefsForCloneRaw(::grpc::ClientContext* context, const ::nexus::ListRefsRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::nexus::ListRefsResponse>::Create(channel_.get(), cq, rpcmethod_ListRefsForClone_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::nexus::ListRefsResponse>* GitService::Stub::PrepareAsyncListRefsForCloneRaw(::grpc::ClientContext* context, const ::nexus::ListRefsRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::nexus::ListRefsResponse>::Create(channel_.get(), cq, rpcmethod_ListRefsForClone_, context, request, false);
}

::grpc::Status GitService::Stub::UploadPack(::grpc::ClientContext* context, const ::nexus::UploadPackRequest& request, ::nexus::UploadPackResponse* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_UploadPack_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::nexus::UploadPackResponse>* GitService::Stub::AsyncUploadPackRaw(::grpc::ClientContext* context, const ::nexus::UploadPackRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::nexus::UploadPackResponse>::Create(channel_.get(), cq, rpcmethod_UploadPack_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::nexus::UploadPackResponse>* GitService::Stub::PrepareAsyncUploadPackRaw(::grpc::ClientContext* context, const ::nexus::UploadPackRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::nexus::UploadPackResponse>::Create(channel_.get(), cq, rpcmethod_UploadPack_, context, request, false);
}

::grpc::Status GitService::Stub::ReceivePack(::grpc::ClientContext* context, const ::nexus::ReceivePackRequest& request, ::google::protobuf::Empty* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_ReceivePack_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::google::protobuf::Empty>* GitService::Stub::AsyncReceivePackRaw(::grpc::ClientContext* context, const ::nexus::ReceivePackRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::google::protobuf::Empty>::Create(channel_.get(), cq, rpcmethod_ReceivePack_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::google::protobuf::Empty>* GitService::Stub::PrepareAsyncReceivePackRaw(::grpc::ClientContext* context, const ::nexus::ReceivePackRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::google::protobuf::Empty>::Create(channel_.get(), cq, rpcmethod_ReceivePack_, context, request, false);
}

GitService::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      GitService_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< GitService::Service, ::nexus::ListRefsRequest, ::nexus::ListRefsResponse>(
          std::mem_fn(&GitService::Service::ListKnownRefs), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      GitService_method_names[1],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< GitService::Service, ::nexus::ListRefsRequest, ::nexus::ListRefsResponse>(
          std::mem_fn(&GitService::Service::ListRefsForClone), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      GitService_method_names[2],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< GitService::Service, ::nexus::UploadPackRequest, ::nexus::UploadPackResponse>(
          std::mem_fn(&GitService::Service::UploadPack), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      GitService_method_names[3],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< GitService::Service, ::nexus::ReceivePackRequest, ::google::protobuf::Empty>(
          std::mem_fn(&GitService::Service::ReceivePack), this)));
}

GitService::Service::~Service() {
}

::grpc::Status GitService::Service::ListKnownRefs(::grpc::ServerContext* context, const ::nexus::ListRefsRequest* request, ::nexus::ListRefsResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status GitService::Service::ListRefsForClone(::grpc::ServerContext* context, const ::nexus::ListRefsRequest* request, ::nexus::ListRefsResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status GitService::Service::UploadPack(::grpc::ServerContext* context, const ::nexus::UploadPackRequest* request, ::nexus::UploadPackResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status GitService::Service::ReceivePack(::grpc::ServerContext* context, const ::nexus::ReceivePackRequest* request, ::google::protobuf::Empty* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


}  // namespace nexus

