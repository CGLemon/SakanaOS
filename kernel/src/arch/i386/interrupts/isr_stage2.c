#include <arch/i386/isr.h>
#include <arch/i386/ports.h>
#include <arch/i386/pic.h>

isr_interrupt_listener_t listeners[ISR_MAX_INTERRUPT_LISTENERS] = { ISR_NULL_LISTENER };

const char * isr_exception_messages[] = {
	"Division By Zero",
	"Debug",
	"Non Maskable Interrupt",
	"Breakpoint",
	"Detected Overflow",
	"Out Of Bounds",
	"Invalid Opcode",
	"No Coprocessor",
	"Double Fault",
	"Coprocessor Segment Overrun",
	"Bad TSS",
	"Segment Not Present",
	"Stack Fault",
	"General Protection Fault",
	"Page Fault",
	"Unknown Interrupt",
	"Coprocessor Fault",
	"Alignment Check",
	"Machine Check",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved"
};

void isr_stage2(isr_cpu_state_t * state) {
    uint32_t interrupt_code = state->interrupt_code;
    isr_interrupt_listener_t listener = listeners[interrupt_code];

    if (listener != ISR_NULL_LISTENER) {
        listener(state);
    }

	if (32 <= interrupt_code && interrupt_code < 48) {
        uint32_t pic_8259_code = interrupt_code - 32;
        if (pic_8259_code < 16) {
		    pic_8259_send_eoi(pic_8259_code);
        }
	}

    if (listener == ISR_NULL_LISTENER &&
            32 > state->interrupt_code &&
            0x80 != state->interrupt_code) {
        // TODO: system call
    }
}

uint32_t isr_register_listener(isr_interrupt_t selector, isr_interrupt_listener_t listener) {
    if (ISR_MAX_INTERRUPT_LISTENERS <= selector) {
        return -1;
    }

    listeners[selector] = listener;
    return 0;
}

uint32_t isr_unregister_listener(isr_interrupt_t selector) {
    if (ISR_MAX_INTERRUPT_LISTENERS <= selector) {
        return -1;
    }

    listeners[selector] = ISR_NULL_LISTENER;
    return 0;
}
