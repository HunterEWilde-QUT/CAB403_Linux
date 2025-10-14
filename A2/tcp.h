#include <netinet/in.h>
#include <sys/socket.h>

const socklen_t SOCKLEN = sizeof(struct sockaddr_in);
const socklen_t CLIENTLEN = sizeof(struct sockaddr);
const int PORT = 3000;
