#ifndef STR_UTILS_H
#define STR_UTILS_H

#include "constants.h"

int str_len(const char *str);

void str_to_lower(const char *str, char *dest);

void str_cat(const char *str1, const char *str2, char *dest);

Bool str_equal(const char *str1, const char *str2);

Bool match(const char *exp, const char *str);

unsigned long int str_to_unsigned_int(char *str, Bool *err);

void str_copy(const char *src, char *dest);

#endif