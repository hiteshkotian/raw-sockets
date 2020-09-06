#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <netdb.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int bind_socket(const struct sockaddr *server_addr, socklen_t sock_len)
{
  int fd = 0;
  struct protoent *proto_entry = NULL;

  proto_entry = getprotobyname("tcp");
  if (proto_entry == NULL) {
    fprintf(stdout, "Unable to find protocol info\n");
    return -1;
  }

  fd = socket(AF_INET, SOCK_RAW, proto_entry->p_proto);
  if (fd < 0) {
    perror("Unable to create raw socket\n");
    return fd;
  }

  if (bind(fd, server_addr, sock_len) < 0) {
    perror("Unable to bind the socket\n");
    close(fd);
    return -1;
  }
  return fd;
}

int main()
{
  int fd;
  struct sockaddr_in server_addr;

  memset(&server_addr, 0, sizeof(struct sockaddr_in));

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(8080);

  if ((fd = bind_socket(
        (const struct sockaddr *)&server_addr, 
        sizeof(struct sockaddr_in))
      ) < 0) {
    return 1;
  }

  fprintf(stdout, "FD is %d\n", fd);

  int len, n;
  struct sockaddr_in client_addr;
  int buffer_len = 4096;
  uint8_t buffer[buffer_len];
  len = sizeof(client_addr);

  n = recvfrom(fd, (char *)buffer, buffer_len, 0, (struct sockaddr *)&client_addr, (socklen_t *)&len);
  char *client_addr_str = inet_ntoa(client_addr.sin_addr);
  int client_port = ntohs(client_addr.sin_port);
  fprintf(stdout, "received request from %s;%d\n", client_addr_str, client_port);
  fprintf(stdout, "Received %d bytes\n", n);
  if (n < 0) {
    perror("Read error\n");
    close(fd);
    return 1;
  }

  int i = 0;
  for (i = 0; i < n; i++) {
    fprintf(stdout, "0x%02x ", buffer[i]);
  }
  fprintf(stdout, "\n");


  fprintf(stdout, "server created\n");
  close(fd);
}
