1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

    > **Answer**:  My shell uses a supervisor fork that forks all the child processes for the piped commands and uses waitpit() to wait for each child process. The main shell processes then waitpit() the supervisor process. If the supervisor process forgot to waitpid() all the child processes the shell would continue executing at the same time as a child process and intermingling output between the processes.

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

    > **Answer**:  The dup2() replaces the second file descriptor with the first. The reference count for the second file descriptor will be decremented and the reference count for the first will be incremented. If close is not called on an unused pipe after the child processes finish the reference count for the pipe file descriptors will never reach zero and the operating system will retain the memory backing the pipe file descriptors. If the pipes are left open the program could cause a memory leak and eventually run out of memory as it keeps opening new pipes and never closes them.

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

    > **Answer**:  cd is implemented as an internal command because it is meant to change the current directory that the shell is in. If cd were a seperate program it's current directory could be different than the shell and it would be difficult to find a way for a seperate program to change the directory of the shell.

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

    > **Answer**:  The shell could read input from stdin one char at a time instead of using fgets and put the chars that are read into buffers of a pre-set chunk size. If a buffer is filled a new one would be created and input would be put in the newly created buffer until it is filled. This would repeat until the newline character is read then all the buffer chunks would be combined into one large buffer to be parsed and prepared for execution. This algorithm would need to consider what buffer chunk size would be most efficient to minimize allocation of buffer chunks and minimize wasted memory from sparsely populated buffer chunks.
