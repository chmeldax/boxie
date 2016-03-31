#include <iostream>
#include <sched.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <string>
#include <cstring>
#include <vector>
#include "container.h"

Container build_container()
{
    const std::string name = "funny-name";
    Container::Builder builder;
    builder.set_hostname(name);
    return builder.build();
}

int run(void* arg)
{
    char **argv = (char**) arg;
    const std::vector<std::string> args(1);
    
    Container container = build_container();
    try 
    {
        container.run_command((const std::string) argv[1], args);
    }
    catch (const std::exception& exc)
    {
        std::cerr << "Exception caught " << exc.what() << "\n";
        exit(1);
    }
}


int main(int argc, char *argv[])
{
    struct utsname uts;
    char *stack = (char*) malloc(1024*1024);
    
    pid_t pid = clone(run, (stack + 1024*1024), CLONE_NEWUTS | SIGCHLD, argv);
    if (pid == -1)
    {
        std::cerr << "Creating new namespace failed. Error number: " << errno;
        exit(1);
    }
    
    sleep(1);
    
    uname(&uts);
    std::cout << "This is a parent's nodename: " << uts.nodename << std::endl;
    waitpid(pid, NULL, 0);
}
