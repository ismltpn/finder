#ifndef CONSTANTS_H
#define CONSTANTS_H

/* File Type */
typedef enum file_type {
    F_UNKNOWN = 0,
    F_DIRECTORY = 1,
    F_SOCKET = 2,
    F_B_DEVICE = 3,
    F_C_DEVICE = 4,
    F_FILE = 5,
    F_PIPE = 6,
    F_LINK = 7
} FileType;

/* Bool */
typedef enum bool {
    FALSE = 0,
    TRUE = 1
} Bool;

/* FLags */
#define B_00000000 0
#define B_00000001 1
#define B_00000010 2
#define B_00000100 4
#define B_00001000 8
#define B_00010000 16
#define B_00100000 32

typedef char FlagHolder;
#define NOFLAG B_00000000

#define FILENAME_FLAG B_00000001
#define FILESIZE_FLAG B_00000010
#define FILETYPE_FLAG B_00000100
#define PERMISSIONS_FLAG B_00001000
#define NUMOFLINKS_FLAG B_00010000
#define PATH_FLAG B_00100000

typedef int PermissionFlagHolder;

/* Exit codes */
#define USER_ERROR_EXIT 1
#define SYSTEM_ERROR_EXIT -1
#define NO_ERROR_EXIT 0

/* Strings */
#define S_WRONG_USAGE_ERR "Usage: "
#define S_ARGUMENT_EXAMPLES " -f filename.ext -b 809632 -t f -p rwxr-xr-- -l 2 -w .\n -w path (required)\n -f filename\n -b filesize\n -t filetype (d s b c f p or l)\n -p permissions\n -l number of links\n"

#endif