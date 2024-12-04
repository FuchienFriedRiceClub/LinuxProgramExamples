#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define STR_MAX_LEN		0x100

int main(void)
{
	int pfd[2];
	pid_t child_pid;
	char data[STR_MAX_LEN] = "pipe message\0";
	char buf[STR_MAX_LEN];

	pipe(pfd);
	child_pid = fork();
	if (child_pid < 0) {
		printf("fork error\n");

		exit(1);
	}

	if (child_pid == 0) {
		close(pfd[1]);
		read(pfd[0], buf, STR_MAX_LEN);
		printf("recevie: %s\n", buf);
		exit(0);
	}
	else {
		close(pfd[0]);
		write(pfd[1], data, strnlen(data, STR_MAX_LEN)); 
	}

	return 0;
}
