#include <sys/socket.h>
#include <iostream>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <string.h>
#include <arpa/inet.h>

const int PORT = 8000;
const int MAX_EVENTS = 10;

int main() {
  // create socket
  int server_fd = socket(PF_INET, SOCK_STREAM, 0);
  if (server_fd == -1) {
    std::cerr << "Failed to create a socket." << std::endl;
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
  if (epoll_fd == -1) {
    std::cerr << "Failed to create an epoll instance." << std::endl;
    close(server_fd);
    return 1;
  }

  // add the server socket to the epoll instance
  epoll_event ev;
  ev.events = EPOLLIN;
  ev.data.fd = server_fd;
  if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev) == -1) {
    std::cerr << "Failed to add the server socket to the epoll instance." << std::endl;
    close(server_fd);
    close(epoll_fd);
    return 1;
  }

  // event loop
  epoll_event events[MAX_EVENTS];
  while (true) {
    int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
    if (nfds == -1) {
      std::cerr << "Failed on epoll_wait." << std::endl;
      break ;
    }

    for (int i = 0; i < nfds; ++i) {
      if (events[i].data.fd == server_fd) {
	sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	int client_fd = accept(server_fd, (sockaddr *)&client_addr, &client_addr_len);

	if (client_fd == -1) {
	  std::cerr << "Failed to accept the client connection." << std::endl;
	  continue ;
	}

	ev.events = EPOLLIN;
	ev.data.fd = client_fd;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) == -1) {
          std::cerr << "Failed to add client socket to epoll instance."
                    << std::endl;
          close(client_fd);
	  continue ;
        }

        std::cout << "Client connected: " << inet_ntoa(client_addr.sin_addr)
                  << ":" << ntohs(client_addr.sin_port) << std::endl;
      } else {
        int client_fd = events[i].data.fd;
        char buffer[1024];

        ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
	  std::cout << "Client disconnected or error occurred." << std::endl;
	  epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
          close(client_fd);
        } else {
	  send(client_fd, buffer, bytes_received, 0);
        }
      }
    }
  }

  close(epoll_fd);
  close(server_fd);  
  
  return 0;
}
