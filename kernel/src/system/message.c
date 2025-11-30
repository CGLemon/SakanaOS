#include <system/message.h>
#include <system/panic.h>
#include <memory/heap.h>

static circular_buffer_t * kmessage_log_buffer;

void kmessage_init() {
    kmessage_log_buffer = circular_buffer_create(
        MSG_BUFFER_SIZE, sizeof(kmessage_log_t), kmalloc);
    if (!kmessage_log_buffer) {
        KPANIC(KPANIC_OUT_OF_MEMORY, "Memory allocation failed.", NULL);
    }
}

void kmessage(int level, const char * message) {
    kmessage_log_t log;
    kmessage_log_t * log_ptr = &log; 
    memset(log_ptr, 0, sizeof(kmessage_log_t));

    log_ptr->level = level;
    size_t msg_len = strlen(message);
    if (msg_len >= MSG_LOG_SIZE) {
        msg_len = MSG_LOG_SIZE-1;
    }
    memcpy(log_ptr->message, message, msg_len);
    log_ptr->message[msg_len] = '\0';

    if (circular_buffer_full(kmessage_log_buffer)) {
        circular_buffer_dequeue(kmessage_log_buffer, NULL);
    }
    circular_buffer_enqueue(kmessage_log_buffer, (void *)log_ptr);
}

circular_buffer_t * kmessage_get_log() {
    return kmessage_log_buffer;
}
