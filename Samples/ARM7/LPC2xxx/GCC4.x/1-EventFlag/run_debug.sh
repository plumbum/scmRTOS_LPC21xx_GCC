#!/bin/sh

make clean && make && ddd --debugger arm-none-eabi-gdb -x debug.gdb fruits.elf


