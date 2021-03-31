#ifndef _FIND_H
#define _FIND_H

#include "constants.h"
#include <unistd.h>

void find(FlagHolder flags, char *filename, size_t filesize, FileType filetype, PermissionFlagHolder permissions, size_t numoflinks, char *path);

#endif