# Contributing to the HIBAZERO projects

Thank you for your interest in contributing to this project! Since this project involves low level C code, If you are new to OSdev, It is recommended to go through this guide before submitting issues or pull requests.

# Table of contents:

- [Setting up Toolchain](https://github.com/SmartHimanshu/HIBAZERO?tab=contributing-ov-file#setting-up-toolchain--development-environment)

- [Coding Standards](https://github.com/SmartHimanshu/HIBAZERO?tab=contributing-ov-file#coding-standards--constraints)

- [Commit Messages Guidelines](https://github.com/SmartHimanshu/HIBAZERO?tab=contributing-ov-file#coding-standards--constraints)

## Setting up Toolchain & Development Environment

In order to build & test this project, you must use the i686- cross compiler toolchain:

### Required Dependencies

Make sure you have the following packages intalled:

- __Make__      (GNU Make v4.0+)
- __GNU AS__    (GAS)
- __QEMU__      (x86_64 target preferrable)
- __GDB__       (Debugging)

### Building Cross-Compiler

Please refer to these sources for Cross-Compiler setup:

- __x86_64__ (For kernel) => https://wiki.osdev.org/GCC_Cross-Compiler
- __i686__ (For boot loader) => https://wiki.osdev.org/GCC_Cross-Compiler

## Coding standards & Constraints

1. __Code Sanity__ => You must make sure that your code doesn't have any errors or whatever kind of feature you are adding, you must thoroughly check if it is working or not.

2. __Anti Bloat__ => Since the final compiled binary of the bootloader must be under 1MB, it is advised not to make useless extra drivers for what range has already been decided.

3. __No Code Hinderence__ => This is more of an advised rule, your code or module you code like a driver for example, should not disrupt the main flow of the other code interacting with it.

4. __Use of AI__ => It is advised not to use AI for this build. We would not recommend it because, AI generates a lot of bad code. However, you are free to use AI for contributing etc. Since OSdev is such a difficult and complex coding, we generally don't advise it. 

## How to contribute

1. __Step 1: Open an Issue__ => Before writing code, search the issue tracker to ensure your feature or bug is not already being addressed. If it is new, open an issue explaining your proposed changes.

2. __Step 2: Branching and Development__ => Name your branch in detail. Examples : fix/serial_driver_bug or feature/vbe_implementation

3. __Step 3: Following Code Constraints__ => Refer to the points mentioned above in __Coding standards & Constraints__ section.

## Commit Messages Guidelines

We use clear commit messages to ensure we have a readable repositry history.

Format: [Scope] Action Description

Example: [HIBABLZERO] Fix serial driver for uart 8520 chip data line.

Also please ensure that since HIBAZERO is a collection of projects designed to work with each other and possibly with other bootloaders and kernels, In the __Scope__ field, please ensure to use the correct project specific folder.