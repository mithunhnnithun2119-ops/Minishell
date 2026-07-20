# Mini Shell

A lightweight Linux command-line interpreter written in C, built to demonstrate core system programming concepts such as process creation, signal handling, and job control.

## Overview

Mini Shell mimics the behavior of a standard Unix shell. It reads commands from the user, parses them, and executes them by forking child processes. Beyond simple command execution, it supports job control — allowing processes to be run in the background, stopped, resumed, and managed — much like a real shell such as `bash`.

## Features

- **Command Execution** — Runs external programs and system commands using `fork()` and `exec()` family system calls.
- **Built-in Commands**
  - `cd` — Change the current working directory.
  - `exit` — Exit the shell.
  - `jobs` — List all background and stopped jobs.
  - `fg` — Bring a background/stopped job to the foreground.
  - `bg` — Resume a stopped job in the background.
- **Signal Handling**
  - `SIGINT` (Ctrl+C) — Interrupts the foreground process without terminating the shell.
  - `SIGTSTP` (Ctrl+Z) — Suspends the foreground process.
  - `SIGCHLD` — Handles child process termination/status changes asynchronously.
- **Job Control** — Tracks background and stopped processes using a linked list data structure, storing job ID, process ID, state, and command name.

## Concepts Demonstrated

- Process creation and management (`fork`, `exec`, `wait`/`waitpid`)
- Signal handling and asynchronous event management
- Job control and process state tracking (Running, Stopped, Done)
- Linked list-based data structures for dynamic job management
- Linux system call usage and low-level system programming

## Getting Started

### Prerequisites

- A Linux operating system
- GCC compiler

### Build

```bash
gcc -o minishell minishell.c
```

### Run

```bash
./a.out
```

## Usage

```
mini-shell$ ls -l
mini-shell$ sleep 100 &
mini-shell$ jobs
[1]  Running    sleep 100 &
mini-shell$ fg 1
mini-shell$ cd ..
mini-shell$ exit
```

## Project Structure

```
mini-shell/
├── minishell.c     # Main source code
├── README.md       # Project documentation
```

## Future Improvements

- Support for I/O redirection (`>`, `<`, `>>`)
- Support for pipes (`|`)
- Command history and auto-completion
- Environment variable expansion

## License

This project is open source and available for educational use.
