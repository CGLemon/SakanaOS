#include <system/message.h>
#include <system/panic.h>
#include <memory/heap.h>
#include <utils/circular_buffer.h>

#include <drivers/video/vga/vga.h>

circular_buffer_t * kmessage_log_buffer;
void kmessage_log_stream_gets(kmessage_log_t * log, stream_t * stream);

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

void kmessage_log_stream_gets(kmessage_log_t * log, stream_t * stream) {
    stream_puts(stream, "[");
    if (log->level == KMESSAGE_LEVEL_PANIC) {
        stream_puts(stream, "DEBUG");
    }
    if (log->level == KMESSAGE_LEVEL_INFO) {
        stream_puts(stream, "INFO");
    }
    if (log->level == KMESSAGE_LEVEL_WARN) {
        stream_puts(stream, "WARN");
    }
    if (log->level == KMESSAGE_LEVEL_ERROR) {
        stream_puts(stream, "ERROR");
    }
    stream_puts(stream, "] ");
    stream_puts(stream, log->message);
    stream_puts(stream, "\n");
}

void kmessage_dump(stream_t * stream) {
    circular_buffer_foreach(kmessage_log_buffer) {
        kmessage_log_stream_gets((kmessage_log_t *)element, stream);
    } circular_buffer_foreach_end;
}
