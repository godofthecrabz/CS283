#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *, int);
int  setup_buff(char *, char *, int);

//prototypes for functions to handle required functionality
int  count_words(char *, int, int);
//add additional prototypes here
void reverse(char *, int);
void wordPrint(char *, int);
void searchAndReplace(char *, int, char *, char *);


int setup_buff(char *buff, char *user_str, int len){
    //TODO: #4:  Implement the setup buff as per the directions
    // Initialize user string counter and whitespace counter to 0
    int strLen = 0;
    int whitespace = 0;
    // Loop until user_str is empty
    while (*user_str) {
        // Check if user_str is larger than buff
        if (strLen > len) {
            return -1;
        }
        // Check for whitespace characters and handle repeating whitespace
        if (*user_str == ' ' || *user_str == '\t' || *user_str == '\b' || *user_str == '\n') {
            if (whitespace == 0) {
                *buff = ' ';
                buff++;
            }
            whitespace++;
            strLen++;
            user_str++;
            continue;
        }
        // Reset whitespace counter
        whitespace = 0;
        // Put non-whitespace characters into buff
        *buff = *user_str;
        buff++;
        user_str++;
        strLen++;
    }
    // Fill rest of buff with .
    int i = strLen;
    while (i < len) {
        *buff = '.';
        buff++;
        i++;
    }
    return strLen;
}

void print_buff(char *buff, int len){
    printf("Buffer:  ");
    for (int i=0; i<len; i++){
        putchar(*(buff+i));
    }
    putchar('\n');
}

void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);
}

int count_words(char *buff, int len, int str_len){
    //YOU MUST IMPLEMENT
    int words = 0;
    for (int i = 0; i < len; i++) {
        if (*buff == '.')
            return ++words;
        if (*buff == ' ')
            words++;
        buff++;
    }
    return words;
}

//ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS
void reverse(char *buff, int len) {
    int start = 0, last;
    char temp;
    for (int i = 0; i < len; i++) {
        if (*(buff + i) == '.') {
            last = i - 1;
            break;
        }
    }
    while (start < last) {
        temp = *(buff + start);
        *(buff + start) = *(buff + last);
        *(buff + last) = temp;
        start++;
        last--;
    }
}

void wordPrint(char *buff, int len) {
    printf("Word Print\n----------\n");
    int charCount = 0;
    int wordCount = 1;
    printf("%d. ", wordCount);
    while (*buff != '.') {
        if (*buff == ' ') {
            //print word and count
            printf(" (%d)\n%d. ", charCount, ++wordCount);
            charCount = 0;
            buff++;
            continue;
        }
        putchar(*buff);
        charCount++;
        buff++;
    }
    printf(" (%d)\n", charCount);
}

void searchAndReplace(char *buff, int len, char *old, char *new) {
    int start = 0, end = 0, oldSize = 0;
    char localBuff[BUFFER_SZ];
    char *temp = old;
    while (*temp) {
        oldSize++;
        temp++;
    }
    for (int i = 0; i < len; i++) {
        if (*(buff + i) == ' ' || *(buff + i) == '.') {
            temp = buff + start;
            if (oldSize == i - start && memcmp(temp, old, oldSize) == 0) {
                temp = new;
                while (*temp) {
                    *(localBuff + start) = *temp;
                    start++;
                    temp++;
                }
                end = start;
                start++;
            } else {
                start = i + 1;
            }
        }
        *(localBuff + end) = *(buff + i);
        end++;
    }
    memcpy(buff, localBuff, BUFFER_SZ);
}

int main(int argc, char *argv[]){

    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    /*
    *   The amount of arguments in argv is always one more than argc as there will always be a 
    *   null at the end of argv. Since, the name of the executable is the first argument in argv and
    *   always will be there will always be a value in argv at locations 0 and 1.
    */
    if ((argc < 2) || (*argv[1] != '-')){
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1]+1);   //get the option flag

    //handle the help flag and then exit normally
    if (opt == 'h'){
        usage(argv[0]);
        exit(0);
    }

    //WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    //TODO:  #2 Document the purpose of the if statement below
    /*
    *   The purpose of the if statement below is to catch if a user of the program correctly
    *   puts the flag in but forgets to put the "sample string"
    */
    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    // CODE GOES HERE FOR #3
    buff = malloc(BUFFER_SZ);
    if (!buff) {
        printf("Error allocating buffer memory");
        exit(99);
    }


    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);     //see todos
    if (user_str_len < 0){
        printf("Error setting up buffer, error = %d", user_str_len);
        exit(2);
    }

    char* temp;
    int count = 0;
    switch (opt){
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len);  //you need to implement
            if (rc < 0){
                printf("Error counting words, rc = %d", rc);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            break;

        //TODO:  #5 Implement the other cases for 'r' and 'w' by extending
        //       the case statement options
        case 'r':
            reverse(buff, BUFFER_SZ);
            printf("Reversed String: ");
            temp = buff;
            while (*temp != '.' && count < BUFFER_SZ) {
                putchar(*temp);
                temp++;
                count++;
            }
            putchar('\n');
            break;
        case 'w':
            wordPrint(buff, BUFFER_SZ);
            break;
        case 'x':
            if (argc < 5) {
                usage(argv[0]);
                exit(1);
            }
            searchAndReplace(buff, BUFFER_SZ, argv[3], argv[4]);
            printf("Modified String: ");
            temp = buff;
            while (*temp != '.' && count < BUFFER_SZ) {
                putchar(*temp);
                temp++;
                count++;
            }
            putchar('\n');
            break;
        default:
            usage(argv[0]);
            exit(1);
    }

    //TODO:  #6 Dont forget to free your buffer before exiting
    print_buff(buff,BUFFER_SZ);
    free(buff);
    exit(0);
}

//TODO:  #7  Notice all of the helper functions provided in the 
//          starter take both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all we know from main() that 
//          the buff variable will have exactly 50 bytes?
//  
/*
*   It is generally a good practice to provide the buffer length along with the buffer
*   because most arrays will not be a preknown fixed size. If the functions we had to write
*   did not take in the length of the array as an argument then the code would only work for
*   our exact buffer size.
*/