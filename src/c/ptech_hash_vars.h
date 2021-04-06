#include <uthash.h>
#include <string.h>

typedef struct PtechVar {
  char key[MAX_VARNAME_SIZE];
  char value[MAX_VALUE_SIZE];
  UT_hash_handle hh;
} PtechVar;

/* le hash */
PtechVar * allvars = NULL;

/* dereferences a variable */
PtechVar * ptech_dref_var(const char * k)
{
  PtechVar * s;
  HASH_FIND_STR(allvars, k, s);
  return s;
}

/* defines and initializes a variable */
void ptech_add_var(const char * k, const char * v)
{
  PtechVar * s;
  HASH_FIND_STR(allvars, k, s);
  if (s == NULL) { /* if shadowing an existing variable */
    s = (PtechVar *)malloc(sizeof *s);
    strcpy(s->key, k);
    HASH_ADD_STR(allvars, key, s);
  }
  strcpy(s->value, v);
}
