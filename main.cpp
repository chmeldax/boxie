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

#define STACK_SIZE (1024 * 1024)

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
        return 1;
    }
    
    return 0;
}


int main(int argc, char *argv[])
{
    char *stack = (char*) malloc(STACK_SIZE);
    
    int flags = CLONE_NEWUTS | CLONE_NEWIPC | CLONE_NEWNET | CLONE_NEWNS | CLONE_NEWPID | CLONE_NEWUSER | 
    SIGCHLD;
    
    pid_t pid = clone(run, (stack + STACK_SIZE), flags, argv);
    if (pid == -1)
    {
        std::cerr << "Creating new namespace failed. Error number: " << errno;
        return 1;
    }
    
    waitpid(pid, NULL, 0);
    std::cout << "Removing container." << std::endl;
    return 0;
}
