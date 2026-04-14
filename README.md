# Parallel Cipher Engine

A high-performance, multithreaded file encryption and decryption engine built in C++17. It efficiently traverses directories and processes multiple files concurrently.

## Features
- **High-speed Multithreading**: Utilizes a robust thread pool pattern for concurrent processing, maximizing CPU core utilization.
- **Recursive Directory Processing**: Encrypts or decrypts entire directory trees seamlessly.
- **Optimized I/O**: Lightweight task scheduling handles file operations cleanly without maintaining giant OS blocks in memory.
- **Clean Concurrency**: Prevents race conditions and ensures all threads safely exit upon task completion.

## Prerequisites
- A **C++17** compliant compiler (e.g., `g++` via MinGW/GCC, Clang, or MSVC)
- GNU `make` utility

## Build Instructions

To build the project, open your terminal at the project's root directory and run:

```bash
make clean
make all
```

This commands will compile the C++ source files and generate the main executable named `pce` (or `pce.exe` on Windows).

## Configuration

The application requires a numerical encryption key to function. 
1. Create a file exactly named `.env` in the root of the project directory.
2. Inside this file, place your integer encryption/decryption key.

**Example `.env` file content:**
```text
8745
```

## How to Run

Start the compiled engine from your terminal:

```bash
./pce          # Linux / macOS / WSL
# OR
.\pce.exe      # Windows
```

When you run it, the program will interactively prompt you for inputs:

```text
Enter the directory path: ./test_folder
Enter the action (encrypt/decrypt): encrypt
```

1. **directory path**: Enter the relative or absolute path to the folder containing the files you wish to process (e.g., `C:\my_files` or `./src`).
2. **action**: Type either `encrypt` or `decrypt`.

The engine will then queue all files recursively and use the initialized thread pool to rapidly process them, printing start and end timestamps.

## Project Structure Highlights
- `/src/core/scheduler/`: Contains the Thread Pool management and task queue.
- `/src/core/cipher/`: Core robust block processing for encryption/decryption routines.
- `/src/core/io/`: Handles efficient file streaming and `.env` config loading.
- `engine.cpp`: The main entry point integrating CLI UX, configuration parsing, and worker queue submissions.
