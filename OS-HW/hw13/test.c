#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#define INITIAL_FILE_NAME "a"
#define TEMP_DIRECTORY "temp_dir"

void exit_on_error(char *err_message) {
    perror(err_message);
    exit(EXIT_FAILURE);
}

int main(void) {
    char current_file_name[3] = INITIAL_FILE_NAME;
    char symlink_name[3] = "aa";
    int recursion_depth = 0;
    int file_descriptor;

    if(mkdir(TEMP_DIRECTORY, 0755) < 0)
        exit_on_error("Directory creation failed");

    if(chdir(TEMP_DIRECTORY) < 0)
        exit_on_error("Directory change failed");

    if((file_descriptor = open(current_file_name, O_CREAT | O_RDWR, 0666)) < 0)
        exit_on_error("File creation failed");
    close(file_descriptor);

    while(1) {
        if(symlink(current_file_name, symlink_name) < 0)
            exit_on_error("Symlink creation failed");

        if((file_descriptor = open(symlink_name, O_RDWR)) < 0) {
            printf("Maximum symlink recursion depth: %d\n", recursion_depth);
            break;
        }
        close(file_descriptor);
        recursion_depth++;

        strcpy(current_file_name, symlink_name);
        if(symlink_name[1] == 'z') {
            symlink_name[0]++;
            symlink_name[1] = 'a';
        } else {
            symlink_name[1]++;
        }
    }

    return EXIT_SUCCESS;
}
