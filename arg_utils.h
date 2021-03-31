#ifndef _ARG_UTILS_H
#define _ARG_UTILS_H
#include <unistd.h>
#include "constants.h"

void get_filename_arg(char *str, char *dest);
size_t get_filesize_arg(char *str, Bool *err);
FileType get_filetype_arg(char *str, Bool *err);
PermissionFlagHolder get_permissions_arg(char *str, Bool *err);
size_t get_numoflinks_arg(char *str, Bool *err);
void get_path_arg(char *str, char *dest);

#endif
