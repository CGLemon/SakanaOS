#ifndef _KERNEL_SYSTEM_MESSAGE_H
#define _KERNEL_SYSTEM_MESSAGE_H

#include <utils/string.h>
#include <utils/circular_buffer.h>

#define KMESSAGE_LEVEL_PANIC 0
#define KMESSAGE_LEVEL_INFO 1
#define KMESSAGE_LEVEL_WARN 2
#define KMESSAGE_LEVEL_ERROR 3

#define MSG_BUFFER_SIZE 128
#define MSG_LOG_SIZE 128

typedef struct kmessage_log {
    int level;
    char message[MSG_LOG_SIZE];
} kmessage_log_t;

void kmessage_init();

void kmessage(int level, const char * message);

circular_buffer_t * kmessage_get_log();


#endif
