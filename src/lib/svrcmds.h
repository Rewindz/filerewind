#ifndef SERVER_COMMANDS_H_
#define SERVER_COMMANDS_H_

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

#include "types.h"

void process_cd_cmd(DIR **dir, const char *newDir);
StringArray process_ls_cmd(DIR *dir);
DIR *get_cwd();




#endif //SERVER_COMMANDS_H_
