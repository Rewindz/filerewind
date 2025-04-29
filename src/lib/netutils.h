#ifndef REW_NET_UTILS_
#define REW_NET_UTILS_

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>


typedef struct{
  int sock;
  struct sockaddr_in addr_in;
  in_addr_t addr_in_len;
  DIR *dir;
}ClientConnectionInfo;

typedef enum{
  REW_CMD_ERR = -1,
  REW_OK = 0,
  REW_CMD_QUIT = 1,
  REW_CMD_LS = 2,
  REW_CMD_CD = 3
}CommandAction;


int rew_net_bind_tcp(const char *port);
int rew_net_connect_tcp(const char *rem_addr, const char *port);


#endif //REW_NET_UTILS_
