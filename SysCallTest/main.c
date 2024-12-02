#include <stdio.h>
#include <syscall.h>
#include <unistd.h>

int main(int argc, char** argv)
{
	int ret;

	ret = syscall(__NR_perf_event_open, NULL, 0, 0, 0, 0);
	printf("ret: %d\n", ret);

    return 0;
}
