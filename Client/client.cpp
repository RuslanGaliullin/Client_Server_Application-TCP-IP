#include "../Network/include/client/client.h"
#include <thread>

int main() {
  CSA::Client client{"localhost", 80};
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
