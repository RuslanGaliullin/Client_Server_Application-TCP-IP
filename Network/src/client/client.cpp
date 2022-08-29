#include "../../include/client/client.h"
using namespace boost::asio;
namespace CSA {
Client::Client(const std::string &address, int port) : _socket(_context) {
  ip::tcp::resolver resolver{_context};
  _endpoints = resolver.resolve(address, std::to_string(port));
}
void Client::Run() {
  async_connect(_socket, _endpoints, [this](boost::system::error_code error, ip::tcp::endpoint ep) {
    if (!error)
      DoRead();
  });
  _context.run();
}
void Client::DoRead() {
  async_read_until(_socket, _streamBuf, "\n", [this](boost::system::error_code error, size_t bytesTransferred) {
    OnRead(error, bytesTransferred);
  });
}
void Client::OnRead(boost::system::error_code error, size_t) {
  if (ec) {
    Stop();
    return;
  }
  std::stringstream message;
  message << std::istream(&_streamBuf).rdbuf();
  OnMessage(message.str());
  DoRead();
}

void Client::Stop() {
  boost::system::error_code error;
  _socket.close(error);
  
  if (error) {
    std::cerr << error << std::endl;
  }
}
void Client::DoWrite() {
  async_write(_socket,
              buffer(_outgoingMessages.front()),
              [this](boost::system::error_code error, size_t bytesTransferred) {
                OnWrite(error, bytesTransferred);
              });
}

void Client::OnWrite(boost::system::error_code ec, size_t) {
  if (error) {
    Stop();
    return;
  }
  _outgoingMessages.pop();
  if (!_outgoingMessages.empty()) {
    DoWrite();
  }
}
void Client::Post(const std::string &message) {
  bool queueIdle = _outgoingMessages.empty();
  _outgoingMessages.push(message);
  
  if (queueIdle) {
    DoWrite();
  }
}
}