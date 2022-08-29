#include <iostream>
#include "../Network/include/server/server.h"
int main() {
  CSA::Server server(CSA::IPV::v4, 80);
  server.OnJoin = [](std::shared_ptr<CSA::TCPConnection> server) {
    std::cout << "User has joined the server: " << server->GetUsername() << std::endl;
  };
  
  server.OnLeave = [](std::shared_ptr<CSA::TCPConnection> server) {
    std::cout << "User has left the server: " << server->GetUsername() << std::endl;
  };
  
  server.OnClientMessage = [&server](const std::string &message) {

    // Send message to client
    server.Broadcast(message);
  };
  server.Run();
}