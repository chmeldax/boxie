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
    char **argv = (char**) arg;
	const std::string name = "funny-name";
	struct utsname uts;
	sethostname((const char *) name.c_str(), name.size());
	
	std::cout << "This is child" << std::endl;
	uname(&uts);
	std::cout << "This is a child's nodename: " << uts.nodename << std::endl;
	
	pid_t pid = fork();
    if (pid == 0)
    {
		std::cout << "About to spin a new task" << std::endl;
		std::cout << argv[1] << std::endl;
		execvp(argv[1], &argv[1]);
    }
    else if (pid > 0)
    {
        // parent process
		waitpid(pid, NULL, 0);
    }
    else
    {
		std::cout << "Forking failed" << std::endl;
        return 1;
    }
	
	return 0;
}


int main(int argc, char *argv[])
{
	struct utsname uts;
	
	char *stack = (char*) malloc(1024*1024);
	std::cout << "This is parent" << std::endl;
	pid_t pid = clone(run, (stack + 1024*1024), CLONE_NEWUTS | SIGCHLD, argv);
	if (pid == -1)
	{
		std::cout << "Creating new namespace failed. Error number: " << errno;
	}
	
	sleep(1);
	
	uname(&uts);
	std::cout << "This is a parent's nodename: " << uts.nodename << std::endl;
	waitpid(pid, NULL, 0);
}
