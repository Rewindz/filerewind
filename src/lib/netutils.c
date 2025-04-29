#include "netutils.h"

int rew_net_bind_tcp(const char *port) // -> sockfd
{
  struct addrinfo hints = {0};
  struct addrinfo *result;
  struct addrinfo *rp;

  int sfd;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  hints.ai_protocol = 0;
  hints.ai_canonname = NULL;
  hints.ai_addr = NULL;
  hints.ai_next = NULL;

  int s = getaddrinfo(NULL, port, &hints, &result);
  if(s != 0){
    fprintf(stderr, "Could not prepare port: %s!\n%s\n", port, gai_strerror(s));
    abort(); //TODO: handle this better
  }
  
  for(rp = result; rp != NULL; rp = rp->ai_next){
    sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    if(sfd == -1)
      continue;

    if(bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
      break; // Success

    close(sfd);

  }

  freeaddrinfo(result);

  if(rp == NULL) {
    fprintf(stderr, "Could not bind on port: %s\n", port);
    abort(); //TODO: handle this better
  }

  return sfd;

}

int rew_net_connect_tcp(const char *rem_addr, const char *port) // -> sockfd
{
  struct addrinfo hints = {0};
  struct addrinfo *result;
  struct addrinfo *rp;

  int sfd;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = 0;
  hints.ai_protocol = 0;

  int s = getaddrinfo(rem_addr, port, &hints, &result);
  if(s != 0){
    fprintf(stderr, "Could not prepare port: %s!\n%s\n", port, gai_strerror(s));
    abort(); //TODO: handle this better
  }
  
  for(rp = result; rp != NULL; rp = rp->ai_next){
    sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    if(sfd == -1)
      continue;

    if(connect(sfd, rp->ai_addr, rp->ai_addrlen) != 1)
      break; // Success

    close(sfd);

  }

  freeaddrinfo(result);

  if(rp == NULL) {
    fprintf(stderr, "Could not bind on port: %s\n", port);
    abort(); //TODO: handle this better
  }

  return sfd;

}

int server_ok(int sockfd)
{
   const char *ok_msg = "OK\n\0";
   int ok_msg_len = strlen(ok_msg) + 1;
   if(write(sockfd, ok_msg, ok_msg_len) != ok_msg_len){
     fprintf(stderr, "Failed to respond to client!\n%s\n", strerror(errno));
     return errno;
   }
   return 0;
}

int server_reply(int sockfd, StringArray reply)
{
  int msg_len = 0;
  for(int i = 0; i < reply.count; ++i)
    msg_len += 1 + strlen(reply.arr[i]);    
  msg_len++;
  
  char *msg = calloc(msg_len, sizeof(char));
  for(int i = 0; i < reply.count; ++i){
    strcat(msg, reply.arr[i]);
    strcat(msg, "\n");
  }
 
  if(write(sockfd, msg, msg_len) == -1){
    fprintf(stderr, "Error sending reply to client!\n%s\n", strerror(errno));
    free(msg);
    return errno;
  }

  if(server_ok(sockfd)){
    free(msg);
    return errno;
  }

  free(msg);
  return 0;
}
