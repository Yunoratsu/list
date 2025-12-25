Simple LS (Beta)

A simple reimplementation of the Unix `ls` command in C.  
This project is a learning exercise to understand file handling, CLI argument parsing, and portable code design.

## Features
- Basic file listing
- Color output for directories and executables
- Early experiments with `stat` structures for file metadata
- Work-in-progress CLI options

## Roadmap
- [ ] Rewrite `strcmp` in a professional manner
- [ ] Implement a custom `snprintf` for formatted output
- [ ] Add argument handling (CLI options like `-a`, `-l`, etc.)
- [ ] Improve portability across systems
- [ ] Reimplement and refine until stable

## Notes
See `src/notes.txt` for development notes and resources.  
See `src/todo.txt` for planned improvements.

## Build
```bash
make
sudo make install

## Usage

list

## Planned Options
These options are not yet implemented but are part of the roadmap:
•    → Show hidden files
•    → Show detailed file information (permissions, size, etc.)
•    → List files in a specific directory

## Version
Current version: 0.01

## License
This project is licensed under the MIT License — see LICENSE for details.
