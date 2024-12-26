#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>


// ================ File Operations BEGIN (Mode 1) ================
void create_file(const char *filename, int truncateExisting) {
    int flags = O_RDWR | O_CREAT;
    if (truncateExisting) {
        flags |= O_TRUNC;
    }
    int fd = open(filename, flags, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd == -1) {
        perror("Error creating/opening file");
    } else {
        printf("File '%s' created/opened successfully. Descriptor: %d\n", filename, fd);
        close(fd);
    }
}

void open_file(const char *filename, int readWrite) {
    int fd;
    if (readWrite) {
        fd = open(filename, O_RDWR);
    } else {
        fd = open(filename, O_RDONLY);
    }
    if (fd == -1) {
        perror("Error opening file");
    } else {
        printf("File '%s' opened successfully. Descriptor: %d\n", filename, fd);
        close(fd);
    }
}

void change_file_permission(const char *filename, mode_t permission) {
    if (chmod(filename, permission) == -1) {
        perror("Error changing file permission");
    } else {
        printf("Permissions changed for '%s'.\n", filename);
    }
}

void read_file(const char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file for reading");
        return;
    }
    char buffer[1024];
    ssize_t bytesRead;
    printf("Contents of '%s':\n", filename);
    while ((bytesRead = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytesRead] = '\0';
        printf("%s", buffer);
    }
    if (bytesRead == -1) {
        perror("Error reading file");
    }
    close(fd);
}

void write_file(const char *filename) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd == -1) {
        perror("Error opening file for writing");
        return;
    }
    printf("Enter text (end with Ctrl+D):\n");
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        ssize_t written = write(fd, buffer, strlen(buffer));
        if (written == -1) {
            perror("Error writing to file");
            close(fd);
            return;
        }
    }
    if (ferror(stdin)) {
        perror("Error reading input");
    }
    close(fd);
    printf("Content written to '%s'.\n", filename);
}

void remove_file_op(const char *filename) {
    if (unlink(filename) == -1) {
        perror("Error removing file");
    } else {
        printf("File '%s' removed successfully.\n", filename);
    }
}
// ================ File Operations END (Mode 1) ================


// ================ Directory Operations BEGIN (Mode 2) ================
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
    /*
      Usage examples:
        // FILE (mode=1):
        ./integrated_ops -m 1 1 newFile.txt           // Create file (create if not exist)
        ./integrated_ops -m 1 1 newFile.txt trunc     // Create file (truncate if exist)
        ./integrated_ops -m 1 2 existingFile.txt rw   // Open file read/write
        ./integrated_ops -m 1 3 file.txt 0644         // Change file permission to 0644
        ./integrated_ops -m 1 4 file.txt              // Read file
        ./integrated_ops -m 1 5 file.txt              // Write to file
        ./integrated_ops -m 1 6 file.txt              // Remove file

        // DIRECTORY (mode=2):
        ./integrated_ops -m 2 1 myDir                 // Create directory
        ./integrated_ops -m 2 2 myDir                 // Remove directory
        ./integrated_ops -m 2 3                       // Print working directory
        ./integrated_ops -m 2 4 myDir                 // List files in directory

    */
    if (argc < 3) {
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
        // ========== MODE 1: FILE OPERATIONS ==========
        // File Operation
        case 1:
            if (argc < 5) {
            fprintf(stderr, "Usage for file ops: %s -m 1 <operation> <filename> [param]\n", argv[0]);
            return 1;
            }
            const char *filename = argv[4];
    
            switch (operation) {
                case 1: // create
                    if (argc >= 6 && strcmp(argv[5], "trunc") == 0) {
                        create_file(filename, 1);
                    } else {
                        create_file(filename, 0);
                    }
                    break;
                case 2: // open
                    if (argc >= 6 && strcmp(argv[5], "rw") == 0) {
                        open_file(filename, 1);
                    } else {
                        open_file(filename, 0);
                    }
                    break;
                case 3: // change permission
                    if (argc < 6) {
                        fprintf(stderr, "Error: Missing permission. Example: 0644\n");
                        return 1;
                    } else {
                        mode_t perm = (mode_t)strtol(argv[5], NULL, 8);
                        change_file_permission(filename, perm);
                    }
                    break;
                case 4: // read
                    read_file(filename);
                    break;
                case 5: // write
                    write_file(filename);
                    break;
                case 6: // remove
                    remove_file_op(filename);
                    break;
                default:
                    fprintf(stderr, "Error: Invalid file operation.\n");
                    return 1;
            }
        // ========== MODE 2: DIRECTORY OPERATIONS ==========
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
        // ========== MODE 3: KEYLOGGER OPERATIONS ==========
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
