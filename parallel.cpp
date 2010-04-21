/*
 * parallel.cpp
 *
 *  Created on: 18-Jun-2009
 *      Author: matthew
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/cdefs.h>
#include <sys/io.h>

#define base 0x378           /* printer port base address */

void
fnSetPort(int value)
{

  if (ioperm(base, 1, 1))
    return;

  outb((unsigned char) value, base);
}

