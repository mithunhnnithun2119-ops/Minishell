# Mini Shell

A custom Unix shell implementation in C that mimics core functionality of a real terminal, including command execution, signal handling, job control, and environment variable support.

## Features

### 1. Command Execution
- Parses and executes user input (`input_str`)
- Detects and handles environment/prompt-related input such as `PS1=`

### 2. Signal Handling
Implements custom signal handling using `signal()` (no `sigaction()` required):

- **SIGINT (Ctrl + C)**
  - Does not terminate the shell
  - Routed to a custom `signal_handler()`
  - Reprints the prompt (only in the parent process)

- **SIGTSTP (Ctrl + Z)**
  - Does not terminate the shell
  - Routed to `signal_handler()`
  - Reprints the prompt in the parent, and stores the stopped process in the parent
  - Default signal behavior (`SIG_DFL`) is restored in the **child process only**, so signals affect the running command as expected (e.g. `sleep 30` + Ctrl+C terminates it; `sleep 30` + Ctrl+Z stops it)

- **Avoiding duplicate prompts**
  - A global `pid` variable is used to determine whether the process is the parent or child
  - The prompt is only printed once (from the handler, guarded by `pid == 0`), avoiding a double-print from both the handler and the main loop

- **SIGCHLD**
  - Sent to the parent when a child process changes state
  - Used with `waitpid(pid, &status, WUNTRACED)` so the parent is correctly notified when a child is stopped (not just when it exits)
  - Used to clean up resources once a background/stopped process has completed

### 3. Job Control (`jobs`, `fg`, `bg`)
Implemented using a linked list of stopped processes:

```c
struct input
{
    int pid;
    char command[15];
    struct input *link;
};
```

- **`jobs`**
  - Detected via input parsing
  - Prints all currently stopped processes stored in the linked list

- **`fg`**
  - Detected via input parsing
  - Resumes the last stopped process in the foreground: `kill(head->pid, SIGCONT)`
  - Waits for it to finish/stop again: `waitpid(head->pid, &status, WUNTRACED)`
  - Removes the process from the linked list (`delete_first()`) once handled
  - Blocks other shell operations until the foreground process completes

- **`bg`**
  - Detected via input parsing
  - Resumes the last stopped process in the background: `kill(head->pid, SIGCONT)`
  - Removes it from the linked list (`delete_first()`)
  - Shell remains available for other operations
  - Process completion is detected via `SIGCHLD`, at which point resources are cleared in the handler

### 4. Environment Variables
Implemented in `command.c`:

- **`echo $$`** — prints the current process ID using `getpid()`
- **`echo $?`** — prints the exit status of the previous command using `WEXITSTATUS()` on the stored status (0 = success, non-zero = failure)
- **`echo $SHELL`** — prints the value of the `SHELL` environment variable using `getenv("SHELL")`

## File Structure

- `command.c` — command parsing and handling logic (`jobs`, `fg`, `bg`, environment variables)
- `signal_handler()` — central handler for `SIGINT`, `SIGTSTP`, and `SIGCHLD`
- Linked list utilities — `insert_first()`, `delete_first()` for tracking stopped jobs

## Notes / Design Decisions

- The prompt is **not** printed by the real terminal on Ctrl+C/Ctrl+Z, but the mini shell intentionally prints it for clarity.
- The global `pid` flag distinguishes parent vs. child inside the signal handler to avoid printing the prompt twice; it is reset after use.
- Default signal dispositions (`SIG_DFL`) are restored **only in the child**, right before `exec`, so the shell itself remains immune to `SIGINT`/`SIGTSTP` while spawned commands behave normally.

## Development Log

This project was built incrementally:

1. Basic signal handling for `SIGINT` / `SIGTSTP` without terminating the shell
2. Correct parent/child signal disposition and prompt de-duplication
3. `SIGCHLD`-based tracking of stopped children via `waitpid(..., WUNTRACED)`
4. Job control commands: `jobs`, `fg`, `bg`, backed by a linked list of stopped processes
5. Environment variable support: `echo $$`, `echo $?`, `echo $SHELL`

## Building

```bash
gcc -o mini_shell *.c
./mini_shell
```

## Example Usage

```bash
$ sleep 30
^C
$              # process terminated, prompt reprinted

$ sleep 30
^Z
$ jobs
[1] sleep 30
$ bg
$ fg
```
