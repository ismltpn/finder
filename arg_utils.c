#include "arg_utils.h"
#include "constants.h"
#include "str_utils.h"
#include <unistd.h>

int powerof2(int x);

void get_filename_arg(char *str, char *dest) {
    str_to_lower(str, dest);
}

size_t get_filesize_arg(char *str, Bool *err) {
    return str_to_unsigned_int(str, err);
}

FileType get_filetype_arg(char *str, Bool *err) {
    if (str_len(str) != 1) {
        *err = TRUE;
        return F_UNKNOWN;
    }
    switch (str[0]) {
        case 'd':
            return F_DIRECTORY;
        case 's':
            return F_SOCKET;
        case 'b':
            return F_B_DEVICE;
        case 'c':
            return F_C_DEVICE;
        case 'f':
            return F_FILE;
        case 'p':
            return F_PIPE;
        case 'l':
            return F_LINK;
        default:
            return F_UNKNOWN;
    }
}

size_t get_numoflinks_arg(char *str, Bool *err) {
    return str_to_unsigned_int(str, err);
}

void get_path_arg(char *str, char *dest) {
    str_copy(str, dest);
}

PermissionFlagHolder get_permissions_arg(char *str, Bool *err) {
    int i;
    PermissionFlagHolder flags = 0;
    if (str_len(str) != 9) {
        return FALSE;
    }
    for (i = 0; i < 9 && !(*err); ++i) {
        switch (i % 3) {
            case 0:
                if (!(str[i] == '-' || str[i] == 'r')) {
                    *err = TRUE;
                }
                break;
            case 1:
                if (!(str[i] == '-' || str[i] == 'w')) {
                    *err = TRUE;
                }
                break;
            case 2:
                if (!(str[i] == '-' || str[i] == 'x')) {
                    *err = TRUE;
                }
                break;
        }
        if (str[i] != '-') {
            flags += powerof2(8 - i);
        }
    }
    return flags;
}

int powerof2(int x) {
    int i;
    int result = 1;
    for (i = 0; i < x; ++i) {
        result *= 2;
    }
    return result;
}