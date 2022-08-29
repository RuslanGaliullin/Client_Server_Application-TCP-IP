#pragma once

#include <string>
#include <boost/asio.hpp>
#include <functional>
#include <optional>
#include <unordered_set>
#include <queue>
#include <functional>
namespace CSA {
using MessageHandler = std::function<void(std::string)>;
using ErrorHandler = std::function<void()>;
class TCPConnection : public std::enable_shared_from_this<TCPConnection> {
 public:
  explicit TCPConnection(boost::asio::ip::tcp::socket &&socket);
  void Post(const std::string &message);
  void DoRead();
  void OnRead(boost::system::error_code &error, size_t bytes);
  void DoWrite();
  void OnWrite(boost::system::error_code &error, size_t bytes);
  void Start(MessageHandler &&message_handler, ErrorHandler &&error_handler);
//  void Stop(const boost::system::error_code &error);
  std::string GetUsername() { return _username; };
 private:
  
  boost::asio::ip::tcp::socket _socket;
  std::string _username;
  
  std::queue<std::string> _outgoingMessages;
  boost::asio::streambuf _streamBuf{65536};
  
  MessageHandler _messageHandler;
  ErrorHandler _errorHandler;
};
}
