/*
 *    SPDX-FileCopyrightText: 2021 Monaco F. J. <monaco@usp.br>
 *    SPDX-FileCopyrightText: 2024 MurilloMMartins <murillomartins19@gmail.com>
 *   
 *    SPDX-License-Identifier: GPL-3.0-or-later
 *
 *  This file is a derivative work from SYSeg (https://gitlab.com/monaco/syseg)
 *  and contains modifications carried out by the following author(s):
 *  MurilloMMartins <murillomartins19@gmail.com>
 */

/* This source file implements the kernel entry function 'kmain' called
   by the bootloader, and the command-line interpreter. Other kernel functions
   were implemented separately in another source file for legibility. */

#include "bios1.h"		/* For kwrite() etc.            */
#include "bios2.h"		/* For kread() etc.             */
#include "kernel.h"		/* Essential kernel functions.  */
#include "kaux.h"		/* Auxiliary kernel functions.  */
#include "file_system.h"

/* Kernel's entry function. */

void put_short(int num) {
    // Handle negative numbers
    if (num < 0) {
        kwrite("-");
        num = -num;
    }

    // Convert short int to string
    char buffer[20]; // Assuming a maximum of 5 digits (plus sign and null terminator)
    int index = 0;
    
    // Special case for num = 0
    if (num == 0) {
        buffer[index++] = '0';
    }
    else {
        while (num > 0) {
            buffer[index++] = '0' + (num % 10);
            num /= 10;
        }
    }

    // Add null terminator
    buffer[index] = '\0';

    // Reverse the string in place
    int start = 0;
    int end = index - 1;
    while (start < end) {
        char temp = buffer[start];
        buffer[start] = buffer[end];
        buffer[end] = temp;
        start++;
        end--;
    }

    // Output the entire string using puts
    kwrite(buffer);
}

void kmain(void)
{
  int i, j;
  
  register_syscall_handler();	/* Register syscall handler at int 0x21.*/

  splash();			/* Uncessary spash screen.              */

  clear();

  shell();			/* Invoke the command-line interpreter. */
  
  halt();			/* On exit, halt.                       */
  
}

/* Tiny Shell (command-line interpreter). */

char buffer[BUFF_SIZE];
int go_on = 1;

void shell()
{
  int i;
  clear();
  kwrite ("gladOS 1.0\n");
  
  while (go_on)
    {

      /* Read the user input. 
	 Commands are single-word ASCII tokens with no blanks. */
      do
	{
	  kwrite(PROMPT);
	  kread (buffer);
	}
      while (!buffer[0]);

      /* Check for matching built-in commands */
      
      i=0;
      while (cmds[i].funct)
	{
	  if (!strcmp(buffer, cmds[i].name))
	    {
	      cmds[i].funct();
	      break;
	    }
	  i++;
	}

      /* If the user input does not match any built-in command name, just
	 ignore and read the next command. If we were to execute external
	 programs, on the other hand, this is where we would search for a 
	 corresponding file with a matching name in the storage device, 
	 load it and transfer it the execution. Left as exercise. */
      
      if (!cmds[i].funct)
	kwrite ("Command not found\n");
    }
}


/* Array with built-in command names and respective function pointers. 
   Function prototypes are in kernel.h. */

struct cmd_t cmds[] =
  {
    {"help",    f_help},     /* Print a help message.       */
    {"quit",    f_quit},     /* Exit TyDOS.                 */
    {"exec",    f_exec},     /* Execute an example program. */
    {"list",    f_list},
    {0, 0}
  };


/* Build-in shell command: help. */

void f_help()
{
  kwrite ("...me, Obi-Wan, you're my only hope!\n\n");
  kwrite ("   But we can try also some commands:\n");
  kwrite ("      exec    (to execute an user program example)\n");
  kwrite ("      list    (list all files in the filesystem)\n");
  kwrite ("      quit    (to exit gladOS)\n");
}

void f_quit()
{
  kwrite ("Program halted. Bye.");
  go_on = 0;
}

/* Built-in shell command: example.

   Execute an example user program which invokes a syscall.

   The example program (built from the source 'prog.c') is statically linked
   to the kernel by the linker script (tydos.ld). In order to extend the
   example, and load and external C program, edit 'f_exec' and 'prog.c' choosing
   a different name for the entry function, such that it does not conflict with
   the 'main' function of the external program.  Even better: remove 'f_exec'
   entirely, and suppress the 'example_program' section from the tydos.ld, and
   edit the Makefile not to include 'prog.o' and 'libtydos.o' from 'tydos.bin'.

  */

void f_exec()
{
  kwrite("Please give the program's name: ");
  char program_name[10];
  kread(program_name);

  unsigned int header_address = FS_HEADER_MEMORY_LOCATION;
  fs_header* header = get_fs_header((unsigned int *)header_address);

  int first_sector = 1 + header->number_of_boot_sectors;
  int sectors_to_read = header->number_of_file_entries * FILE_NAME_LEN / SECTOR_LEN;

  extern char _MEMORY_POLL;
  void *target_address = (void *)&_MEMORY_POLL;

  load_disk_into_memory(first_sector, sectors_to_read, target_address);

  int program_binary_index = -1;
  for(int i = 0; i < header->number_of_file_entries; i++) {
    char *file_name = target_address + i * FILE_NAME_LEN;
    if(!file_name[0])
      break;

    if(!strcmp(file_name, program_name)) {
      program_binary_index = i;
      break;
    }
  }

  if(program_binary_index == -1) {
    kwrite("Program not found.\n");
    return;
  }

  int bin_sector_location = first_sector + sectors_to_read + header->max_file_size * program_binary_index -1;
  int memory_offset = header->number_of_file_entries * FILE_NAME_LEN - (sectors_to_read-1) * 512;
  void *user_program_load_address = (void *)USER_PRG_LOAD_ADDR;
  void *memory_location = user_program_load_address - memory_offset;

  load_disk_into_memory(bin_sector_location, header->max_file_size, memory_location);

  execute_program(memory_location);
}

void f_list() {
  unsigned int header_address = FS_HEADER_MEMORY_LOCATION;
  fs_header* header = get_fs_header((unsigned int *)header_address);

  int first_sector = 1 + header->number_of_boot_sectors;
  int sectors_to_read = header->number_of_file_entries * FILE_NAME_LEN / SECTOR_LEN;

  extern char _MEMORY_POLL;
  void *target_address = (void *)&_MEMORY_POLL;

  load_disk_into_memory(first_sector, sectors_to_read, target_address);

  kwrite("Available files:\n");
  for(int i = 0; i < header->number_of_file_entries; i++) {
    char *file_name = target_address + i * FILE_NAME_LEN;
    if(!file_name[0])
      break;
    
    kwrite("  ");
    kwrite(file_name);
    kwrite("\n");
  }
}