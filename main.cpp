#include <sys/socket.h>
#include <iostream>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/epoll.h>

const int PORT = 8000;
const int MAX_EVENTS = 10;

int main() {
  // create socket
  int server_fd = socket(PF_INET, SOCK_STREAM, 0);
  if (server_fd == -1) {
    std::cerr << "Failed to create the socket." << std::endl;
    return 1;
  }

  // set address and port
  sockaddr_in server_addr;
  bzero(&server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(PORT);

  // bind socket
  if (bind(server_fd, (sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
    std::cerr << "Failed to bind the socket." << std::endl;
    close(server_fd);
    return 1;
  }

  // listen socket
  if (listen(server_fd, SOMAXCONN) == -1) {
    std::cerr << "Failed to listen the socket" << std::endl;
    close(server_fd);
    return 1;
  }


  // create epoll instance
  int epoll_fd = epoll_create1(0);
  
  return 0;
}
