#include "find.h"
#include "constants.h"
#include "file_tree.h"
#include "str_utils.h"
#include "syscall_utils.h"
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

int get_permission_flag_from_st_mode(__mode_t mod);
Bool build_file_tree(FileTree *root, FileTree *parent, char filepath[4096], char name[256], FlagHolder flags, char *filename, size_t filesize, FileType filetype, PermissionFlagHolder permissions, size_t numoflinks, char *path);
Bool check_file(struct stat *filestat, char *name, FlagHolder flags, char *filename, size_t filesize, FileType filetype, PermissionFlagHolder permissions, size_t numoflinks, char *path);

Bool check_file(struct stat *filestat, char *name, FlagHolder flags, char *filename, size_t filesize, FileType filetype, PermissionFlagHolder permissions, size_t numoflinks, char *path)
{
    printf("Checking file: %s\n", name);
    Bool result = TRUE;
    char lowername[256];
    if (flags & FILENAME_FLAG != 0)
    {
        str_to_lower(name, lowername);
        result = str_equal(lowername, filename);
        if (!result)
        {
            return FALSE;
        }
    }
    if (flags & FILESIZE_FLAG != 0)
    {
        result = filesize == filestat->st_size;
        if (!result)
        {
            return FALSE;
        }
    }
    if (flags & FILETYPE_FLAG != 0)
    {
        switch (filetype)
        {
        case F_DIRECTORY:
            result = filestat->st_mode & S_IFDIR;
            break;
        case F_SOCKET:
            result = filestat->st_mode & S_IFSOCK;
            break;
        case F_B_DEVICE:
            result = filestat->st_mode & S_IFBLK;
            break;
        case F_C_DEVICE:
            result = filestat->st_mode & S_IFCHR;
            break;
        case F_FILE:
            result = filestat->st_mode & S_IFREG;
            break;
        case F_PIPE:
            result = filestat->st_mode & S_IFIFO;
            break;
        case F_LINK:
            result = filestat->st_mode & S_IFLNK;
            break;
        default:
            result = FALSE;
            break;
        }
        if (!result)
        {
            return FALSE;
        }
    }
    if (flags & PERMISSIONS_FLAG != 0)
    {
        result = get_permission_flag_from_st_mode(filestat->st_mode) == permissions;
        if (!result)
        {
            return FALSE;
        }
    }
    if (flags & NUMOFLINKS_FLAG != 0)
    {
        result = filestat->st_nlink == numoflinks;
    }
    return result;
}

Bool build_file_tree(FileTree *root, FileTree *parent, char filepath[4096], char name[256], FlagHolder flags, char *filename, size_t filesize, FileType filetype, PermissionFlagHolder permissions, size_t numoflinks, char *path)
{
    DIR *dir = opendir(filepath);
    struct dirent *dir_data;
    Bool is_dir = TRUE;
    struct stat filestat;
    Bool file_found = FALSE;
    Bool inner_file_found = FALSE;
    FileTree *child;
    printf("build tree file: %s\n", filepath);
    if (dir == NULL)
    {
        if (errno == ENOTDIR)
        {
            is_dir = FALSE;
        }
        else
        {
            perror(path);
            shutdown(USER_ERROR_EXIT);
        }
    }
    stat(filepath, &filestat);
    file_found = check_file(&filestat, name, flags, filename, filesize, filetype, permissions, numoflinks, path);
    if (is_dir)
    {
        do
        {
            errno = 0;
            dir_data = readdir(dir);
            if (dir_data != NULL)
            {

                if (parent == NULL)
                {
                    child = root;
                }
                else
                {
                    child = peek_child(parent);
                }
                push_child(child, name);
                pretty_print_tree(root);
                str_cat(filepath, "/", filepath);
                str_cat(filepath, dir_data->d_name, filepath);
                inner_file_found = build_file_tree(root, child, filepath, dir_data->d_name, flags, filename, filesize, filetype, permissions, numoflinks, path);
                if (inner_file_found)
                {
                    file_found = TRUE;
                }
                else
                {
                    if (parent != NULL)
                    {
                        pop_child(parent);
                    }
                    else
                    {
                        return FALSE;
                    }
                }
            }
            else
            {
                if (errno)
                {
                    closedir(dir);
                    perror(path);
                    shutdown(SYSTEM_ERROR_EXIT);
                }
            }
        } while (dir_data != NULL);
    }
    return file_found;
}

void find(FlagHolder flags, char *filename, size_t filesize, FileType filetype, PermissionFlagHolder permissions, size_t numoflinks, char *path)
{
    FileTree *tree = create_file_tree(path);
    struct dirent *dir_data;
    DIR *dir = NULL;
    char cur_path[4096];
    char name[256];
    Bool file_found = FALSE;
    printf("FIND path: -%s-\n", path);
    dir = opendir(path);
    if (dir == NULL)
    {
        printf("PERROR\n");
        perror(path);
        shutdown(USER_ERROR_EXIT);
    }
    closedir(dir);
    str_copy(path, cur_path);
    str_copy(path, name);
    printf("before build\n");
    file_found = build_file_tree(tree, NULL, cur_path, name, flags, filename, filesize, filetype, permissions, numoflinks, path);
    if (file_found)
    {
        pretty_print_tree(tree);
    }
    else
    {
        safe_write(STDOUT_FILENO, "No file found\n", 15);
    }
    free_file_tree(tree);
}

PermissionFlagHolder get_permission_flag_from_st_mode(__mode_t mod)
{
    PermissionFlagHolder flag = 0;
    if (mod & S_IRUSR)
    {
        flag += 256; /* 100000000 */
    }
    if (mod & S_IWUSR)
    {
        flag += 128; /* 010000000 */
    }
    if (mod & S_IXUSR)
    {
        flag += 64; /* 001000000 */
    }
    if (mod & S_IRGRP)
    {
        flag += 32; /* 000100000 */
    }
    if (mod & S_IWGRP)
    {
        flag += 16; /* 000010000 */
    }
    if (mod & S_IXGRP)
    {
        flag += 8; /* 000001000 */
    }
    if (mod & S_IROTH)
    {
        flag += 4; /* 000000100 */
    }
    if (mod & S_IWOTH)
    {
        flag += 2; /* 000000010 */
    }
    if (mod & S_IXOTH)
    {
        flag += 1; /* 000000001 */
    }
    return flag;
}
