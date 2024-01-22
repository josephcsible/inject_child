/* SPDX-License-Identifier: AGPL-3.0-or-later */

#define _GNU_SOURCE

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

extern char _binary_child_start[], _binary_child_end[];
extern char **environ;

#ifndef MFD_EXEC
#define MFD_EXEC 0x0010U
#endif

static int memfd_create_exec(const char *name, unsigned int flags) {
    int fd = memfd_create(name, flags | MFD_EXEC);
    if(fd == -1 && errno == EINVAL) {
        fd = memfd_create(name, flags);
    }
    return fd;
}

static bool writeloop(int fd, const void *buf, size_t count) {
    size_t written = 0;
    while(written < count) {
        ssize_t rv = write(fd, (const char *)buf + written, count - written);
        if(rv < 0) return true;
        written += (size_t)rv;
    }
    return false;
}

__attribute__((__constructor__))
static void inject(void) {
    pid_t pid = fork();
    switch(pid) {
    case -1:
        perror("fork");
        abort();
    case 0:
        int fd = memfd_create_exec("", MFD_CLOEXEC);
        if(fd == -1) {
            perror("memfd_create");
            abort();
        }
        if(writeloop(fd, _binary_child_start, _binary_child_end - _binary_child_start)) {
            perror("write");
            abort();
        }
        if(unsetenv("LD_PRELOAD")) { // Without this, we'd accidentally fork bomb when the child binary is dynamically linked
            perror("unsetenv");
            abort();
        }
        char *argv[] = {"./child", NULL};
        fexecve(fd, argv, environ);
        perror("fexecve");
        abort();
    default:
        int wstatus;
        if(waitpid(pid, &wstatus, 0) != pid) {
            perror("waitpid");
        }
        if(!WIFEXITED(wstatus) || WEXITSTATUS(wstatus)) {
             fprintf(stderr, "wstatus was %d\n", wstatus);
             abort();
        }
    }
}
