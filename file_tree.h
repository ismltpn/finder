#ifndef FILE_TREE_H
#define FILE_TREE_H

#include "constants.h"
#include <unistd.h>

typedef struct file_node {
    char filename[256];
} FileNode;

typedef struct file_tree {
    FileNode node;
    size_t size;
    size_t capacity;
    struct file_tree *children;
} FileTree;

FileTree *create_file_tree(char filename[256]);

void push_child(FileTree *tree, char filename[256]);

FileTree pop_child(FileTree *tree);

FileTree *peek_child(FileTree *tree);

void free_file_tree(FileTree *tree);

void pretty_print_tree(FileTree *tree);

#endif