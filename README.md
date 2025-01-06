# TMN4133SP--Group05-Project

Tan Chuan Hock (81341) <br>
Rahul Kumar Shawn A/L Rajesh Kumar (82102) <br>
Richard Jong Wei Ze (81049) <br>
Yeow Lik Zen (81540) <br>    

## Tasks
- TASK A: Coding Task
- TASK B: TESTING AND EVALUATION
- TASK C: REFLECTION
- TASK D: USING GITHUB 
- TASK E: PROJECT PRESENTATION

## Table of Contents

- [Requirements](#requirements)
- [Setup Instructions](#setup-instructions)
- [Usage](#usage)


## Requirements

To run this script, you will need:

- A C compiler (e.g., `gcc`)
- A Linux environment (for keylogger operations)

## Setup Instructions

1. Clone the repository:
    ```bash
    git clone https://github.com/TanCH010/TMN4133SP-Group05-Project.git
    cd TMN4133SP-Group05-Project
    ```

2. Compile the programs:
    ```bash
    gcc -o supercommand supercommand.c
    gcc -o supercommand_menu supercommand_menu.c
    ```

3. Install the man page:
    ```bash
    sudo mkdir -p /usr/local/share/man/man1
    sudo cp supercommand.1 /usr/local/share/man/man1/
    sudo mandb
    man supercommand
    ```

## Usage

### Command-Line Usage (supercommand.c)

#### File Operations (mode=1)

- Create file (create if not exist):
    ```sh
    ./supercommand -m 1 1 newFile.txt
    ```

- Create file (truncate if exist):
    ```sh
    ./supercommand -m 1 1 newFile.txt trunc
    ```

- Open file read/write:
    ```sh
    ./supercommand -m 1 2 existingFile.txt rw
    ```

- Change file permission to 0644:
    ```sh
    ./supercommand -m 1 3 file.txt 0644
    ```

- Read file:
    ```sh
    ./supercommand -m 1 4 file.txt
    ```

- Write to file:
    ```sh
    ./supercommand -m 1 5 file.txt
    ```

- Remove file:
    ```sh
    ./supercommand -m 1 6 file.txt
    ```

#### Directory Operations (mode=2)

- Create directory:
    ```sh
    ./supercommand -m 2 1 myDir
    ```

- Remove directory:
    ```sh
    ./supercommand -m 2 2 myDir
    ```

- Print working directory:
    ```sh
    ./supercommand -m 2 3
    ```

- List directory:
    ```sh
    ./supercommand -m 2 4 myDir
    ```

#### Keylogger Operations (mode=3)

- Run the keylogger on the specified input device:
    ```sh
    ./supercommand -m 3 /dev/input/event0
    ```

### Menu-Based Usage (supercommand_menu.c)

Run the menu-based program:
```sh
./supercommand_menu
