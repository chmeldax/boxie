# boxie
Play with containers solutions in Linux

# Rationale

Personal project to help me understand namespaces and cgroups in Linux kernel.

# Implemented solution

* `main.cpp` uses `clone` system call to create a new process within new namespaces
* `container.cpp` handles container and acts a bit like `init` process. It uses forking to start a new tasks

