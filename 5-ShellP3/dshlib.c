#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>

#include "dshlib.h"

void printCommandList(command_list_t *clist);
void printCommand(cmd_buff_t *cmd);
/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */
int exec_local_cmd_loop()
{
    char *cmd_buff = malloc(sizeof(char) * SH_CMD_MAX);
    int rc = 0;
    command_list_t cmds;
    bool shouldExit = false;

    while (!shouldExit) {
        printf("%s", SH_PROMPT);
        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }
        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';

        rc = build_cmd_list(cmd_buff, &cmds);

        switch (rc) {
            case OK:
                if (cmds.num == 0) {
                    printf(CMD_WARN_NO_CMD);
                } else if (cmds.num > 1) {
                    int null_fd = open("/dev/null", O_WRONLY);
                    if (null_fd == -1) {
                        perror("open");
                        exit(EXIT_FAILURE);
                    }
                    pid_t supervisor = fork();
                    if (supervisor == -1) {
                        perror("fork supervisor");
                        exit(EXIT_FAILURE);
                    }

                    if (supervisor == 0) {  // Supervisor process
                        execute_pipeline(&cmds);
                        dup2(null_fd, STDOUT_FILENO);
                        close(null_fd);
                        exit(EXIT_SUCCESS);
                    }

                    close(null_fd);
                    waitpid(supervisor, NULL, 0);
                } else {
                    Built_In_Cmds ret = exec_built_in_cmd(&cmds.commands[0]);
                    switch (ret) {
                        case BI_EXECUTED:
                            break;
                        case BI_CMD_EXIT:
                            printf("exiting...\n");
                            shouldExit = true;
                            break;
                        case BI_NOT_BI:
                            exec_cmd(&cmds.commands[0]);
                            break;
                        default:
                            printf("SHOULD NOT REACH HERE\n");
                            break;
                    }
                }
                free_cmd_list(&cmds);
                break;
            case ERR_TOO_MANY_COMMANDS:
                printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
                break;
            case ERR_CMD_OR_ARGS_TOO_BIG:
                printf("error: executable name or arguments are too long\n");
                break;
            default:
                printf("SHOULD NOT REACH HERE\n");
                break;
        }
    }

    free(cmd_buff);
    return OK;
}

int alloc_cmd_buff(cmd_buff_t *cmd_buff) {
    cmd_buff->_cmd_buffer = malloc(sizeof(char) * SH_CMD_MAX);

    if (cmd_buff->_cmd_buffer) {
        return OK;
    }
    return ERR_MEMORY;
}

int free_cmd_buff(cmd_buff_t *cmd_buff) {
    free(cmd_buff->_cmd_buffer);
    return OK;
}

int clear_cmd_buff(cmd_buff_t *cmd_buff) {
    cmd_buff->argc = 0;
    for (int i = 0; i < CMD_ARGV_MAX; i++) {
        cmd_buff->argv[i] = NULL;
    }
    for (int i = 0; i < SH_CMD_MAX; i++) {
        cmd_buff->_cmd_buffer[i] = '\0';
    }
    return OK;
}

//int close_cmd_buff(cmd_buff_t *cmd_buff) {
//}

int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff) {
    int argCount = 0;
    int cmdLen = 0;
    int start = 0;
    bool spaceLast = true;
    bool includeSpace = false;

    while (*cmd_line) {
        if (*cmd_line == SPACE_CHAR && !includeSpace) {
            if (!spaceLast) {
                cmd_buff->_cmd_buffer[cmdLen] = '\0';
                cmd_buff->argv[argCount] = cmd_buff->_cmd_buffer + start;
                cmdLen++;
                argCount++;
                start = cmdLen;
            }
            spaceLast = true;
        } else {
            if (cmdLen >= SH_CMD_MAX - 1) {
                return ERR_CMD_OR_ARGS_TOO_BIG;
            }
            if (*cmd_line == '\"') {
                if (spaceLast && !includeSpace) start++;
                includeSpace = !includeSpace;
            } else {
                cmd_buff->_cmd_buffer[cmdLen] = *cmd_line;
            }
            spaceLast = false;
            cmdLen++;
        }
        cmd_line++;
    }
    if (!spaceLast) {
        cmd_buff->_cmd_buffer[cmdLen] = '\0';
        cmd_buff->argv[argCount] = cmd_buff->_cmd_buffer + start;
        argCount++;
        cmd_buff->argc = argCount;
    }
    return OK;
}

int build_cmd_list(char *cmd_line, command_list_t *clist) {
    int cmdCount = 0;
    int rc;

    char *tok = strtok(cmd_line, PIPE_STRING);
    while (tok) {
        if (cmdCount == CMD_MAX) {
            return ERR_TOO_MANY_COMMANDS;
        }
        alloc_cmd_buff(&clist->commands[cmdCount]);
        clear_cmd_buff(&clist->commands[cmdCount]);
        rc = build_cmd_buff(tok, &clist->commands[cmdCount]);
        if (rc == ERR_CMD_OR_ARGS_TOO_BIG) {
            return rc;
        }
        cmdCount++;
        tok = strtok(NULL, PIPE_STRING);
    }
    clist->num = cmdCount;

    return OK;
}

int free_cmd_list(command_list_t *cmd_lst) {
    for (int i = 0; i < cmd_lst->num; i++) {
        free_cmd_buff(&cmd_lst->commands[i]);
    }
    return OK;
}

