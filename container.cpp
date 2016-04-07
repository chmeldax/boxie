#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mount.h>
#include "container.h"

Container::Container(std::string hostname): hostname(hostname)
{
    netnsname = "ns-name";
    change_hostname();
    set_network();
}

void Container::run_command(const std::string command, const std::vector<std::string> args)
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

    args_char.push_back((char *) NULL); // needed for execvp
    
    pid_t pid = fork();
    if (pid == 0)
    {
        std::cout << "Running command `" << command << "` in container..." << std::endl;
        execvp((const char*) command.c_str(), &args_char[0]);
        exit(1);
    }
    else if (pid > 0)
    {
        waitpid(pid, NULL, 0);
    }
    else
    {
        std::cerr << "Forking failed" << std::endl;
    }
}

void Container::change_hostname()
{
    sethostname((const char *) hostname.c_str(), hostname.size());
}

void Container::set_network()
{
    char ns_path[100];
    snprintf(ns_path, 100, "%s/%s", "/var/run/netns", netnsname.c_str());
    close(open(ns_path, O_RDONLY|O_CREAT|O_EXCL, 0));
    mount("/proc/self/ns/net", ns_path, "none", MS_BIND, NULL);
    
    run_command("ip", {"ip", "link", "set", "dev",  "lo", "up"});
    run_command("ping", {"ping", "-c3", "127.0.0.1"});
}
