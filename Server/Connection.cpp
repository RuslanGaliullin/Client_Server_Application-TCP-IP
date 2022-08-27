//
// Created by Ruslan Galiullin on 27.08.2022.
//
#include "Connection.h"
#include <strstream>
#include <iostream>
namespace Server_connection {
void TCPConnection::Post(const std::string &message) {
  bool queueIdle = _outgoingMessages.empty();
  _outgoingMessages.push(message);
  
  if (queueIdle) {
    DoWrite();
  }
}

TCPConnection::TCPConnection(boost::asio::ip::tcp::socket &&socket) : _socket(std::move(socket)) {
  boost::system::error_code ec;
  std::stringstream name;
  name << _socket.remote_endpoint();
  
  _username = name.str();
}
void TCPConnection::Start(MessageHandler &&message_handler, ErrorHandler &&error_handler) {
  _messageHandler = std::move(message_handler);
  error_handler = std::move(error_handler);
  
  DoRead();
}
/*void TCPConnection::Stop(const boost::system::error_code &error) {
  _socket.close(error);
  _errorHandler();
  return;
}*/

void TCPConnection::DoRead() {
  boost::asio::async_read_until(_socket, _streamBuf, "\n", [self = shared_from_this()]
      (boost::system::error_code ec, size_t bytesTransferred) {
    self->OnRead(ec, bytesTransferred);
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
      (boost::system::error_code ec, size_t bytesTransferred) {
    self->OnWrite(ec, bytesTransferred);
  });
}
void TCPConnection::OnWrite(boost::system::error_code &error, size_t bytes) {
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