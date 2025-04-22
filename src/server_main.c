#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "lib/netutils.h"


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

  printf("Made a socket!!! %i\n", socket);


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

    printf("Recieved connection!\n");
    
    printf("Port: %i\nRemote Address: %s\n", connected_client.sin_port, inet_ntoa(connected_client.sin_addr));

    char buffer[256];
    ssize_t recieved = recv(open_sock, buffer, sizeof(buffer), 0);

    if(recieved == -1){
      fprintf(stderr, "Error recieving message!\n%s\n", strerror(errno));
    }

    printf("Recieved %zi bytes, message:\n%s\n\n", recieved, buffer);
    
    
    close(open_sock);
    
  }while(1);

  close(socket);
  
  
  return errno;
}
