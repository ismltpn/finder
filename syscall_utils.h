#ifndef _SYSCALL_UTILS_H
#define _SYSCALL_UTILS_H

int write_multiple(int fd, const char *strs[], int strc);
void safe_write(int fd, const char *str, int len);
void shutdown(int err_code);

#endif