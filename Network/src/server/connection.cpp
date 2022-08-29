#include "../../include/server/connection.h"
#include <iostream>
using namespace  boost::asio;
namespace CSA {
void TCPConnection::Post(const std::string &message) {
  bool queueIdle = _outgoingMessages.empty();
  _outgoingMessages.push(message);
  
  if (queueIdle) {
    DoWrite();
  }
}

TCPConnection::TCPConnection(ip::tcp::socket &&socket) : _socket(std::move(socket)) {
  std::stringstream name;
  name << _socket.remote_endpoint();
  
  _username = name.str();
}
void TCPConnection::Start(MessageHandler &&messageHandler, ErrorHandler &&errorHandler) {
  _messageHandler = std::move(messageHandler);
  errorHandler = std::move(errorHandler);
  
  DoRead();
}
/*void TCPConnection::Stop(const boost::system::error_code &error) {
  _socket.close(error);
  _errorHandler();
  return;
}*/

void TCPConnection::DoRead() {
  async_read_until(_socket, _streamBuf, "\n", [self = shared_from_this()]
      (boost::system::error_code error, size_t bytesTransferred) {
    self->OnRead(error, bytesTransferred);
  });
}
void TCPConnection::OnRead(boost::system::error_code &error, size_t bytes) {
  if (error) {
    _socket.close(error);
    _errorHandler();
    return;
  }
  
  std::stringstream message;
  message << _username << ": " << std::istream(&_streamBuf).rdbuf();
  _streamBuf.consume(bytes);
  std::cout << message.str();
  _messageHandler(message.str());
  DoRead();
}
void TCPConnection::DoWrite() {
  boost::asio::async_write(_socket, boost::asio::buffer(_outgoingMessages.front()), [self = shared_from_this()]
      (boost::system::error_code error, size_t bytesTransferred) {
    self->OnWrite(error, bytesTransferred);
  });
}
void TCPConnection::OnWrite(boost::system::error_code &error, size_t) {
  if (error) {
    _socket.close(error);
    
    _errorHandler();
    return;
  }
  _outgoingMessages.pop();
  if (!_outgoingMessages.empty())
    DoWrite();
}
}