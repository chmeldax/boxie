#ifndef CONTAINER_H
#define CONTAINER_H

class Container
{
public:
    class Builder;
    void run_command(const std::string command, const std::vector<std::string> args);
    
private:
    Container(std::string hostname);
    std::string hostname;
    std::string netnsname;
    void change_hostname();
    void set_network();
};

class Container::Builder
{
public:
    Builder& set_hostname(const std::string hostname) { this->hostname = hostname; return *this; }
    Container build() { return Container(hostname); }
    
private:
    std::string hostname;
};

#endif /* CONTAINER_H */
