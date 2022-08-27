#include <memory>

#include "Connection.h"
#include "Server.h"
#include <iostream>
using namespace boost::asio;

namespace Server_connection {
Server::Server(IPV address, int port)
    : _ipVersion(address),
      _port(port),
      _acceptor(_context, ip::tcp::endpoint(_ipVersion == IPV::V4 ? ip::tcp::v4() : ip::tcp::v6(), _port)) {}
int Server::Run() {
  try {
    StartAccept();
    _context.run();
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
    return -1;
  }
  return 0;
}
void Server::Broadcast(const std::string &message) {
  std::cout<< "broadcast";
  for (auto &connection : _connections) {
    connection->Post(message);
  }
}
void Server::StartAccept() {
  _socket.emplace(_context);
  _acceptor.async_accept(*_socket, [this](const boost::system::error_code &error) {
    std::shared_ptr<TCPConnection> new_connection = std::make_shared<TCPConnection>(std::move(*_socket));
    std::cout << "User has joined the server: " << new_connection->GetUsername() << std::endl;
    _connections.insert(new_connection);
    if (!error) {
      new_connection->Start([this](const std::string &message) { OnClientMessage(message); },
                            [&, weak = std::weak_ptr(new_connection)] {
                              if (auto shared = weak.lock(); shared && _connections.erase(shared)) {
                                std::cout << "User has left the server: " << new_connection->GetUsername() << std::endl;
                              }
                            });
    } else {
//      _socket.close(error);
//      _errorHandler();
    }
    StartAccept();
  });
}
}

int main() {
  Server_connection::Server server(Server_connection::IPV::V4, 80);
  server.OnJoin = [](std::shared_ptr<Server_connection::TCPConnection> server) {
    std::cout << "User has joined the server: " << server->GetUsername() << std::endl;
  };
  
  server.OnLeave = [](std::shared_ptr<Server_connection::TCPConnection> server) {
    std::cout << "User has left the server: " << server->GetUsername() << std::endl;
  };
  
  server.OnClientMessage = [&server](const std::string& message) {
    // Parse the message
    // Do game server things
    
    // Send message to client
    server.Broadcast(message);
  };
  server.Run();
}