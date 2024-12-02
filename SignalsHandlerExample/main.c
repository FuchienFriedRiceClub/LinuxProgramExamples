#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <setjmp.h>
#include <signal.h>
#include <ucontext.h>

typedef void (*signal_handle_func)(int, siginfo_t*, void*);

#define SIGNAL_REGISTER_FAILED		((signal_handle_func)-1)
#define SETJMP_RET_VAL_1			2333
#define KRNL_UCNTXT_ELE_CNT			5

typedef struct my_signal_info {
	unsigned long sig_num;
	signal_handle_func handle_func;
} my_siginfo;

static void my_signal_handle(int, siginfo_t*, void*);

static my_siginfo my_si[] = {
	{
		.sig_num = SIGKILL,
		.handle_func = my_signal_handle,
	},
	{
		.sig_num = SIGTERM,
		.handle_func = my_signal_handle,
	},
};
static int ret_num = 0;
static jmp_buf test_jmp_context;

static void my_atexit_func(void)
{
	printf("enter %s, program will exit\n", __func__);
}

static void my_atexit_register(void (*func)(void))
{
	int ret;

	ret = atexit(func);
	if (ret != 0) {
		printf("register atexit function failed\n");

		exit(ret);
	}
}

static void siginfo_dump(siginfo_t* si)
{
	if (si) {
		printf(
			"\n[**] signinfo (signinfo_t size 0x%llx) - (_sifields size 0x%llx):\n"
			"si_signo = %08d ; si_errno = %08d ; si_code = %08d ;\n"
			"si_pid   = %08d ; si_uid   = %08d ;\n"
			"[--] _sifields will be displayed differently depending on the signal\n"
			"[--] only pid and uid will be shown here\n",
			sizeof(*si), sizeof(si->_sifields),
			si->si_signo, si->si_errno, si->si_code,
			si->_sifields._pad[0], si->_sifields._pad[1]
		);
	}
}

static void libc_fpstate_dump(fpregset_t fpregs)
{
	printf(
		"\tcwd = %d ; swd = %d ; ftw = %d ; fop = %d ;\n"
		"\trip = 0x%016lx ; rdp = 0x%016lx ;\n"
		"\tmxcsr = 0x%08x ; mxcr_mask = 0x%08x ;\n"
		"\tno [_st] [_xmm]\n",
		fpregs->cwd,
		fpregs->swd,
		fpregs->ftw,
		fpregs->fop,
		fpregs->rip,
		fpregs->rdp,
		fpregs->mxcsr,
		fpregs->mxcr_mask
	);
}

static void ucontext_dump(ucontext_t* ucontext)
{
	ssize_t arr_size, ele_size, ele_cnt;
	int i;

	if (ucontext) {
		printf(
			"\n[**] ucontext (ucontext_t size 0x%llx):\n"
			"uc_flags = 0x%016lx ; uc_link = 0x%016lx ;\n"
			"uc_stack (stack_t size 0x%llx) ->\n"
			"\tss_sp = 0x%016lx ; ss_flags = 0x%016lx ; ss_size = 0x%016lx\n"
			"uc_mcontext (mcontext_t size 0x%llx) ->\n"
			"\t---- gregs start (sizeof 0x%llx)----",
			sizeof(*ucontext),
			ucontext->uc_flags, (unsigned long)ucontext->uc_link, sizeof(ucontext->uc_stack),
			ucontext->uc_stack.ss_sp, ucontext->uc_stack.ss_flags, ucontext->uc_stack.ss_size,
			sizeof(ucontext->uc_mcontext), sizeof(ucontext->uc_mcontext.gregs)
		);

		i = 0;
		while (i < __NGREG) {
			if ((i % 4) == 0) {
				printf("\n\t");
			}

			printf(
				"0x%016lx ; ", ucontext->uc_mcontext.gregs[i]
			);

			i++;
		}

		printf(
			"\n\t---- gregs end ----\n"
			"\t---- fpregs start (size 0x%llx)-----\n",
			sizeof(ucontext->uc_mcontext.fpregs)
		);
		libc_fpstate_dump(ucontext->uc_mcontext.fpregs);
		printf("\t---- fpregs end ----\n");

		printf(
			"no uc_sigmask (sigset_t size 0x%llx)\n"
			"__fpregs_mem (_libc_fpstate size 0x%llx) ->\n",
			sizeof(ucontext->uc_sigmask), sizeof(ucontext->__fpregs_mem)
		);
		libc_fpstate_dump(&ucontext->__fpregs_mem);

		i = 0;
		arr_size = sizeof(ucontext->__ssp);
		ele_size = sizeof(unsigned long long);
		ele_cnt = arr_size / ele_size;
		printf(
			"__ssp (array size 0x%llx) ->\n\t",
			arr_size
		);
		while (i < 4) {
			printf("0x%016llx ; ", ucontext->__ssp[i]);

			i++;
		}
		printf("\n");
	}
}

static void my_signal_handle(int signum, siginfo_t* si, void* ucontext)
{
	printf(
		"\n[**] receive signal, signal base info:\n"
		"signal num    = %d \n"
		"signal info   = 0x%016lx\n"
		"user context  = 0x%016lx\n",
		signum, (unsigned long)si, (unsigned long)ucontext
	);

	siginfo_dump(si);
	ucontext_dump(ucontext);
}

static signal_handle_func my_customize_signal_register_process(my_siginfo* msi)
{
	int ret;
	struct sigaction new_act, old_act;

	memset(&new_act, 0, sizeof(struct sigaction));

	sigemptyset(&new_act.sa_mask);
	new_act.sa_flags = SA_SIGINFO;
#ifdef  SA_RESTART
	new_act.sa_flags |= SA_RESTART;
#endif
	new_act.sa_sigaction = msi->handle_func;

	ret = sigaction(msi->sig_num, &new_act, &old_act);
	if (ret != 0) {
		return SIGNAL_REGISTER_FAILED;
	}

	return old_act.sa_sigaction;
}

static void my_signal_register(void)
{
	signal_handle_func tmp_func;
	size_t arry_size, ele_size, ele_cnt;

	arry_size = sizeof(my_si);
	ele_size = sizeof(my_siginfo);
	ele_cnt = arry_size / ele_size;

	do {
		tmp_func = my_customize_signal_register_process(&my_si[ele_cnt - 1]);
		if (tmp_func == SIGNAL_REGISTER_FAILED) {
			printf("cannot register signo %d, errno %d\n", my_si[ele_cnt - 1].sig_num, errno);
		}
		else {
			printf("register signo %d succeed\n", my_si[ele_cnt - 1].sig_num);
		}
	} while(--ele_cnt);
}

static void my_signal_setting(void)
{
	my_atexit_register(my_atexit_func);

	my_signal_register();
}

static void setting4globaljmp(void)
{
	printf("enter %s\n", __func__);

	longjmp(test_jmp_context, SETJMP_RET_VAL_1);

	printf("leave %s\n", __func__);
}

static void global_jmp_test(void)
{
	int cur_ret_val;

	cur_ret_val = setjmp(test_jmp_context);
	printf("num %d -> setjmp return: %d\n", ret_num, cur_ret_val);
	ret_num++;

	if (cur_ret_val == 0) {
		setting4globaljmp();
	}
}

int main(void)
{
	my_signal_setting();
	global_jmp_test();

	printf("pid = %d, waiting for a signal\n", getpid());
	pause();
}
