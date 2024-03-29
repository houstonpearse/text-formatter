# Basic Text Formatter

A basic text formatter program written in 2019 in my first year at unimelb.

## Usage

`gcc main.c -o tfmt`

`cat test.txt | ./tfmt`

## text processing commands

commands must be at the start of a line

| command  | effect                          |
| -------- | ------------------------------- |
| .p       | Paragraph break                 |
| .w <int> | Set line width + new praragraph |
| .l <int> | Set line margin + new paragraph |
| .c       | Center line                     |
| .h <int> | Header 1,2, or 3                |
| .b       | Line break                      |
