    .code16gcc
    .global _start
    .text

    jmp _start
_start:
    call main
    ret
    