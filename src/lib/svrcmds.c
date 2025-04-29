#include "svrcmds.h"



int dirent_cmp(const void *dirA, const void *dirB)
{
  const struct dirent *dir1 = (struct dirent *)dirA;
  const struct dirent *dir2 = (struct dirent *)dirB;
  
  if(dir1->d_type == DT_DIR && dir2->d_type != DT_DIR)
    return -1;
  else if(dir2->d_type == DT_DIR && dir1->d_type != DT_DIR)
    return 1;

  return strcmp(dir1->d_name, dir2->d_name);
  
}

DIR *get_cwd()
{
  char cwd[1024];
  if(!getcwd(cwd, sizeof(cwd))){
    fprintf(stderr, "Could not get the current working directory!\n%s\n", strerror(errno));
    return NULL;
  }

  printf("CWD: %s\n", cwd);

  DIR *directory = opendir(cwd);
  if(!directory){
    fprintf(stderr, "Could not open directory %s!\n%s\n", cwd, strerror(errno));
    return NULL;
  }
  return directory;
}

void process_cd_cmd(DIR **dir, const char *newDir)
{
  if(chdir(newDir) == 1){
    fprintf(stderr, "Couldn't change directory to %s!\n", newDir);
    return;
  }

  closedir(*dir);

  *dir = get_cwd();
}


void process_ls_cmd(DIR *dir)
{
  int dirList_cap = 512;
  struct dirent *dirList = calloc(dirList_cap, sizeof(struct dirent)); // Need to change this; would be very bad

  struct dirent *dirInfo = {0}; // manpage says dont free
  int count = 0;
  do{
    errno = 0;
    dirInfo = readdir(dir);
    if(errno != 0 && !dirInfo){
      fprintf(stderr, "Could not get directory information!\n%s\n", strerror(errno));
      return;
    }
    if(dirInfo == NULL)
      break;

    if(count > dirList_cap) break; // Hack until I implement dynamic/growing array for dirList
    
    memcpy((dirList + count), dirInfo, sizeof(struct dirent));
    
    count++;
    
  } while(dirInfo != NULL);

  qsort(dirList, count, sizeof(struct dirent), dirent_cmp);
  
  for(int i = 0; i < count; ++i){
    printf("File %i: %s\t\t Type: %i\n", i, dirList[i].d_name, dirList[i].d_type);
  }

  free(dirList);
  printf("LS DONE\n");
  return;
}


