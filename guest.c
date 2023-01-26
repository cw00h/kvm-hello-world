#include <stddef.h>
#include <stdint.h>
#include "defs.h"

static inline void outb(uint16_t port, uint32_t value) {
  asm("out %0,%1" : /* empty */ : "a" (value), "Nd" (port) : "memory");
}

void printVal(uint32_t val) {
	outb(PORT_PRINT_VALUE, val);
}

void
__attribute__((noreturn))
__attribute__((section(".start")))
_start(void) {
	const char *p;

	/* Print Hello world */
	for (p = "Hello, world!\n"; *p; ++p)
		outb(PORT_PRINT_CHAR, *p);

	/* Test printVal() */
	printVal(0x1234);
	outb(PORT_PRINT_CHAR, '\n');

	/* Halt after setting 0x400 & rax as 42 */
	*(long *) 0x400 = 42;
	for (;;)
		asm("hlt" : /* empty */ : "a" (42) : "memory");
}
