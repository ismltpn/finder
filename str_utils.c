#include "str_utils.h"
#include "constants.h"
#include <unistd.h>

int to_digit(char c);

char to_lower(char c);

Bool is_upper(char c);

void str_cat(const char *str1, const char *str2, char *dest) {
    int i, j;
    for (i = 0; str1[i] != '\0'; ++i) {
        dest[i] = str1[i];
    }
    for (j = 0; str2[j] != '\0'; ++j) {
        dest[j + i] = str2[j];
    }
    dest[j + i] = '\0';
}

void str_copy(const char *src, char *dest) {
    int i;
    for (i = 0; src[i] != '\0'; ++i) {
        dest[i] = src[i];
    }
    dest[i] = 0;
}

int str_len(const char *str) {
    int i;
    if (str == NULL) {
        return -1;
    }
    for (i = 0; str[i] != '\0'; ++i);
    return i;
}

void str_to_lower(const char *str, char *dest) {
    int i;
    for (i = 0; str[i] != '\0'; ++i) {
        if (is_upper(str[i])) {
            dest[i] = to_lower(str[i]);
        } else {
            dest[i] = str[i];
        }
    }
    dest[i] = '\0';
}

unsigned long int str_to_unsigned_int(char *str, Bool *err) {
    int i, d;
    unsigned long res = 0;
    for (i = 0; str[i] != '\0'; ++i) {
        d = to_digit(str[i]);
        if (d < 0) {
            *err = TRUE;
            return -1;
        }
        res *= 10;
        res += d;
    }
    return res;
}

int to_digit(char c) {
    if (c >= 48 && c <= 57) {
        return c - 48;
    }
    return -1;
}

char to_lower(char c) {
    return (char)(c + 32);
}

Bool is_upper(char c) {
    return c >= 65 && c <= 90;
}

Bool str_equal(const char *str1, const char *str2) {
    int i;
    for (i = 0; str1[i] != '\0' && str2[i] != '\0'; ++i) {
        if (str1[i] != str2[i]) {
            return FALSE;
        }
    }
    if (str1[i] != '\0' || str2[i] != '\0') {
        return FALSE;
    }
    return TRUE;
}

Bool match(const char *exp, const char *str) {
    int i, j;
    char prev = 0;
    for (i = 0, j=0; exp[i] != '\0' && str[j] != '\0'; ++i, ++j) {
        if(exp[i] == '+'){
            while (str[j] == prev){
                ++j;
            }
            --j;
        }
        else if (exp[i] != str[j]) {
            return FALSE;
        }
        prev = exp[i];
    }
    if (exp[i] != '\0' || str[j] != '\0') {
        return FALSE;
    }
    return TRUE;
}