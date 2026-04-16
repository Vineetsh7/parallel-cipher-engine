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

This will compile the C++ source files and generate two executables:
- `pce` / `pce.exe` — the main parallel cipher engine
- `cipher` / `cipher.exe` — a standalone cipher runner

## Configuration

The application requires a numerical encryption key to function. 
1. Create a file exactly named `.env` in the root of the project directory.
2. Inside this file, place your integer encryption/decryption key.

**Example `.env` file content:**
```text
8745
```

## How to Run

The engine is driven entirely by command-line flags — there are no interactive prompts.

```bash
# Linux / macOS / WSL
./pce --dir <directory_path> --action <encrypt|decrypt>

# Windows
.\pce.exe --dir <directory_path> --action <encrypt|decrypt>
```

**Flags:**
| Flag | Description |
|------|-------------|
| `--dir` | Relative or absolute path to the folder to process |
| `--action` | Either `encrypt` or `decrypt` |

**Examples:**
```bash
./pce --dir ./test_folder --action encrypt
./pce --dir C:\my_files  --action decrypt
```

If either flag is missing or `--action` is not `encrypt`/`decrypt`, the engine prints usage and exits:
```text
Usage: pce --dir <directory_path> --action <encrypt|decrypt>
```

The engine will then queue all files recursively and use the initialized thread pool to rapidly process them, printing start and end timestamps.

## Project Structure Highlights
- `/src/core/scheduler/`: Contains the Thread Pool management and task queue.
- `/src/core/cipher/`: Core robust block processing for encryption/decryption routines.
- `/src/core/io/`: Handles efficient file streaming and `.env` config loading.
- `engine.cpp`: The main entry point integrating CLI UX, configuration parsing, and worker queue submissions.
