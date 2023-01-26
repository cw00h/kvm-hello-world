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

int read(int fd, void *buf, size_t count) {
	struct rw_args args[1];
	args[0].fd = fd;
	args[0].buf = buf;
	args[0].count = count;

	uint32_t temp = (char *)args - (char *)0; // Cast struct rw_args * to uint32_t
	outb(PORT_READ, temp);
	temp = inb(PORT_READ);
	return temp;
}

int write(int fd, void *buf, size_t count) {
	struct rw_args args[1];
	args[0].fd = fd;
	args[0].buf = buf;
	args[0].count = count;

	uint32_t temp = (char *)args - (char *)0; // Cast struct rw_args * to uint32_t
	outb(PORT_WRITE, temp);
	temp = inb(PORT_WRITE);
	return temp;
}

void
__attribute__((noreturn))
__attribute__((section(".start")))
_start(void) {
	const char *p;
	int fd, fd1, fd2, fd3, fd4;

	/* Print Hello world */
	display("----------------------------------------\n");
	for (p = "Hello, world!\n"; *p; ++p)
		outb(PORT_PRINT_CHAR, *p);

	/* Test printVal() */
	display("----------------------------------------\n");
	printVal(1234);
	outb(PORT_PRINT_CHAR, '\n');

	/* Test getNumExits() */
	display("----------------------------------------\n");
	uint32_t numExits = getNumExits();
	printVal(numExits);
	outb(PORT_PRINT_CHAR, '\n');

	/* Test display() */
	display("----------------------------------------\n");
	printVal(getNumExits());
	display("\nHello world again!\n");
	printVal(getNumExits());
	display("\n");

	/* Test open() */
	display("----------------------------------------\n");
	display("Opening test.txt\n");
	fd = open("test.txt");
	printVal(fd);
	display("\n");

	/* Test read() */
	display("----------------------------------------\n");
	char buf[100];

	display("Reading test.txt - read returned ");
	printVal(read(fd, buf, sizeof(buf)));

	display("\nContents of test.txt:\n");
	display(buf);

	for(int i = 0; i < 100; i++) buf[i] = 0; // Clear buf

	/* Test multiple opening & reading */
	display("----------------------------------------\n");
	fd1 = open("a.txt");
	fd2 = open("b.txt");
	fd3 = open("c.txt");

	display("\nOpening a.txt, b.txt, c.txt\n");
	printVal(fd1);
	display("\n");
	printVal(fd2);
	display("\n");
	printVal(fd3);
	display("\n");

	read(fd1, buf, sizeof(buf));
	display("Contents of a.txt:\n");
	display(buf);
	for(int i = 0; i < 100; i++) buf[i] = 0;

	read(fd2, buf, sizeof(buf));
	display("Contents of b.txt:\n");
	display(buf);
	for(int i = 0; i < 100; i++) buf[i] = 0;

	read(fd3, buf, sizeof(buf));
	display("Contents of c.txt:\n");
	display(buf);
	for(int i = 0; i < 100; i++) buf[i] = 0;

	/* Test write() */
	display("----------------------------------------\n");
	display("Writing to test.txt - write returned ");
	printVal(write(fd, "Writing someting\n", 17));
	display("\n");

	display("Contents of test.txt:\n");
	// Reopen test.txt
	open("test.txt");
	fd4 = open("test.txt");
	read(fd4, buf, sizeof(buf));
	display(buf);
	for(int i = 0; i < 100; i++) buf[i] = 0;

	/* Halt after setting 0x400 & rax as 42 */
	*(long *) 0x400 = 42;
	for (;;)
		asm("hlt" : /* empty */ : "a" (42) : "memory");
}
