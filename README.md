# TBCC

This project is a naive C compiler following the [book](https://norasandler.com/book/) by Nora Sandler.

### Progress

- Chapter 1: Constant integers
    - [/] Lexing a basic source file
    - [/] Parsing the produced tokens
    - [/] Converting the C abstract syntax tree into an assembly abstract
            syntax tree.
    - [ ] Generating assembly code from the tree.

Currently I am working on ensuring I don't have any errors or memory leaks in my
program since I am working with a lot of dynamically allocated memory in my data
structures. Once I am confident in it, I am going to move on to the codegen for 
chapter one.