#include "file_tree.h"
#include "constants.h"
#include "str_utils.h"
#include "syscall_utils.h"
#include <stdlib.h>
#include <unistd.h>

void expand_tree(FileTree *tree);
void clear_tree(FileTree *tree);
void _pretty_print_tree(FileTree *tree, int depth);

FileTree *create_file_tree(char filename[256])
{
    FileTree *tree = (FileTree *)malloc(sizeof(FileTree));
    tree->capacity = 2;
    tree->size = 0;
    tree->children = (FileTree *)malloc(sizeof(FileTree) * 2);
    str_copy(filename, tree->node.filename);
    return tree;
}
void push_child(FileTree *tree, char filename[256])
{
    FileTree child;
    child.capacity = 2;
    child.size = 0;
    child.children = (FileTree *)malloc(sizeof(FileTree) * 2);
    ++(tree->size);
    if (tree->size >= tree->capacity)
    {
        expand_tree(tree);
    }
    str_copy(filename, child.node.filename);
    tree->children[tree->size - 1] = child;
}
FileTree pop_child(FileTree *tree)
{
    --(tree->size);
    return tree->children[tree->size];
}
FileTree *peek_child(FileTree *tree)
{
    return &(tree->children[tree->size - 1]);
}
void free_file_tree(FileTree *tree)
{
    clear_tree(tree);
    free(tree);
}
void clear_tree(FileTree *tree)
{
    FileTree t;
    while (tree->size > 0)
    {
        t = pop_child(tree);
        clear_tree(&t);
    }
    free(tree->children);
}

void pretty_print_tree(FileTree *tree)
{
    _pretty_print_tree(tree, 0);
}

void _pretty_print_tree(FileTree *tree, int depth)
{
    int i;
    if (depth != 0)
    {
        safe_write(STDOUT_FILENO, "|", 2);
    }
    for (i = 0; i < depth; ++i)
    {
        safe_write(STDOUT_FILENO, "-", 2);
    }
    safe_write(STDOUT_FILENO, tree->node.filename, str_len(tree->node.filename));
    safe_write(STDOUT_FILENO, "\n", 2);
    for (i = 0; i < tree->size; ++i)
    {
        _pretty_print_tree(&(tree->children[i]), depth + 1);
    }
}

void expand_tree(FileTree *tree)
{
    int i;
    FileTree *new_children = (FileTree *)malloc(sizeof(FileTree) * tree->capacity * 2);
    for (i = 0; i < tree->size; ++i)
    {
        new_children[i] = tree->children[i];
    }
    free(tree->children);
    tree->children = new_children;
    tree->capacity *= 2;
}