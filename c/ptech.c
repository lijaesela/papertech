#include <uthash.h>
#include <stdlib.h>
#include <stdio.h>

#define ERR(msg) fprintf(stderr, "ERROR: %s\n", msg)
#define DIE exit(EXIT_FAILURE)

/* BLOCK_DELIM checks if a character in a string is an escape character alone on a line.
 * this is surprisingly complex as this escape char will be surrounded by newlines
 * unless it is at the very beginning or end of the file (which will often be the case!).
 * if we're calling this we already know that s[i] is an escape character
 * - takes (string, iterator)
 */
#define BLOCK_DELIM(s, i) (i == 0 && s[i+1] == '\n') || (s[i-1] == '\n' && (s[i+1] == '\n' || s[i+1] == '\0'))

#define MAX_VARNAME_SIZE 999
#define MAX_VALUE_SIZE 999

typedef struct PtechVar {
  char key[MAX_VARNAME_SIZE];
  char value[MAX_VALUE_SIZE];
  UT_hash_handle hh;
} PtechVar;

PtechVar * allvars = NULL;

void print_help(void);
const char * file_to_string(const char * path);
void ptech_parse(const char esc, const char * in);
PtechVar * ptech_dref_var(const char * k);
void ptech_add_var(const char * k, const char * v);

main(int argc, char * argv[])
{
  char esc; /* user-configured escape character */
  int argv_file_index; /* which arg is the file */
  PtechVar * s;

  if (argc < 2) { /* if no args */
    print_help();
    return EXIT_SUCCESS;
  } else if (argc < 3) { /* if only file is given */
    esc = '\\';
    argv_file_index = 1;
  } else { /* if both escapechar and file are given */
    esc = argv[1][0];
    argv_file_index = 2;
  }

  const char * buffer = file_to_string(argv[argv_file_index]);

  ptech_parse(esc, buffer);
}

PtechVar * ptech_dref_var(const char * k)
{
  PtechVar * s;
  HASH_FIND_STR(allvars, k, s);
  return s;
}

void ptech_add_var(const char * k, const char * v)
{
  /* TODO: check if key already exists and shadow existing variable */
  PtechVar * s;
  s = malloc(sizeof(PtechVar));
  strcpy(s->key, k);
  strcpy(s->value, v);
  HASH_ADD_STR(allvars, key, s);
}

void ptech_parse(const char esc, const char * in)
{
  enum States {
    Norm,
    Dref,
    Blok,
    Decl,
  };
  int i = 0;
  int ii = 0;
  short pstate = Norm;
  /* buffer used for both dereferencing and setting variables. always holds a variable name. */
  char buf[MAX_VARNAME_SIZE];
  /* buffer used to hold contents of variable being declared */
  char contents[MAX_VALUE_SIZE];
  PtechVar * v;

  while (in[i] != '\0') {
    switch (pstate) {
      case Norm: /* spitting input back out */
        if (in[i] == esc) {
          if (BLOCK_DELIM(in, i))
            pstate = Blok;
          else
            pstate = Dref;
        } else
          putchar(in[i]);
        break;
      case Dref: /* dereferencing variables */
        if (in[i] == esc) {
          buf[ii] = '\0';
          v = ptech_dref_var(buf);
          if (v == NULL) {
            printf("UNDEFINED");
          } else {
            printf("%s", v->value);
          }
          ii = 0;
          pstate = Norm;
        } else {
          buf[ii] = in[i];
          ++ii;
        }
        break;
      case Blok: /* in comment or declaration */
        if (in[i] == esc) {
          if (BLOCK_DELIM(in, i)) {
            ++i;
            pstate = Norm;
          } else {
            /* top buf off for Decl mode */
            buf[ii] = '\0';
            pstate = Decl;
          }
          ii = 0;
          break;
        }
        switch (in[i]) {
          /* ignore whitespace */
          case ' ':
            break;
          case '\t':
            break;
          case '\n':
            ii = 0;
            break;
          default:
            buf[ii] = in[i];
            ++ii;
            break;
        }
        break;
      case Decl: /* initializing a variable */
        if (in[i] == esc) {
          contents[ii] = '\0';
          ii = 0;
          ptech_add_var(buf, contents);
          pstate = Blok;
        } else {
          contents[ii] = in[i];
          ++ii;
        }
        break;
    }
    ++i;
  }
}

void print_help(void)
{
  printf(
      "usage: ptech <escape character> <file>\n"
      "or:    ptech <file>\n"
      "The latter defaults to '\\' as an escape.\n"
      );
}

const char * file_to_string(const char * path)
{
  char * buffer = 0;
  long length;
  FILE * f = fopen (path, "rb");

  if (f == NULL) {
    ERR("Null file pointer. File does not exist or is not readable.");
    DIE;
  }
  
  /* TODO: more errors */
  fseek (f, 0, SEEK_END);
  length = ftell (f);
  fseek (f, 0, SEEK_SET);
  buffer = malloc (length);
  fread (buffer, 1, length, f);
  fclose (f);
  buffer[length] = '\0';

  return buffer;
}

/* useless now */
int string_to_int(const char * name)
{
  int i = 0,
      ret = 0;
  while (name[i] != '\0') {
    ret = ret + (int) name[i];
    ++i;
  }
  return ret;
}
