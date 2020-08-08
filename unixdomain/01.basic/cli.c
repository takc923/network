#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>

char *socket_path = "./socket";
//char *socket_path = "\0hidden";

int main(int argc, char *argv[]) {
  struct sockaddr_un addr;
  char buf[100] = "1234567890\0";
  int rc = 10;
  int fd;

  if (argc > 1) socket_path = argv[1];

  if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    perror("socket error");
    exit(-1);
  }

  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  if (*socket_path == '\0') {
    *addr.sun_path = '\0';
    strncpy(addr.sun_path + 1, socket_path + 1, sizeof(addr.sun_path) - 2);
  } else {
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);
  }

  int optval;
  socklen_t optlen = sizeof(optval);
  int ret = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &optval, &optlen);
  printf("getsockopt ret: %d, optval: %d\n", ret, optval);
  ret = getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &optval, &optlen);
  printf("getsockopt ret: %d, optval: %d\n", ret, optval);

  if (connect(fd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
    perror("connect error");
    exit(-1);
  }

  for (int i = 1; i <= 10000; i++) {
    printf("%04d ", i);
    fflush(stdout);
    if (i % 10 == 0) {
      printf("\n");
    }
    if (write(fd, buf, rc) != rc) {
      if (rc > 0) fprintf(stderr, "partial write");
      else {
        perror("write error");
        exit(-1);
      }
    }
  }

  return 0;
}
