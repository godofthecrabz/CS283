1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**:  `execvp` replaces the current process image with the process being opened. To make sure that the current process doesn't get replaced a `fork` is needed first to make a clone of the current process then one of the clones can `execvp`.

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  If the fork system call fails the process won't be split and fork will return -1. My implementation checks if fork returns -1 and if it does it prints an error message to the screen and terminates the shell process.

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**:  If execvp is not given a file path it checks the PATH environment variable for the provided command.

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**:  The purpose of calling wait in the parent process is to force the parent to wait for the child process to finish before continuing execution. If wait was not called the parent process would continue running at the same time as the child possible messing with output to the console or worse.

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**:  WEXITSTATUS extracts the errno code from the child process after it finished executing. The errno code provides information about how the child process exitted either normally or abnormally.

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**:  My implementation toggles between including and excluding spaces when parsing the command string. The build_cmd_buff main loop checks for spaces and separates arguments by the spaces except if the include spaces toggle is on. In that case all characters are added to the current argument until another double quote is encountered. This is necessary because there has to be some way to indicate that a quoted section of the command line input has been entered and a toggle is the simplest solution. Only two checks are needed: one to check for the double quote and one to check if the current character is within a quoted argument before parsing for the delimiters.

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**:  The two major changes I had to make for parsing were to change the parsing to use the enum return codes provided by the dshlib.h header and handling the logic for the quoted arguments. The hardest part of refactoring my old code was separating the larger chunks of logic into separate functions.

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**:  The purpose of signals is to allow for processes/threads to communicate with the operating system. Signals differ from other forms of interprocess communication because the communications between the process and the operating system are of a predefined set that all have specific meanings.

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**:  SIGINT notifies a process that it needs to suspend execution and prepare for termination. SIGINT is commonly used for terminating a process that fails to terminate properly. SIGKILL notifies the operating system to terminate a process. SIGKILL is used to quickly terminate an erroneous or unresponsive process. SIGTERM notifies the operating system to terminate a process. Unlike SIGKILL, SIGTERM gives the terminating process time to properly clean itself up while SIGKILL immiediately terminates the program.

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**: SIGSTOP tells a process to stop executing until it receives a SIGCONT signal to resume execution. SIGSTOP cannot be caught or ignored because the operating system needs some way to stop or suspend an unresponsive program without it being ignored. SIGSTOP and SIGKILL both are meant with the purpose of allowing suspension or termination of a program no matter what.
