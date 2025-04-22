#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

#include "lib/netutils.h"




int main(int argc, char **argv)
{

  if(argc < 3){
    fprintf(stderr, "Usage: %s [server_address] [server_port]\n", argv[0]);
    return 1;
  }
  
  int server_sock = rew_net_connect_tcp(argv[1], argv[2]);
  if(server_sock == -1){
    fprintf(stderr, "There was a problem creating connection!\n%s\n", strerror(errno));
  }

  const char *hello_world = "Hello world! From Client :)";
  int hello_world_len = strlen(hello_world);
  if(write(server_sock, hello_world, hello_world_len) != hello_world_len){
    fprintf(stderr, "Failed to properly write to server! Wrote %i bytes.\n", hello_world_len);
  }

  close(server_sock);

  

  return errno;
}
