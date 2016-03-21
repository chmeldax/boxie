#include <iostream>
#include <sched.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <string>

int run(void* arg)
{
	const std::string name = "funny-name";
	struct utsname uts;
	sethostname((const char *) name.c_str(), name.size());
	
	std::cout << "This is child" << std::endl;
	uname(&uts);
	std::cout << "This is a child's nodename: " << uts.nodename << std::endl;
	
	return 0;
}

int main()
{
	struct utsname uts;
	
	char *stack = (char*) malloc(1024*1024);
	std::cout << "This is parent" << std::endl;
	pid_t pid = clone(run, (stack + 1024*1024), CLONE_NEWUTS | SIGCHLD, NULL);
	if (pid == -1)
	{
		std::cout << "Creating new namespace failed. Error number: " << errno;
	}
	
	sleep(1);
	
	uname(&uts);
	std::cout << "This is a parent's nodename: " << uts.nodename << std::endl;
	waitpid(pid, NULL, 0);
}
