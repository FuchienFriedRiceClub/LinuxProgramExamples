#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define STR_MAX_LEN		0x100

int add(int a, int b)
{
	return (a + b);
}

int main(void)
{
	int pfd[2];
	pid_t child_pid;
	char buf[STR_MAX_LEN];

	pipe(pfd);
	child_pid = fork();
	if (child_pid < 0) {
		printf("fork error\n");

		exit(-1);
	}

	if (child_pid == 0) {
		printf("child: will send message\n");
		read(pfd[0], buf, STR_MAX_LEN);
		printf("chlid: recevie = %s\n", buf);

		printf("child: will write message\n");
		write(pfd[1], "meaasge from child", 19);
	}
	else {
		printf("parent: will send message\n");
		write(pfd[1], "meaasge from pargent", 21); 
		sleep(1);

		printf("parent: will read message\n");
		read(pfd[0], buf, STR_MAX_LEN);
		printf("parent: recevie = %s\n", buf);
	}

	return 0;
}
