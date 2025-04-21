#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#include "lib/netutils.h"


int main(int argc, char **argv)
{
  int socket = rew_net_bind_tcp("7777");

  if(socket == 0){
    fprintf(stderr, "Could not create a socket!\n");
    return 1;
  }

  printf("Made a socket!!! %i\n", socket);

  close(socket);
  
  
  return 0;
}
