#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

#include "lib/netutils.h"



int send_command(int sock, const char* cmd, const char *error_format)
{
  int cmd_len = strlen(cmd) + 1;
  printf("CMD: %s\tLength: %i\n", cmd, cmd_len);
  if(write(sock, cmd, cmd_len) < 0){
    fprintf(stderr, error_format, strerror(errno));
    return -1;
  } 
  return 0;
}




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
  memset(buffer, 0, sizeof(buffer));
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


  char *input = calloc(1024, sizeof(char));
  if(!input){
    fprintf(stderr, "Out of memory wtf?\n%s\n", strerror(errno));
    close(server_sock);
    return errno;
  }


  memset(buffer, 0, sizeof(buffer));
  while(strcmp("QUIT", input) != 0){
    scanf("%s", input);
    printf("Scan length: %zi\n", strlen(input));
    if(send_command(server_sock, input, "Failed to send command!\n%s\n") < 0) break;
    //Wait for reply
    // -- Server needs to send the length with the OK msg, so we can allocate a buffer sizable enough for the reply.

    int reply_len = 0;
    ssize_t recieved = recv(server_sock, buffer, sizeof(buffer), 0);
    if(recieved == -1){
      
    }

    
  }
  free(input);

  close(server_sock);

  

  return errno;
}
