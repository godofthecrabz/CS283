#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dshlib.h"


extern void print_dragon();

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
    // TODO IMPLEMENT MAIN LOOP

    // TODO IMPLEMENT parsing input to cmd_buff_t *cmd_buff

    // TODO IMPLEMENT if built-in command, execute builtin logic for exit, cd (extra credit: dragon)
    // the cd command should chdir to the provided directory; if no directory is provided, do nothing

    // TODO IMPLEMENT if not built-in command, fork/exec as an external command
    // for example, if the user input is "ls -l", you would fork/exec the command "ls" with the arg "-l"

    char *cmd_buff = malloc(sizeof(char) * SH_CMD_MAX);
    int rc = 0;
    int forkRet = 0;
    cmd_buff_t cmd;
    bool shouldExit = false;
    alloc_cmd_buff(&cmd);

    while (!shouldExit) {
        printf("%s", SH_PROMPT);
        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL) {
            printf("\n");
            continue;
        }
        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';

        clear_cmd_buff(&cmd);
        rc = build_cmd_buff(cmd_buff, &cmd);

        switch (rc) {
            case OK:
                Built_In_Cmds ret = exec_built_in_cmd(&cmd);
                switch (ret) {
                    case BI_EXECUTED:
                        break;
                    case BI_CMD_EXIT:
                        shouldExit = true;
                        break;
                    case BI_RC:
                        printf("%d\n", forkRet);
                        break;
                    case BI_NOT_BI:
                        forkRet = exec_cmd(&cmd);
                        break;
                    default:
                        printf("SHOULD NOT REACH HERE\n");
                        break;
                }
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
    free_cmd_buff(&cmd);
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

int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff) {
    int cmdCount = 0;

    if (strcmp(cmd_line, "") == 0) {
        cmd_buff->_cmd_buffer[0] = '\0';
        cmd_buff->argv[0] = cmd_buff->_cmd_buffer;
    }

    char *tok = strtok(cmd_line, PIPE_STRING);
    while (tok) {
        int argCount = 0;
        int cmdLen = 0;
        int start = 0;
        bool spaceLast = true;
        bool includeSpace = false;

        while (*tok) {
            if (cmdCount == CMD_MAX) {
                return ERR_TOO_MANY_COMMANDS;
            }
            if (*tok == SPACE_CHAR) {
                if (!spaceLast && !includeSpace) {
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
                if (*tok == '"') {
                    includeSpace = !includeSpace;
                } else {
                    cmd_buff->_cmd_buffer[cmdLen] = *tok;
                }
                spaceLast = false;
                cmdLen++;
            }
            tok++;
        }
        cmd_buff->_cmd_buffer[cmdLen] = '\0';
        cmd_buff->argv[argCount] = cmd_buff->_cmd_buffer + start;
        argCount++;
        cmd_buff->argc = argCount;
        cmdCount++;
        tok = strtok(NULL, PIPE_STRING);
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
    } else if (strcmp(input, "rc") == 0) {
        return BI_RC;
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
            print_dragon();
            return BI_EXECUTED;
        case BI_CMD_CD:
            if (cmd->argc > 1) {
                chdir(cmd->argv[1]);
            }
            return BI_EXECUTED;
        case BI_RC:
            return BI_RC;
        case BI_NOT_BI:
            return BI_NOT_BI;
        case BI_EXECUTED:
            return BI_EXECUTED;
        default:
            printf("Error executing built in command\n");
            exit(ERR_EXEC_CMD);
    }
}

int exec_cmd(cmd_buff_t *cmd) {
    int f_result, c_result;

    f_result = fork();
    if (f_result < 0) {
        perror("fork error");
        exit(ERR_EXEC_CMD);
    }

    if (f_result == 0) {
        int rc = execvp(cmd->argv[0], cmd->argv);
        if (rc < 0) {
            perror("fork error");
            exit(ERR_EXEC_CMD);
        }
    } else {
        int rc = wait(&c_result);
        if (rc < f_result) {
            perror("fork error");
            exit(ERR_EXEC_CMD);
        }
        return c_result;
    }
}
