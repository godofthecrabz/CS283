1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**:  Since, `fgets()` stops reading input after reading a new line character it allows us to read each line that the user enters.

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**:  When the shell passes the arguments string array to a newly created application it will reuse the same input string provided to it. If the string was a fized-size array a program launching the shell might see their arguments change if the shell begins reading a new line while the program executes. By using `malloc()`, we can ensure there exists one argument string for each program launched from the shell.


3. In `dshlib.c`, the function `build_cmd_list()` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**:  The commands are meant to be used as the names and arguments for another program for the shell to execute. If the leading and trailing spaces were included it would make it much hard to pass the command string to another program as arguements. If the spaces weren't removed it could make it impossible to execute the program, because the program name would have the leading spaces, and the last argument would have the garbage spaces at the end that could mess up the program behavior.

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**:  The Drexel Shell should implement redirection of STDIN, STDOUT, and STDERR. The challenges that come from redirection of standard input and output are the redirecting of output from a program to replace STDIN and the redirecting of the program output to another program. Without some way to directly send data directly between programs, pipes, temporary files would have to be created for passing the information between the two programs.

- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**:  The key difference between redirection and piping is that redirection passes data to files and streams while piping passes data to other programs. 

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**:  Since STDERR is a seperate output stream a programmer can ensure their is a safe place for debugging and error output to go in case the STDOUT is redirected to another file or stream. 

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**:  The Drexel Shell should print out any output from the failed command to the shell. The shell should merge the output from STDOUT and STDERR either by coloring the output from the two streams differently, i.e. color STDOUT white and STDERR red, or labelling each line with which output stream it came from.