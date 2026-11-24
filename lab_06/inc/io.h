#ifndef IO_H
#define IO_H

#include <stdio.h>

#include "tree.h"

int read_choice(void);
int read_file_path(char path[PATH_CAPACITY]);
int read_key(char word[WORD_CAPACITY], int single_letter);
int file_read_word(FILE *file, char word[WORD_CAPACITY], int *has_word);
int load_tree(const char *path, node_t **root);
int file_count_prefix(FILE *file, const char *letter, size_t *count, size_t *comparisons);
void print_tree(const node_t *root, const char *letter);
int export_tree_dot(const node_t *root, const char *path, const char *letter);
int export_tree_png(const node_t *root, const char *letter);
void print_error(int rc);

#endif
