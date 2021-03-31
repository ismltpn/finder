#include <unistd.h>
#include "constants.h"
#include "arg_utils.h"
#include "find.h"
#include "syscall_utils.h"

Bool validate_flags(FlagHolder flags);

void print_usage();

int main(int argc, char *argv[])
{
    int opt;
    char filename[256];
    size_t filesize;
    FileType filetype;
    PermissionFlagHolder permissions;
    size_t numoflinks;
    char path[4096];
    Bool err = FALSE;
    FlagHolder flags = NOFLAG;

    while ((opt = getopt(argc, argv, "f:b:t:p:l:w:")) != -1)
    {
        switch (opt)
        {
        case 'f':
            flags = flags | FILENAME_FLAG;
            get_filename_arg(optarg, filename);
            break;
        case 'b':
            flags = flags | FILESIZE_FLAG;
            filesize = get_filesize_arg(optarg, &err);
            if (err)
            {
                safe_write(STDERR_FILENO, "Invalid integer format\n", 41);
                print_usage(argv[0]);
                return USER_ERROR_EXIT;
            }
            break;
        case 't':
            flags = flags | FILETYPE_FLAG;
            filetype = get_filetype_arg(optarg, &err);
            if (err)
            {
                safe_write(STDERR_FILENO, "File type parameter must be 1 character\n", 41);
                print_usage(argv[0]);
                return USER_ERROR_EXIT;
            }
            if (filetype == F_UNKNOWN)
            {
                safe_write(STDERR_FILENO, "Unknown file type\n", 19);
                print_usage(argv[0]);
                return USER_ERROR_EXIT;
            }
            break;
        case 'p':
            flags = flags | PERMISSIONS_FLAG;
            permissions = get_permissions_arg(optarg, &err);
            if (err)
            {
                safe_write(STDERR_FILENO, "Invalid permission format\n", 27);
                print_usage(argv[0]);
                return USER_ERROR_EXIT;
            }
            break;
        case 'l':
            flags = flags | NUMOFLINKS_FLAG;
            numoflinks = get_numoflinks_arg(optarg, &err);
            if (err)
            {
                safe_write(STDERR_FILENO, "Invalid integer format\n", 41);
                print_usage(argv[0]);
                return USER_ERROR_EXIT;
            }
            break;
        case 'w':
            flags = flags | PATH_FLAG;
            get_path_arg(optarg, path);
            break;
        default:
            print_usage(argv[0]);
            return USER_ERROR_EXIT;
        }
    }
    if (!validate_flags(flags))
    {
        return USER_ERROR_EXIT;
    }
    find(flags, filename, filesize, filetype, permissions, numoflinks, path);

    return 0;
}

Bool validate_flags(FlagHolder flags)
{
    // at least one filter
    FlagHolder filter_validation_flag =
        FILENAME_FLAG | FILESIZE_FLAG | FILETYPE_FLAG | PERMISSIONS_FLAG | NUMOFLINKS_FLAG;
    if ((filter_validation_flag & flags) == 0)
    {
        safe_write(STDERR_FILENO, "You have to provide at least one filter.\n", 42);
        return FALSE;
    }
    // path
    if ((flags & PATH_FLAG) == 0)
    {
        safe_write(STDERR_FILENO, "You have to provide a path.\n", 29);
        return FALSE;
    }
    return TRUE;
}

void print_usage(char *name)
{
    const char *wrong_usage_err[3] = {S_WRONG_USAGE_ERR, name, S_ARGUMENT_EXAMPLES};
    write_multiple(STDERR_FILENO, wrong_usage_err, 3);
}