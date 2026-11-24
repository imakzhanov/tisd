#define _POSIX_C_SOURCE 200809L

#include "io.h"

#include <errno.h>
#include <spawn.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

extern char **environ;

// Запускает Graphviz для преобразования DOT в PNG и проверяет результат.
static int render_png(char *dot_path, char *png_path)
{
    char *args[] = {"dot", "-Tpng", dot_path, "-o", png_path, NULL};
    pid_t child;
    int rc = posix_spawnp(&child, args[0], NULL, NULL, args, environ);
    if (rc != 0)
        return rc == ENOENT ? GRAPHVIZ_MISSING : GRAPHVIZ_ERROR;

    int status;
    pid_t waited;
    do
        waited = waitpid(child, &status, 0);
    while (waited == -1 && errno == EINTR);

    if (waited == -1 || !WIFEXITED(status) || WEXITSTATUS(status) != 0)
        return GRAPHVIZ_ERROR;
    return OK;
}

// Сохраняет дерево в PNG с помощью Graphviz.
int export_tree_png(const node_t *root, const char *letter)
{
    char dot_path[] = "tree.dot";
    int rc = export_tree_dot(root, dot_path, letter);
    if (rc != OK)
        return rc;

    char temporary[] = "tree.png.XXXXXX";
    int file = mkstemp(temporary);
    if (file == -1)
        return FILE_ERROR;

    if (close(file) == -1)
        rc = FILE_ERROR;
    else
        rc = render_png(dot_path, temporary);

    if (rc == OK && rename(temporary, "tree.png") == -1)
        rc = FILE_ERROR;
    if (rc != OK)
        unlink(temporary);
    return rc;
}
