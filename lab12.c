#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <unistd.h>

// This function use is to separat the  command line arguments into left and right groups.
// It splits them at the special -pipe divider argument.
void split_arguments(char *argv[], int argc, char *left_args[], char *right_args[],
                    int *left_count, int *right_count) {
    // This flag remembers whether the -pipe divider is found yet.
    bool found_pipe = false;
    
    // start counting left arguments from zero.
    *left_count = 0;
    
    // start counting right arguments from zero.
    *right_count = 0;
    
    // Examining each argument starting from pthe first position.
    for (int i = 1; i < argc; i++) {
        // Checks to see if the current argument is our -pipe divider.
        if (strcmp(argv[i], "-pipe") == 0) {
            // Sets the flag when  finding the divider.
            found_pipe = true;
        } else {
            // For the non-divider arguments, decide which side they belong on.
            if (!found_pipe) {
                // Stores the left side arguments within the left_args array.
                left_args[(*left_count)++] = argv[i];
            } else {
                // Stores the right-side arguments withiin the right_args array.
                right_args[(*right_count)++] = argv[i];
            }
        }
    }
    
    // Marks the end of left arguments with NULL for the execvp.
    left_args[*left_count] = NULL;
    
    // Marks the end of right arguments with NULL for the execvp
    right_args[*right_count] = NULL;
}

// This is the main function, the entry point. It creates the pipe between two commands.
// It handles all setup and error checking for the piping operation.
int main(int argc, char *argv[]) {
    // First verifies if received the minimum required arguments.
    if (argc < 4) {
        // Shows the  proper usage if  arguments are missing.
        printf("Usage: %s <left command> [args...] -pipe <right command> [args...]\n", argv[0]);
        // Exits with an error code when arguments are insufficient.
        return 1;
    }
    
    //  arrays implemented  to hold arguments for both commands.
    char *left_args[argc];
    char *right_args[argc];
    
    // These variables will track argument counts for each side.
    int left_count, right_count;
    
    // Splits the arguments into left and right groups.
    split_arguments(argv, argc, left_args, right_args, &left_count, &right_count);
    
    // Verifies that it was actually received a left-side command.
    if (left_count == 0) {
        // Prints an error message if left command is missing.
        printf("Error: Missing left command\n");
        // Returns an error code when left command is absent.
        return 1;
    }
    
    // Verifies if actually received a right-side command.
    if (right_count == 0) {
        // Prints an error message if right command is missing.
        printf("Error: Missing right command\n");
        // Returns an error code when right command is absent.
        return 1;
    }
    
    // Prepares the  file descriptors for the communication pipe.
    int pipefd[2];
    
    // Attempts to create the pipe and checks for success.
    if (pipe(pipefd) == -1) {
        // Notifies user if  the pipe creation has failed.
        printf("pipe error\n");
        // Exits with error code when pipe creation fails.
        return 1;
    }
    
    // Creates a child process by calling  the fork()
    pid_t pid = fork();
    
    // Checks to see if the fork operation  has succeeded.
    if (pid == -1) {
        // Prints an error message if the  fork has failed.
        printf("fork error\n");
        // Returns an error code when  the process creation  has failed.
        return 1;
    }
    
    // The child process will handle the left-side command.
    if (pid == 0) {
        // Closes the read end of the pipe thats not in use.
        close(pipefd[0]);
        
        // Redirects the command output to the pipe's write end.
        if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
            // Warns the  user if  the redirection failed.
            printf("dup2 error\n");
            // Exits  with an error  code when the  redirection failed.
            return 1;
        }
        
        // Closes the original write end after completing redirection.
        close(pipefd[1]);
        
        // Replaces this process with the left-side command.
        execvp(left_args[0], left_args);
        
        // This only executes if the command failed to run.
        printf("execvp left error\n");
        // Returs an error code when command execution fails.
        return 1;
    } 
    // The parent process will handle the right-side command.
    else {
        // Close the write end of the pipe thats not in use.
        close(pipefd[1]);
        
        // Redirects command input to come from pipe's read end.
        if (dup2(pipefd[0], STDIN_FILENO) == -1) {
            // Warns the  user if the redirection has failed.
            printf("dup2 error\n");
            // Exits with an error code when redirection fails.
            return 1;
        }
        
        // Closes the  original read end after completing redirection.
        close(pipefd[0]);
        
        // Replaces this process with the right-side command.
        execvp(right_args[0], right_args);
        
        // This only executes if the command failed to run.
        printf("execvp right error\n");
        // Returns  an error code when  the command execution fails.
        return 1;
    }
    
    // This return statement is included for  the completeness of the code.
    return 0;
}