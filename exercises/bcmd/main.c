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

#include "bios.h"
#include "utils.h"

#define PROMPT "$ "		/* Prompt sign.      */
#define SIZE 20			/* Read buffer size. */

char buffer[SIZE];		/* Read buffer.      */

void drawSmiley() {
  readln(buffer);
  short Xlocation = buffer[0] - 48;

  readln(buffer);
  short Ylocation = buffer[0] - 48;

  short *video = (short *) 0xb8000 + 80*Ylocation + Xlocation;
  video[0] = (Xlocation*Ylocation+1 << 8) + 2;
}

int main()
{
  clear();
  
  println  ("Boot Command 1.0");

  while (1) {
      print(PROMPT);		/* Show prompt.               */
      readln(buffer);		/* Read use input.            */

      if (buffer[0]) {	/* Execute built-in command.  */
        if (!strcmp(buffer,"smiley"))
          drawSmiley();
        else 
          println("Unkown command.");
	  }
  }
  
  return 0;

}

