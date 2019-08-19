#include <arpa/inet.h>
#include <errno.h>
#include <error.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

int main() {
  printf("Starting btdb\n");

  struct addrinfo hints, *serv_info;
  memset(&hints, 0, sizeof(hints));

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  auto err = getaddrinfo("127.0.0.1", "3939", &hints, &serv_info);
  if (err != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(err));
    exit(EXIT_FAILURE);
  }

  if (serv_info == nullptr) {
    fprintf(stderr, "getaddrinfo returned an empty linked list");
    exit(EXIT_FAILURE);
  }

  // TODO: Loop through results. Look at example in man 3 getaddrinfo.
  auto fd = socket(serv_info->ai_family, serv_info->ai_socktype, serv_info->ai_protocol);
  if (fd < 0) {
    perror("Failed to create socket");
    exit(EXIT_FAILURE);
  }
  auto opt = 1;
  if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
    perror("Failed to set socket options");
    exit(EXIT_FAILURE);
  }
  if (bind(fd, serv_info->ai_addr, serv_info->ai_addrlen) < 0) {
    perror("Failed to bind socket to address");
    exit(EXIT_FAILURE);
  }
  printf("Shutting down btdb\n");
  return 0;
}