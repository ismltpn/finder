#include "find.h"
#include "constants.h"
#include "file_tree.h"
#include "str_utils.h"
#include "syscall_utils.h"
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>

int get_permission_flag_from_st_mode(__mode_t mod);
Bool is_link(struct stat *filestat);
Bool ends_with(const char *str, char c);

Bool
build_file_tree(FileTree *root, FileTree *parent, char filepath[4096], char name[256], FlagHolder flags, char *filename,
                size_t filesize, FileType filetype, PermissionFlagHolder permissions, size_t numoflinks, char *path);

Bool check_file(struct stat *filestat, char *name, FlagHolder flags, char *filename, size_t filesize, FileType filetype,
                PermissionFlagHolder permissions, size_t numoflinks, char *path);

Bool check_file(struct stat *filestat, char *name, FlagHolder flags, char *filename, size_t filesize, FileType filetype,
                PermissionFlagHolder permissions, size_t numoflinks, char *path) {
    /*printf("Checking file: %s\n", name);*/
    Bool result = TRUE;
    char lowername[256];
    if ((flags & FILENAME_FLAG) != 0) {
        str_to_lower(name, lowername);
        result = match(filename, lowername);
        if (!result) {
            return FALSE;
        }
    }
    if ((flags & FILESIZE_FLAG) != 0) {
        result = filesize == filestat->st_size;
        if (!result) {
            return FALSE;
        }
    }
    if ((flags & FILETYPE_FLAG) != 0) {
        switch (filetype) {
            case F_DIRECTORY:
                result = S_ISDIR(filestat->st_mode);
                break;
            case F_SOCKET:
                result = S_ISSOCK(filestat->st_mode);
                break;
            case F_B_DEVICE:
                result = S_ISBLK(filestat->st_mode);
                break;
            case F_C_DEVICE:
                result = S_ISCHR(filestat->st_mode);
                break;
            case F_FILE:
                result = S_ISREG(filestat->st_mode);
                break;
            case F_PIPE:
                result = S_ISFIFO(filestat->st_mode);
                break;
            case F_LINK:
                result = S_ISLNK(filestat->st_mode);
                break;
            default:
                result = FALSE;
                break;
        }
        if (!result) {
            return FALSE;
        }
    }
    if ((flags & PERMISSIONS_FLAG) != 0) {
        result = get_permission_flag_from_st_mode(filestat->st_mode) == permissions;
        if (!result) {
            return FALSE;
        }
    }
    if ((flags & NUMOFLINKS_FLAG) != 0) {
        result = filestat->st_nlink == numoflinks;
    }
    return result;
}

Bool
build_file_tree(FileTree *root, FileTree *parent, char filepath[4096], char name[256], FlagHolder flags, char *filename,
                size_t filesize, FileType filetype, PermissionFlagHolder permissions, size_t numoflinks, char *path) {
    DIR *dir = opendir(filepath);
    struct dirent *dir_data;
    Bool is_dir = TRUE;
    struct stat filestat;
    Bool file_found = FALSE;
    Bool inner_file_found = FALSE;
    FileTree *child;
    char new_path[4096];
    lstat(filepath, &filestat);
    file_found = check_file(&filestat, name, flags, filename, filesize, filetype, permissions, numoflinks, path);
    if (dir == NULL) {
        if (errno == ENOTDIR || is_link(&filestat)) {
            is_dir = FALSE;
        } else {
            perror(filepath);
            free_file_tree(root);
            shutdown(USER_ERROR_EXIT);
        }
    }
    if (is_dir) {
        do {
            errno = 0;
            dir_data = readdir(dir);
            if (dir_data != NULL) {

                if (!str_equal(dir_data->d_name, ".") && !str_equal(dir_data->d_name, "..")) {
                    if (parent == NULL) {
                        child = root;
                    } else {
                        child = peek_child(parent);
                    }
                    push_child(child, dir_data->d_name);
                    if(!ends_with(filepath, '/')){
                        str_cat(filepath, "/", new_path);
                        str_cat(new_path, dir_data->d_name, new_path);
                    }else{
                        str_cat(filepath, dir_data->d_name, new_path);
                    }
                    inner_file_found = build_file_tree(root, child, new_path, dir_data->d_name, flags, filename,
                                                       filesize, filetype, permissions, numoflinks, path);
                    if (inner_file_found) {
                        file_found = TRUE;
                    } else {
                        if (parent != NULL) {
                            pop_child(child);
                        } else {
                            pop_child(root);
                        }
                    }
                }
            } else {
                if (errno) {
                    closedir(dir);
                    perror(filepath);
                    free_file_tree(root);
                    shutdown(SYSTEM_ERROR_EXIT);
                }
            }
        } while (dir_data != NULL);
        closedir(dir);
    }
    return file_found;
}

void find(FlagHolder flags, char *filename, size_t filesize, FileType filetype, PermissionFlagHolder permissions,
          size_t numoflinks, char *path) {
    FileTree *tree = create_file_tree(path);
    struct dirent *dir_data;
    DIR *dir = NULL;
    char cur_path[4096];
    char name[256];
    Bool file_found = FALSE;
    dir = opendir(path);
    if (dir == NULL) {
        printf("PERROR\n");
        perror(path);
        free_file_tree(tree);
        shutdown(USER_ERROR_EXIT);
    }
    closedir(dir);
    str_copy(path, cur_path);
    str_copy(path, name);
    file_found = build_file_tree(tree, NULL, cur_path, name, flags, filename, filesize, filetype, permissions,
                                 numoflinks, path);
    if (file_found) {
        pretty_print_tree(tree);
    } else {
        safe_write(STDOUT_FILENO, "No file found\n", 15);
    }
    free_file_tree(tree);
}

PermissionFlagHolder get_permission_flag_from_st_mode(__mode_t mod) {
    PermissionFlagHolder flag = 0;
    if (mod & S_IRUSR) {
        flag += 256; /* 100000000 */
    }
    if (mod & S_IWUSR) {
        flag += 128; /* 010000000 */
    }
    if (mod & S_IXUSR) {
        flag += 64; /* 001000000 */
    }
    if (mod & S_IRGRP) {
        flag += 32; /* 000100000 */
    }
    if (mod & S_IWGRP) {
        flag += 16; /* 000010000 */
    }
    if (mod & S_IXGRP) {
        flag += 8; /* 000001000 */
    }
    if (mod & S_IROTH) {
        flag += 4; /* 000000100 */
    }
    if (mod & S_IWOTH) {
        flag += 2; /* 000000010 */
    }
    if (mod & S_IXOTH) {
        flag += 1; /* 000000001 */
    }
    return flag;
}
Bool ends_with(const char *str, char c){
    int i;
    for(i=0; str[i]!='\0'; ++i);
    return str[i-1] == c;
}

Bool is_link(struct stat *filestat){
    return S_ISLNK(filestat->st_mode);
}