#!/bin/bash

as --64 out.s -o output.o
ld output.o -o output
./output
echo $?