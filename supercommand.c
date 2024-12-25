#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

// Create directory function
void create_directory(const char *path) {
    if (mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO) == 0) {
        printf("Directory created successfully.\n");
    } else {
        perror("mkdir");
    }
}

// Remove directory function
void remove_directory(const char *path) {
    // Block removal of critical system directories
    if (strcmp(path, "/") == 0 || strcmp(path, "/home") == 0 || strcmp(path, "/root") == 0) {
        fprintf(stderr, "Error: Attempt to remove a critical system directory is not allowed.\n");
        return;
    }

    if (rmdir(path) == 0) {
        printf("Directory removed successfully.\n");
    } else {
        perror("rmdir");
    }
}

// Print working directory function
void print_working_directory() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working directory: %s\n", cwd);
    } else {
        perror("getcwd");
    }
}

// List directory function
void list_directory(const char *path) {
    struct dirent *entry;
    DIR *dir_ptr = opendir(path);

    if (dir_ptr == NULL) {
        perror("opendir");
        return;
    }
    while ((entry = readdir(dir_ptr))) {
        printf("%s\n", entry->d_name);
    }
    closedir(dir_ptr);
}

// Main function
int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s -m <mode> <operation> <args>\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "-m") != 0) {
        fprintf(stderr, "Error: Missing or incorrect '-m' flag.\n");
        return 1;
    }
    
    // atoi() is used to convert string to integer
    int mode = atoi(argv[2]);
    int operation = atoi(argv[3]);

    switch (mode) {
        // File Operation
        case 1:
            if (operation == 1 && argc == 5) {
                // File function call 
                printf("File functionality is otw.\n");
            } else {
                fprintf(stderr, "Invalid file operation.\n");
                return 1;
            }
            break;

        // Directory Operation
        case 2:
            switch (operation) {
                case 1:
                    if (argc == 5) {
                        create_directory(argv[4]);
                    } else {
                        fprintf(stderr, "Invalid directory operation.\n");
                        return 1;
                    }
                    break;
                case 2:
                    if (argc == 5) {
                        remove_directory(argv[4]);
                    } else {
                        fprintf(stderr, "Invalid directory operation.\n");
                        return 1;
                    }
                    break;
                case 3:
                    if (argc == 4 || (argc == 5 && strcmp(argv[4], ".") == 0)) {
                        print_working_directory();
                    } else {
                        fprintf(stderr, "Invalid directory operation.\n");
                        return 1;
                    }
                    break;
                case 4:
                    if (argc == 5) {
                        list_directory(argv[4]);
                    } else {
                        fprintf(stderr, "Invalid directory operation.\n");
                        return 1;
                    }
                    break;
                default:
                    fprintf(stderr, "Invalid directory operation.\n");
                    return 1;
            }
            break;

        // Keylogger Operation
        case 3:
            if (argc == 4) {
                // Keylogger function call 
                printf("Keylogger functionality is otw.\n");
            } else {
                fprintf(stderr, "Invalid keylogger operation.\n");
                return 1;
            }
            break;

        // Default case
        default:
            fprintf(stderr, "Invalid mode: %d\n", mode);
            return 1;
    }
    return 0;
}
