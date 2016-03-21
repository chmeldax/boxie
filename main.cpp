#include<iostream>
#include<sched.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<errno.h>
#include<unistd.h>

int run(void* arg)
{
	sleep(1);
	std::cout << "This is child" << std::endl;
	return 0;
}

int main()
{
	char *stack = (char*) malloc(1024*1024);
	std::cout << "This is parent" << std::endl;
	pid_t pid = clone(run, (stack + 1024*1024), CLONE_NEWNS | SIGCHLD, NULL);
	if (pid == -1)
	{
		std::cout << "Creating new namespace failed. Error number: " << errno;
	}
	sleep(1);
	waitpid(pid, NULL, 0);
}
