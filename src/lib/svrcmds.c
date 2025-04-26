#include "svrcmds.h"






void process_ls_cmd(int argc, char **argv)
{
  char cwd[1024];
  if(!getcwd(cwd, sizeof(cwd))){
    fprintf(stderr, "Could not get the current working directory!\n%s\n", strerror(errno));
    return;
  }

  DIR *directory = opendir(cwd);
  if(!directory){
    fprintf(stderr, "Could not open directory %s!\n%s\n", cwd, strerror(errno));
    return;
  }

  errno = 0;
  struct dirent *dirInfo = readdir(directory);
  if(errno != 0 && !dirInfo){
    fprintf(stderr, "Could not get directory information on directory %s!\n%s\n", cwd, strerror(errno));
    return;
  }

  printf("File 1: %s\n", dirInfo->d_name);

  return;
}


