//
// Created by Ruslan Galiullin on 27.08.2022.
//

#ifndef ECHO_SERVER_SERVER_H_
#define ECHO_SERVER_SERVER_H_
#include <boost/asio.hpp>
#include <functional>
#include <optional>
#include <unordered_set>
#include <memory>
#include "Connection.h"
namespace Server_connection {
using OnJoinHandler = std::function<void(std::shared_ptr<TCPConnection>)>;
using OnLeaveHandler = std::function<void(std::shared_ptr<TCPConnection>)>;
using OnClientMessageHandler = std::function<void(std::string)>;
enum class IPV {
  V4,
  V6
};
class Server {
 private:
  IPV _ipVersion;
  int _port;
  std::unordered_set<std::shared_ptr<TCPConnection>> _connections;
  boost::asio::io_context _context;
  boost::asio::ip::tcp::acceptor _acceptor;
  std::optional<boost::asio::ip::tcp::socket> _socket;
 public:
  Server(IPV address, int port);
  int Run();
  void Broadcast(const std::string &message);
  void StartAccept();
  OnJoinHandler OnJoin;
  OnLeaveHandler OnLeave;
  OnClientMessageHandler OnClientMessage;
};
}
#endif //ECHO_SERVER_SERVER_H_
