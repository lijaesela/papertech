#include <stdarg.h>
#include <string.h>
#include <errno.h>

void err_exit(const char * fmt, ...)
{
  fprintf(stderr, "PTECH ERROR: ");

  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);

  fprintf(stderr, ": %s\n", strerror(errno));

  exit(EXIT_FAILURE);
}

const char * file_to_string(const char * path)
{
#define F2S_CORRUPT err_exit("Error reading `%s`", path)
  char * buffer = 0;
  long length;
  FILE * f = fopen (path, "rb");

  if (f == NULL) F2S_CORRUPT;

  if (fseek(f, 0, SEEK_END) < 0)               F2S_CORRUPT;
  length = ftell(f); if (length < 0)           F2S_CORRUPT;
  if (fseek(f, 0, SEEK_SET) < 0)               F2S_CORRUPT;
  buffer = malloc(length); if (buffer == NULL) F2S_CORRUPT;
  if (fread(buffer, 1, length, f) < 0)         F2S_CORRUPT;

  fclose(f);
  buffer[length] = '\0';

  return buffer;
#undef F2S_CORRUPT
}
