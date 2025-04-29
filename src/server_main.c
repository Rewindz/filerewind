#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <pthread.h>

#include "lib/netutils.h"
#include "lib/svrcmds.h"
#include "lib/types.h"

volatile sig_atomic_t flag_stop = 0;

void handle_sigint(int sig)
{
  (void)sig;
  flag_stop = 1;
}


CommandAction parse_client_cmd(const char *cmd, char *arg)
{
  char *token;
  char *saveptr;
  char *buffer = calloc(strlen(cmd) + 2, sizeof(char));

  CommandAction ret = REW_CMD_ERR;
  
  if(!buffer){
    fprintf(stderr, "Failed to allocate memory!\n");
    return REW_CMD_ERR;
  }
  
  strcpy(buffer, cmd);
  printf("CMD: %s\n", cmd);
  token = strtok_r(buffer, ";", &saveptr);
  do{
    printf("Token: %s\nBuffer: %s\n", token, buffer);

    if(token == NULL)
      break;

    if(strcmp("QUIT", token) == 0){
      ret = REW_CMD_QUIT;
    }
    else if(strcmp("LS", token) == 0){
      ret = REW_CMD_LS;
    }
    else if(strcmp("CD", token) == 0){
      ret = REW_CMD_CD;
    }
    else{
      strcpy(arg, token);
    }

    token = strtok_r(NULL, ";", &saveptr);
  }while(token);
  
  free(buffer);
  return ret;
}


void *serve_client(void *client_con_info) //struct ClientConnectionInfo -- must be freed
{
  ClientConnectionInfo *cci = (ClientConnectionInfo *)client_con_info;
  int close_req = 0;
  
  //TODO: TIMEOUT
  
  //Server Client Loop
  int cmd_buffer_len = 512;
  char cmd_buffer[cmd_buffer_len];

  char *arg = calloc(512, sizeof(char));
  if(!arg){
    fprintf(stderr, "Can't malloc a string????\n");
    abort();
  }
  
  do{
    memset(cmd_buffer, 0, cmd_buffer_len);
    ssize_t recieved = recv(cci->sock, cmd_buffer, cmd_buffer_len, 0);
    printf("Recieved size: %zi\tBuffer: %s\n", recieved, cmd_buffer);

    if(recieved == -1){
      fprintf(stderr, "Error recieving message!\n%s\n", strerror(errno));
      continue;
    }

    if(recieved == 0){
      printf("Recieved empty buffer. Closing socket\n");
      break;
    }

    memset(arg, 0, 512);
    
    switch(parse_client_cmd((char *)&cmd_buffer, arg)){
    case REW_CMD_QUIT:
      close_req = 1;
      printf("Quit command\n");
      break;
    case REW_CMD_LS:
      server_reply(cci->sock, process_ls_cmd(cci->dir));
      break;
    case REW_CMD_CD:
      process_cd_cmd(&cci->dir, arg);
      break;
    default:
      printf("Unknown command!\nCMD: %s\n", cmd_buffer);
      break;
    }

  }while(!close_req);
  free(arg);

  printf("Closing client!\n");
  close(cci->sock);
  closedir(cci->dir);
  free(cci);
  return 0;
}


pthread_t spawn_thread(int sockfd, struct sockaddr_in addr_in, in_addr_t addr_in_len)
{
  ClientConnectionInfo *cci = calloc(1, sizeof(ClientConnectionInfo));
  cci->sock = sockfd;
  cci->addr_in = addr_in;
  cci->addr_in_len = addr_in_len;
  cci->dir = get_cwd();

  if(!cci->dir){
    fprintf(stderr, "Couldn't open cwd!\n%s\n", strerror(errno));
    free(cci);
    close(sockfd);
    return -1;
  }

  
  pthread_t thread_id = 0;
  pthread_attr_t thread_attr = {0};
  pthread_attr_init(&thread_attr);
  int res = pthread_create(&thread_id, &thread_attr, serve_client, (void *)cci);
  if(res != 0){
    fprintf(stderr, "Error creating thread!\n%s\n", strerror(res));
    free(cci);
    close(sockfd);
    return -1;
  }
  return thread_id;
}


int main(int argc, char **argv)
{

  if(argc < 2){
    fprintf(stderr, "Usage: %s [listen-port]\n", argv[0]);
    return 1;
  }

  struct sigaction sa;
  sa.sa_handler = handle_sigint;
  sa.sa_flags = 0;
  sigemptyset(&sa.sa_mask);
  sigaction(SIGINT, &sa, NULL);
  
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

    if(flag_stop) break;
    
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
      if(server_ok(open_sock))
	close(open_sock);
      else
	spawn_thread(open_sock, connected_client, connected_client_len);
    }
    
  }while(!flag_stop);

  close(open_sock);
  close(socket);
  
  
  return errno;
}
