#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <sys/wait.h>
#include "container.h"

void Container::run_command(const std::string& command, const std::vector<std::string>& args)
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
