/* Determine the virtual memory area of a given address.  Linux version.
   Copyright (C) 2002  Bruno Haible <bruno@clisp.org>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include "stackvma.h"
#include <stdio.h>

int
sigsegv_get_vma (unsigned long address, struct vma_struct *vma)
{
  FILE *fp;
  int c;
  unsigned long start, end;
#if STACK_DIRECTION < 0
  unsigned long prev;
#endif

  /* Open the current process' maps file.  It describes one VMA per line.  */
  fp = fopen ("/proc/self/maps", "r");
  if (!fp)
    return -1;

#if STACK_DIRECTION < 0
  prev = 0;
#endif
  for (;;)
    {
      if (fscanf (fp, "%lx-%lx", &start, &end) != 2)
        break;
      while (c = getc (fp), c != EOF && c != '\n')
        continue;
      if (address >= start && address <= end - 1)
        {
          vma->start = start;
          vma->end = end;
#if STACK_DIRECTION < 0
          vma->prev_end = prev;
#else
          if (fscanf (fp, "%lx-%lx", &vma->next_start, &end) != 2)
            vma->next_start = 0;
#endif
          fclose (fp);
          return 0;
        }
#if STACK_DIRECTION < 0
      prev = end;
#endif
    }
  fclose (fp);
  return -1;
}