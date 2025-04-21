#include "netutils.h"

int rew_net_bind_tcp(const char *port)
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
