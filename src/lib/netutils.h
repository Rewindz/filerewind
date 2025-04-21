#ifndef REW_NET_UTILS_
#define REW_NET_UTILS_

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int rew_net_bind_tcp(const char *port);



#endif //REW_NET_UTILS_
