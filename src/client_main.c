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

  const char *connect_cmd = "CON\nFR\n\0";
  int connect_cmd_len = strlen(connect_cmd)+1;
  if(write(server_sock, connect_cmd, connect_cmd_len) != connect_cmd_len){
    fprintf(stderr, "Failed to send connection message to server.\n");
  }

  char buffer[256];
  ssize_t recieved = recv(server_sock, buffer, sizeof(buffer), 0);
  if(recieved == -1){
    fprintf(stderr, "Error recieving message from server!\n%s\n", strerror(errno));
    close(server_sock);
    return errno;
  }

  printf("Got | %s | back from server\n", buffer);
  if(strcmp(buffer, "OK") == 0){
    printf("Got OK!\n");
  }
  

  const char *quit_cmd = "QUIT\n\0";
  int quit_cmd_len = strlen(quit_cmd) + 1;
  if(write(server_sock, quit_cmd, quit_cmd_len) != quit_cmd_len){
    fprintf(stderr, "Failed to properly write to server! Wrote %i bytes.\n", quit_cmd_len);
  }

  close(server_sock);

  

  return errno;
}
