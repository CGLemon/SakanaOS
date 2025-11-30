#include <arch/i386/isr.h>
#include <device/device.h>
#include <device/storage.h>
#include <memory/heap.h>
#include <shell/shell.h>
#include <system/panic.h>
#include <system/message.h>
#include <io/stream.h>
#include <utils/string.h>

static int shell_execute_help(shell_t * s, int argc, char ** argv);
static int shell_execute_clear(shell_t * s, int argc, char ** argv);
static int shell_execute_message(shell_t * s, int argc, char ** argv);
static int shell_execute_memory(shell_t * s, int argc, char ** argv);
static int shell_execute_ata_read(shell_t * s, int argc, char ** argv);
static int shell_execute_ata_write(shell_t * s, int argc, char ** argv);
static int shell_execute_interrupt(shell_t * s, int argc, char ** argv);

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
    shell_register_command(s, "clear", shell_execute_clear);
    shell_register_command(s, "message", shell_execute_message);
    shell_register_command(s, "memory", shell_execute_memory);
    shell_register_command(s, "ataread", shell_execute_ata_read);
    shell_register_command(s, "atawrite", shell_execute_ata_write);
    shell_register_command(s, "int", shell_execute_interrupt);

    return s;
}

void shell_destroy(shell_t * s) {
    linked_list_destroy(s->commands_list, true, kfree);
    kfree(s);
}

void shell_init(shell_t * s, tty_t * tty) {
    s->tty = tty;
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

void shell_execute_command(shell_t * s, char * inputs) {
    size_t len = strlen(inputs);
    size_t num_ws = 0;
    for (int i = 0; i < len; ++i) {
        if (inputs[i] == ' ') {
            num_ws += 1;
        }
    }

    int argc = 0;
    char ** argv = (char **)kmalloc((num_ws + 1) * sizeof(char *));
    char * token = strtok(inputs, " ");

    if (!token) {
        return;
    }
    do {
        argv[argc++] = token;
        token = strtok(NULL, " ");
    } while(token);

    char * name = argv[0];
    bool unknown = true;
    linked_list_foreach(s->commands_list) {
        shell_command_t * c = (shell_command_t *)node->data;
        if (strcmp(name, c->name) == 0) {
            int errno = c->call(s, argc, argv);
            if (errno != 0) {
                stream_printf(s->out_stream, "Fail to execute: %s\n", name);
            }
            unknown = false;
        }
    } linked_list_foreach_end;

    kfree(argv);
    if (unknown) {
        stream_puts(s->out_stream, "Unknown command\n");
    }
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

int shell_execute_help(shell_t * s, int argc, char ** argv) {
    int i = 0;
    linked_list_foreach(s->commands_list) {
        shell_command_t * c = (shell_command_t *)node->data;
        stream_printf(s->out_stream, "[%d] %s\n", ++i, c->name);
    } linked_list_foreach_end;

    return 0;
}

int shell_execute_clear(shell_t * s, int argc, char ** argv) {
    s->tty->video->driver->tm.clean_screen();
    return 0;
}

int shell_execute_message(shell_t * s, int argc, char ** argv) {
    circular_buffer_t * log_buffer = kmessage_get_log();

    circular_buffer_foreach(log_buffer) {
        kmessage_log_t * log = (kmessage_log_t *)element;

        stream_puts(s->out_stream, "[");
        if (log->level == KMESSAGE_LEVEL_PANIC) {
            stream_puts(s->out_stream, "DEBUG");
        }
        if (log->level == KMESSAGE_LEVEL_INFO) {
            stream_puts(s->out_stream, "INFO");
        }
        if (log->level == KMESSAGE_LEVEL_WARN) {
            stream_puts(s->out_stream, "WARN");
        }
        if (log->level == KMESSAGE_LEVEL_ERROR) {
            stream_puts(s->out_stream, "ERROR");
        }
        stream_puts(s->out_stream, "] ");
        stream_puts(s->out_stream, log->message);
        stream_puts(s->out_stream, "\n");
    } circular_buffer_foreach_end;
    return 0;
}

int shell_execute_memory(shell_t * s, int argc, char ** argv) {
    bool heap_enabled = kheap_is_enabled();
    stream_printf(
        s->out_stream,
        "heap_enabled: %s\n",
        heap_enabled ? "True" : "False");

    int i = 0;
    size_t allmemory = 0;
    if (heap_enabled) {
        kheap_block_head_t * block = kheap_get_heap_head();
        while (block != NULL) {
            stream_printf(
                s->out_stream,
                "[%d] s=%d, f=%d.\n",
                i++, block->size, block->free);

            allmemory += (block->size + sizeof(kheap_block_head_t));
            block = block->next;
        }
    }
    stream_printf(s->out_stream, "tot = %x.\n", allmemory);
    return 0;
}

int shell_execute_ata_read(shell_t * s, int argc, char ** argv) {
    if (argc == 3) {
        uint32_t addr = (uint32_t)atoi(argv[1]);
        size_t read_size = (size_t)atoi(argv[2]);
        char * data = (char *)kmalloc(read_size * sizeof(char) + 1);

        storage_device_t * device =
            (storage_device_t *)device_find_by_type(DEVICE_TYPE_STORAGE);
        device->driver->read(addr, data, read_size);

        data[read_size] = '\0';
        stream_puts(s->out_stream, "read: ");
        stream_puts(s->out_stream, data);
        stream_puts(s->out_stream, "\n");
        kfree(data);
    } else {
        return -1;
    }
    return 0;
}

int shell_execute_ata_write(shell_t * s, int argc, char ** argv) {
    if (argc == 3) {
        uint32_t addr = (uint32_t)atoi(argv[1]);
        void * data = (void *)argv[2];
        size_t write_size = strlen(argv[2]);

        storage_device_t * device =
            (storage_device_t *)device_find_by_type(DEVICE_TYPE_STORAGE);
        device->driver->write(addr, data, write_size);
    } else {
        return -1;
    }
    return 0;
}

int shell_execute_interrupt(shell_t * s, int argc, char ** argv) {
    if (argc == 2) {
        int i = atoi(argv[1]);
        isr_interrupt(i);
    } else {
        return -1;
    }
    return 0;
}
