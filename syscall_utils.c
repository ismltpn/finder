#include "syscall_utils.h"
#include "constants.h"
#include "str_utils.h"
#include <errno.h>
#include <unistd.h>

int write_multiple(int fd, const char *strs[], int strc) {
    int i;
    int len;
    int total = 0;
    for (i = 0; i < strc; ++i) {
        len = str_len(strs[i]);
        safe_write(fd, strs[i], len);
        total += len;
    }
    return total;
}

void safe_write(int fd, const char *str, int len) {
    int byteswritten = 0;

    while ((byteswritten = write(fd, str, len) == -1) && (errno == EINTR));
    if (byteswritten < 0) {
        shutdown(SYSTEM_ERROR_EXIT);
    }
}

void shutdown(int err_code) {
    _exit(err_code);
}
