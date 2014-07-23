#include <stdio.h>
#include <stdarg.h>

#include "umfileint.h"

static FILE *output = NULL;

static int verbose;

static const int do_debug = 1;

void switch_bug(const char *routine)
{
  error("no match in switch statement in routine; "
	"may indicate coding bug in umfile or unexpected header value");
}

void error(const char *routine)
{
  if (verbose || do_debug)
    {
      fprintf(output, "umfile: error condition detected in routine %s\n",
	      routine);
      fflush(output);
    }
  verbose = 0;
}

void error_mesg(const char *routine, const char *fmt, ...)
{
  va_list args;

  if (verbose || do_debug)
    {
      va_start(args, fmt);
      fprintf(output, "umfile: error condition detected in routine %s: ", routine);
      vfprintf(output, fmt, args);
      fprintf(output, "\n");
      va_end(args);
      fflush(output);
    }
  verbose = 0;
}

void debug(const char *fmt, ...)
{
  va_list args;

  if (do_debug)
    {
      va_start(args, fmt);
      fprintf(output, "DEBUG: ");
      vfprintf(output, fmt, args);
      fprintf(output, "\n");
      va_end(args);
      fflush(output);
    }
  verbose = 0;
}

void errorhandle_init()
{
  /* init sets verbose -- called at start of each of the interface routines --
   * then first call to error will cause a diagnostic to be printed,
   * but then unsets verbose to avoid series of knock-on messages
   */
  verbose = 1;
  if (output == NULL)
    //output = fopen("/dev/tty", "w");
    output = stdout;
}
