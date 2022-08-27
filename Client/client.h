//
// Created by Ruslan Galiullin on 27.08.2022.
//

#ifndef ECHO_CLIENT_CLIENT_H_
#define ECHO_CLIENT_CLIENT_H_
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <iostream>
#include <queue>
#include <string>
class Client {
  using MessageHandler = std::function<void(std::string)>;
 public:
  Client(const std::string& address, int port);
  void Run();
  void Post(const std::string& message);
  void Stop();
  MessageHandler OnMessage;
 private:
  void DoRead();
  void OnRead(boost::system::error_code error, size_t bytes);
  void DoWrite();
  void OnWrite(boost::system::error_code error, size_t bytes);
 private:
  boost::asio::io_context _context{};
  boost::asio::ip::tcp::socket _socket;
  boost::asio::ip::tcp::resolver::results_type _endpoints;
  boost::asio::streambuf _streamBuf{65536};
  std::queue<std::string> _outgoingMessages {};
};
#endif //ECHO_CLIENT_CLIENT_H_
