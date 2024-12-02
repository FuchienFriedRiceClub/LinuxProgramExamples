#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <bits/mman-linux.h>

static int vuln(char* data)
{
	char cmd[0x100];

	strncpy(cmd, data, 0x100);
	system(cmd);
}

int main(void)
{
	char* msg = "/bin/sh\0";

	mmap(0, 0x1000, PROT_READ  |PROT_WRITE | PROT_EXEC,
		MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	perror("mmap");
	memcpy(0, msg, sizeof(msg));
	
	msg = NULL;
	vuln(msg);
}
