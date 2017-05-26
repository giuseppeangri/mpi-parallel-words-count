#include <stdio.h>
#include <glib.h>
#include <stdarg.h>

/*	Print text on the screen */

int example_printf(const char *format, ...)
{
  va_list arg;
  int done;
  va_start (arg, format);	
  done = vfprintf (stdout, format, arg);
  va_end (arg);
	return done;
}

