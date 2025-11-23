#ifndef _KERNEL_SHELL_SHELL_H
#define _KERNEL_SHELL_SHELL_H

#include <io/stream.h>
#include <io/tty.h>
#include <utils/linked_list.h>

typedef struct shell {
    stream_t * in_stream;
    stream_t * out_stream;
    stream_t * err_stream;
    linked_list_t  * commands_list;
} shell_t;

typedef void (* shell_command_call_t)(shell_t *);

typedef struct shell_command {
    char * name;
    shell_command_call_t call;
} shell_command_t;

shell_t * shell_create();
void shell_init(shell_t * s, tty_t * tty);
void shell_execute(shell_t * s);
void shell_register_command(shell_t * s, char * name, shell_command_call_t call);
void shell_execute_command(shell_t * s, char * name);

#endif
