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

class Container
{
public:
    class Builder;
    void run_command(std::string command, std::vector<std::string> args);
    
private:
    Container(std::string hostname) : hostname(hostname) {};
    std::string hostname;
    void change_hostname(std::string hostname);
};

class Container::Builder
{
public:
    Builder& set_hostname(const std::string& hostname) { this->hostname = hostname; return *this; } 
    Container build() { return Container(hostname); }
    
private:
    std::string hostname;
};

void Container::run_command(std::string command, std::vector<std::string> args)
{
    std::vector<char*> args_char;
    try {
        std::transform(args.begin(), args.end(), std::back_inserter(args_char), [](const std::string & s) -> char* { 
            char *pc = new char[s.size() + 1];
            std::strcpy(pc, s.c_str());
            return pc;
            }
        );
    }
    catch (std::exception const &exc)
    {
        std::cerr << "Exception caught " << exc.what() << "\n";
    }

    pid_t pid = fork();
    if (pid == 0)
    {
        std::cout << "About to spin a new task" << std::endl;
        std::cout << command << std::endl;
        execvp((const char*) command.c_str(), &args_char[0]);
    }
    else if (pid > 0)
    {
        // parent process
        waitpid(pid, NULL, 0);
    }
    else
    {
        std::cout << "Forking failed" << std::endl;
    }
}

void Container::change_hostname(std::string hostname)
{
    sethostname((const char *) hostname.c_str(), hostname.size());
}


int run(void* arg)
{
    const std::string name = "funny-name";
    char **argv = (char**) arg;
    
    Container::Builder builder;
    builder.set_hostname(name);
    Container container = builder.build();
    
    std::vector<std::string> args(1);
    try 
    {
        container.run_command((std::string) argv[1], args);
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
