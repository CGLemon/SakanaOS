#include <shell/shell.h>
#include <memory/heap.h>
#include <system/panic.h>
#include <system/message.h>
#include <io/stream.h>
#include <utils/string.h>

void shell_execute_help(shell_t * s);
void shell_execute_message(shell_t * s);
void shell_execute_memory(shell_t * s);

shell_t * shell_create(tty_t * tty) {
    shell_t * s = (shell_t *)kmalloc(sizeof(shell_t));
    if (!s) {
        KPANIC(KPANIC_OUT_OF_MEMORY, "Memory allocation failed.", NULL);
    }

    s->commands_list = linked_list_create(kmalloc);
    if (!s->commands_list) {
        KPANIC(KPANIC_OUT_OF_MEMORY, "Memory allocation failed.", NULL);
    }

    shell_init(s, tty);
    shell_register_command(s, "help", shell_execute_help);
    shell_register_command(s, "message", shell_execute_message);
    shell_register_command(s, "memory", shell_execute_memory);

    return s;
}

void shell_destroy(shell_t * s) {
    linked_list_destroy(s->commands_list, kfree);
    kfree(s);
}

void shell_init(shell_t * s, tty_t * tty) {
    s->in_stream = tty_get_in_stream(tty);
    s->out_stream = tty_get_out_stream(tty);
    s->err_stream = tty_get_err_stream(tty);
}

void shell_execute(shell_t * s) {
    while (1) {
        stream_puts(s->out_stream, "> ");
        char * inputs = stream_gets(s->in_stream);
        shell_execute_command(s, inputs);
        kfree(inputs);
    }
}

void shell_execute_command(shell_t * s, char * name) {
    linked_list_foreach(s->commands_list) {
        shell_command_t * c = (shell_command_t *)node->data;
        if (strcmp(name, c->name) == 0) {
            c->call(s);
        }
    } linked_list_foreach_end;
}

void shell_register_command(shell_t * s, char * name, shell_command_call_t call) {
    shell_command_t * c = (shell_command_t *)kmalloc(sizeof(shell_command_t));
    if (!c) {
        KPANIC(KPANIC_OUT_OF_MEMORY, "Memory allocation failed.", NULL);
    }

    c->name = (char *)kmalloc(strlen(name));
    if (!c->name) {
        KPANIC(KPANIC_OUT_OF_MEMORY, "Memory allocation failed.", NULL);
    }
    strcpy(c->name, name);
    c->call = call;    

    linked_list_node_t * node = linked_list_node_create((void *)c, kmalloc);
    if (!node) {
        KPANIC(KPANIC_OUT_OF_MEMORY, "Memory allocation failed.", NULL);
    }
    linked_list_append(s->commands_list, node);
}

void shell_execute_help(shell_t * s) {
    int i = 0;
    linked_list_foreach(s->commands_list) {
        shell_command_t * c = (shell_command_t *)node->data;
        stream_printf(s->out_stream, "[%d] %s\n", ++i, c->name);
    } linked_list_foreach_end;
}

void shell_execute_message(shell_t * s) {
    kmessage_dump(s->out_stream);
}

void shell_execute_memory(shell_t * s) {
    kheap_dump(s->out_stream);
}
