#include <stddef.h>
#include <stdint.h>
#include "defs.h"

static inline void outb(uint16_t port, uint32_t value) {
  asm("out %0,%1" : /* empty */ : "a" (value), "Nd" (port) : "memory");
}

static inline uint32_t inb(uint16_t port) {
  uint32_t ret;
  asm("in %1, %0" : "=a"(ret) : "Nd"(port) : "memory" );
  return ret;
}

void printVal(uint32_t val) {
	outb(PORT_PRINT_VALUE, val);
}

uint32_t getNumExits(void) {
	return inb(PORT_GETNUMEXITS);
}

// display prints the argument string to the screen via a hypercall, incuring only one exit.
void display(const char *str) {
	uint32_t temp = str - (char *)0; // Cast char * to uint32_t
	outb(PORT_DISPLAY, temp);
}

int open(const char *filename) {
	uint32_t temp = filename - (char *)0; // Cast char * to uint32_t
	outb(PORT_OPEN, temp);
	temp = inb(PORT_OPEN);
	return temp;
}

void
__attribute__((noreturn))
__attribute__((section(".start")))
_start(void) {
	const char *p;
	int fd;

	/* Print Hello world */
	for (p = "Hello, world!\n"; *p; ++p)
		outb(PORT_PRINT_CHAR, *p);

	/* Test printVal() */
	printVal(1234);
	outb(PORT_PRINT_CHAR, '\n');

	/* Test getNumExits() */
	uint32_t numExits = getNumExits();
	printVal(numExits);
	outb(PORT_PRINT_CHAR, '\n');

	/* Test display() */
	printVal(getNumExits());
	display("\nHello world again!\n");
	printVal(getNumExits());
	display("\n");

	/* Test open() */
	display("Opening test.txt\n");
	fd = open("test.txt");
	printVal(fd);
	display("\n");

	/* Halt after setting 0x400 & rax as 42 */
	*(long *) 0x400 = 42;
	for (;;)
		asm("hlt" : /* empty */ : "a" (42) : "memory");
}
