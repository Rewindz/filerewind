#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <pthread.h>

#include "lib/netutils.h"



CommandAction parse_client_cmd(const char *cmd)
{
  char *token;
  char *saveptr;
  char *buffer = malloc(strlen(cmd));
  
  if(!buffer){
    fprintf(stderr, "Failed to allocate memory!\n");
    return REW_CMD_ERR;
  }
  
  memset(buffer, 0, strlen(cmd));
  strcpy(buffer, cmd);
  do{
    printf("CMD: %s\n", cmd);
    token = strtok_r(buffer, "\n", &saveptr);
    //printf("Token: %s\nBuffer: %s\n", token, buffer);

    if(token == NULL)
      break;

    if(strcmp("QUIT", token) == 0){
      free(buffer);
      return REW_CMD_QUIT;
    }else
      break;

    
  }while(token);
  
  free(buffer);
  return REW_CMD_ERR;
}


void *serve_client(void *client_con_info) //struct ClientConnectionInfo -- must be freed
{
  ClientConnectionInfo *cci = (ClientConnectionInfo *)client_con_info;
  int close_req = 0;
  
  //TODO: TIMEOUT
  
  //Server Client Loop
  char cmd_buffer[512];
  do{
    ssize_t recieved = recv(cci->sock, cmd_buffer, sizeof(cmd_buffer), 0);

    if(recieved == -1){
      fprintf(stderr, "Error recieving message!\n%s\n", strerror(errno));
      continue;
    }

    if(recieved == 0){
      printf("Recieved empty buffer. Closing socket\n");
      break;
    }

    switch(parse_client_cmd((char *)&cmd_buffer)){
    case REW_CMD_QUIT:
      close_req = 1;
      printf("Quit command\n");
      break;
    default:
      printf("Unknown command!\nCMD: %s\n", cmd_buffer);
      break;
    }

  }while(!close_req);

  close(cci->sock);
  free(cci);
  return 0;
}


void spawn_thread(int sockfd, struct sockaddr_in addr_in, in_addr_t addr_in_len)
{
  ClientConnectionInfo *cci = calloc(1, sizeof(ClientConnectionInfo));
  cci->sock = sockfd;
  cci->addr_in = addr_in;
  cci->addr_in_len = addr_in_len;
  
  pthread_t thread_id = 0;
  pthread_attr_t thread_attr = {0};
  pthread_attr_init(&thread_attr);
  int res = pthread_create(&thread_id, &thread_attr, serve_client, (void *)cci);
  if(res != 0){
    fprintf(stderr, "Error creating thread!\n%s\n", strerror(res));
    free(cci);
    close(sockfd);
    return;
  }
}


int main(int argc, char **argv)
{

  if(argc < 2){
    fprintf(stderr, "Usage: %s [listen-port]\n", argv[0]);
    return 1;
  }
  
  int socket = rew_net_bind_tcp(argv[1]);

  if(socket == 0){
    fprintf(stderr, "Could not create a socket!\n");
    return 1;
  }

  listen(socket, 2);

  int open_sock = 0;

  struct sockaddr_in connected_client;
  in_addr_t connected_client_len = sizeof(connected_client);
  
  do{
    open_sock = accept(socket, (struct sockaddr*)&connected_client, &connected_client_len);
    if(open_sock == -1){
      fprintf(stderr, "Couldn't accept connection!\n%s\n", strerror(errno));
      break;
    }

    printf("Recieved connection\t");
    
    printf("Port: %i\tRemote Address: %s\n", connected_client.sin_port, inet_ntoa(connected_client.sin_addr));

    char buffer[256];
    ssize_t recieved = recv(open_sock, buffer, sizeof(buffer), 0);

    if(recieved == -1){
      fprintf(stderr, "Error recieving message!\n%s\n", strerror(errno));
      close(open_sock);
    }else if(strcmp(buffer, "CON\nFR")){
      printf("Received Connection Command: %s\n", buffer);
      const char *ok_msg = "OK\n\0";
      int ok_msg_len = strlen(ok_msg) + 1;
      if(write(open_sock, ok_msg, ok_msg_len) != ok_msg_len){
	fprintf(stderr, "Failed to respond to client!\n%s\n", strerror(errno));
	close(open_sock);
      }
      spawn_thread(open_sock, connected_client, connected_client_len);
    }
    
  }while(1);

  close(socket);
  
  
  return errno;
}