Built_In_Cmds match_command(const char *input) {
    if (strcmp(input, EXIT_CMD) == 0) {
        return BI_CMD_EXIT;
    } else if (strcmp(input, "dragon") == 0) {
        return BI_CMD_DRAGON;
    } else if (strcmp(input, "cd") == 0) {
        return BI_CMD_CD;
    } else if (strcmp(input, "") == 0) {
        return BI_EXECUTED;
    } else {
        return BI_NOT_BI;
    }
}

Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd) {
    Built_In_Cmds bi_cmd = match_command(cmd->argv[0]);
    switch (bi_cmd) {
        case BI_CMD_EXIT:
            return BI_CMD_EXIT;
        case BI_CMD_DRAGON:
            printf("Sorry no dragon\n");
            return BI_EXECUTED;
        case BI_CMD_CD:
            if (cmd->argc > 1) {
                chdir(cmd->argv[1]);
            }
            return BI_EXECUTED;
        case BI_NOT_BI:
            return BI_NOT_BI;
        case BI_EXECUTED:
            printf(CMD_WARN_NO_CMD);
            return BI_EXECUTED;
        default:
            printf("Error executing built in command\n");
            exit(ERR_EXEC_CMD);
    }
}

int exec_cmd(cmd_buff_t *cmd) {
    int f_result, c_result;
    int inFD = -2, outFD = -2;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;

    for (int i = 0; i < cmd->argc; i++) {
        if (strcmp(cmd->argv[i], "<") == 0) {
            if (i + 1 == cmd->argc || !cmd->argv[i + 1]) {
                printf("error: No file name provided for redirection\n");
                return ERR_CMD_ARGS_BAD;
            }
            cmd->argv[i] = NULL;
            inFD = open(cmd->argv[i + 1], O_RDONLY, mode);
        } else if (strcmp(cmd->argv[i], ">") == 0) {
            if (i + 1 == cmd->argc || !cmd->argv[i + 1]) {
                printf("error: No file name provided for redirection\n");
                return ERR_CMD_ARGS_BAD;
            }
            cmd->argv[i] = NULL;
            outFD = open(cmd->argv[i + 1], O_WRONLY | O_CREAT, mode);
        } else if (strcmp(cmd->argv[i], ">>") == 0) {
            if (i + 1 == cmd->argc || !cmd->argv[i + 1]) {
                printf("error: No file name provided for redirection\n");
                return ERR_CMD_ARGS_BAD;
            }
            cmd->argv[i] = NULL;
            outFD = open(cmd->argv[i + 1], O_WRONLY | O_CREAT | O_APPEND, mode);
        }
    }

    if (inFD == -1) {
        printf("error: opening file for redirection\n");
        return ERR_EXEC_CMD;
    }

    if (outFD == -1) {
        printf("error: opening file for redirection\n");
        return ERR_EXEC_CMD;
    }

    f_result = fork();
    if (f_result < 0) {
        perror("fork error");
        exit(ERR_EXEC_CMD);
    }

    if (f_result == 0) {
        if (inFD != -2) {
            dup2(inFD, STDIN_FILENO);
            close(inFD);
        }
        if (outFD != -2) {
            dup2(outFD, STDOUT_FILENO);
            close(outFD);
        }
        int rc = execvp(cmd->argv[0], cmd->argv);
        if (rc < 0) {
            int err = errno;
            switch (err) {
                case EPERM:
                    printf("Operation not permitted\n");
                    break;
                case ENOENT:
                    printf("Command not found\n");
                    break;
                case EIO:
                    printf("Error reading file");
                    break;
                case EACCES:
                    printf("Access to command denied\n");
                    break;
                case EBADF:
                    printf("Bad file number for command\n");
                    break;
                case ENFILE:
                    printf("File table full\n");
                    break;
                case EMFILE:
                    printf("Too many files open to load command\n");
                    break;
                case EFBIG:
                    printf("Command file too large\n");
                    break;
                default:
                    printf("Unrecognized error: %d\n", err);
                    break;
            } 
            exit(err);
        }
        return 0;
    } 
    if (inFD != -2) {
        close(inFD);
    }
    if (outFD != -2) {
        close(outFD);
    }
    int rc = wait(&c_result);
    if (rc < f_result) {
        perror("fork error");
        exit(ERR_EXEC_CMD);
    }
    return WEXITSTATUS(c_result);
}

int execute_pipeline(command_list_t *clist) {
    int pipes[clist->num - 1][2];  // Array of pipes
    pid_t pids[clist->num];        // Array to store process IDs

    // Create all necessary pipes
    for (int i = 0; i < clist->num - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    // Create processes for each command
    for (int i = 0; i < clist->num; i++) {
        pids[i] = fork();
        if (pids[i] == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pids[i] == 0) {  // Child process
            // Set up input pipe for all except first process
            if (i > 0) {
                dup2(pipes[i-1][0], STDIN_FILENO);
            }

            // Set up output pipe for all except last process
            if (i < clist->num - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }

            // Close all pipe ends in child
            for (int j = 0; j < clist->num - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            // Execute command
            execvp(clist->commands[i].argv[0], clist->commands[i].argv);
            perror("execvp");
            exit(EXIT_FAILURE);
        }
    }

    // Parent process: close all pipe ends
    for (int i = 0; i < clist->num - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    // Wait for all children
    for (int i = 0; i < clist->num; i++) {
        waitpid(pids[i], NULL, 0);
    }

    return OK;
}

void printCommandList(command_list_t *clist) {
    for (int i = 0; i < clist->num; i++) {
        printCommand(&clist->commands[i]);
    }
}

void printCommand(cmd_buff_t *cmd) {
    for (int i = 0; i < cmd->argc; i++) {
        printf("%s ", cmd->argv[i]);
    }
    printf("\n");
}
