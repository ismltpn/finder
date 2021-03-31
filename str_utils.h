#ifndef _STR_UTILS_H
#define _STR_UTILS_H
#include "constants.h"

int str_len(const char *str);
void str_to_lower(char *str, char *dest);
void str_cat(char *str1, char *str2, char *dest);
Bool str_equal(char *str1, char *str2);
unsigned long int str_to_unsigned_int(char *str, Bool *err);
void str_copy(char *src, char *dest);

#endif