#include "client.h"
#include <thread>
using namespace boost::asio;

int main() {
  Client client{"localhost", 80};
  client.OnMessage = [](const std::string &message) {
    std::cout << message;
  };
  std::thread t{[&client]() { client.Run(); }};
  
  while (true) {
    std::string message;
    getline(std::cin, message);
    
    if (message == "\\q") break;
    message += "\n";
    
    client.Post(message);
  }
  client.Stop();
  t.join();
  return 0;
}
Client::Client(const std::string &address, int port): _socket(_context) {
  ip::tcp::resolver resolver{_context};
  _endpoints = resolver.resolve(address, std::to_string(port));
}
void Client::Run() {
  async_connect(_socket, _endpoints, [this](boost::system::error_code ec, ip::tcp::endpoint ep) {
    if (!ec)
      DoRead();
  });
  _context.run();
}
void Client::DoRead() {
  async_read_until(_socket, _streamBuf, "\n", [this](boost::system::error_code ec, size_t bytesTransferred) {
    OnRead(ec, bytesTransferred);
  });
}
void Client::OnRead(boost::system::error_code ec, size_t bytesTransferred) {
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
  boost::system::error_code ec;
  _socket.close(ec);
  
  if (ec) {
    std::cerr << ec << std::endl;
  }
}
void Client::DoWrite() {
  async_write(_socket,
              buffer(_outgoingMessages.front()),
              [this](boost::system::error_code ec, size_t bytesTransferred) {
                OnWrite(ec, bytesTransferred);
              });
}

void Client::OnWrite(boost::system::error_code ec, size_t bytesTransferred) {
  if (ec) {
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
