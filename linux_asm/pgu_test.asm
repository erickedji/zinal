#           after running the program
#
#VARIABLES:
#           %eax holds the system call number
#           %ebx holds the return status
#
.section .data
.section .text
.globl _start
_start:
movl $1, %eax       # this is the linux kernel command
# number (system call) for exiting
# a program
movl $3, %ebx       # this is the status number we will
# return to the operating system.
# Change this around and it will
# return different things to
# echo $?
int $0x80           # this wakes up the kernel to run
# the exit command
                                                                       
