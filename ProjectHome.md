# Implementation of forth with following constraints #

  * Approx. 500 lines of c
  * Only uses 5 c funtions, fopen, fclose, fputc, fgetc, and printf for debug version only
  * VM has 21 opcodes
  * Interface with VM is using run, peek and poke for minimal api


## Getting Started ##

On windows can use [Tiny C Compiler](http://bellard.org/tcc/)

  1. Compile CompC - tcc.exe src\CompC.c -o CompC.exe
  1. Copy eforth.cmp to same directory as CompC.exe
  1. Run CompC.exe type BYE - this will produce eforth.img

To create Arduino version run src\Arduino.py in same directory this will
create forth.ino which can be compiled and run in Arduino.ide.
