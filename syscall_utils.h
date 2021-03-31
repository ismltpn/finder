#ifndef SYSCALL_UTILS_H
#define SYSCALL_UTILS_H

int write_multiple(int fd, const char *strs[], int strc);

void safe_write(int fd, const char *str, int len);

void shutdown(int err_code);

#endif