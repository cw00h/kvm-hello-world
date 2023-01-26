#define PORT_PRINT_CHAR  0xE9
#define PORT_PRINT_VALUE 0xEA
#define PORT_GETNUMEXITS 0xEB
#define PORT_DISPLAY     0xEC
#define PORT_OPEN        0xED
#define PORT_READ        0xEE
#define PORT_WRITE       0xEF

#define MAX_FD 256

struct rw_args{
	int fd;
	void *buf;
	size_t count;
};
