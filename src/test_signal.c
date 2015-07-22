//============================================================================
// Name        : test_signals.cpp
// Author      : TheRockLiuHY
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/mman.h>

#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/mman.h>
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define handle_error(msg) \
          do { perror(msg); exit(EXIT_FAILURE); } while (0)

static char *buffer;

void handler(int sig, siginfo_t *si, void *unused)
{
	printf("thread %ld  Got SIGSEGV at address: 0x%lx\n", pthread_self(),
			(long) si->si_addr);

	int j, nptrs;
#define SIZE 100
	void *buffer[100];
	char **strings;

	nptrs = backtrace(buffer, SIZE);
	printf("backtrace() returned %d addresses\n", nptrs);

	/* The call backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO)
	 would produce similar output to the following: */

	strings = backtrace_symbols(buffer, nptrs);
	if (strings != NULL)
	{
		for (j = 0; j < nptrs; j++)
			printf("%s\n", strings[j]);
		free(strings);
	}

	printf("thread exit...\n");

	pthread_exit(NULL);
}

void* thread_fun(void* x)
{

	sleep(5);

	char *p;
	int pagesize;
	struct sigaction sa;

	sa.sa_flags = SA_SIGINFO;
	sigemptyset(&sa.sa_mask);
	sa.sa_sigaction = handler;
	if (sigaction(SIGSEGV, &sa, NULL) == -1)
		handle_error("sigaction");

	pagesize = sysconf(_SC_PAGE_SIZE);
	if (pagesize == -1)
		handle_error("sysconf");

	/* Allocate a buffer aligned on a page boundary;
	 initial protection is PROT_READ | PROT_WRITE */
	buffer = (char*) memalign(pagesize, 4 * pagesize);
	if (buffer == NULL)
		handle_error("memalign");

	printf("Start of region:        0x%lx\n", (long) buffer);

	if (mprotect(buffer + pagesize * 2, pagesize,
	PROT_READ) == -1)
		handle_error("mprotect");

	for (p = buffer;;)
		*(p++) = 'a';

	printf("thread %ld is out...\n", pthread_self());

}

int main(int argc, char *argv[])
{

	for (auto x = 0; x < 10; x++)
	{
		pthread_t y;
		pthread_create(&y, NULL, thread_fun, NULL);
//		std::thread y(thread_fun);
//		y.detach();
	}

	sleep(10);
	printf("Loop completed\n"); /* Should never happen */
	exit(EXIT_SUCCESS);
}

