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

int __attribute__((fastcall)) strcmp(const char *s1, const char *s2)
{
  while (*s1 && *s2 && *s1 == *s2) {
    s1++;
    s2++;
  }
  return (*s1 - *s2);
}


