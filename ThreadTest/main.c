#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <sys/types.h>

void* test_thread_run(void* targv)
{
	printf("enter %s - %p, pid = %d, tid = %d\n",
		__func__, &test_thread_run, getpid(), gettid());

	getchar();
}

int main(void)
{
	int ret;
	pthread_t my_th;
	pid_t fkpid;

	printf("enter %s, pid = %d, tid = %d\n",
		__func__, getpid(), gettid());

	ret = pthread_create(&my_th, NULL, test_thread_run, NULL);
	if (ret) {
		printf("thread create failed, errno: %d\n", ret);
	}

	fkpid = fork();
	if (fkpid == 0) {
		printf("child process pid = %d\n", getpid());
		exit(0);
	}

	printf("waiting thread\n");
	pthread_join(my_th, NULL);
	printf("%s will exit\n", __func__);

	return 0;
}
