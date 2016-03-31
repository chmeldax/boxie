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
#include <algorithm>
#include "container.h"

int run(void* arg)
{
    const std::string name = "funny-name";
    char **argv = (char**) arg;
    
    Container::Builder builder;
    builder.set_hostname(name);
    Container container = builder.build();
    
    const std::vector<std::string> args(1);
    try 
    {
        container.run_command((const std::string) argv[1], args);
    }
    catch (std::exception const &exc)
    {
        std::cerr << "Exception caught " << exc.what() << "\n";
    }
    
    struct utsname uts;
    std::cout << "This is child" << std::endl;
    uname(&uts);
    std::cout << "This is a child's nodename: " << uts.nodename << std::endl;
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
