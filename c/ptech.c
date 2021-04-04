#include <stdlib.h>
#include <stdio.h>

#define ERR(msg) fprintf(stderr, "ERROR: %s\n", msg)
#define DIE exit(EXIT_FAILURE)

/* #define PTECH_DEBUG */

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

void ptech_parse(const char e, const char * s)
{
	int i = 0;
	while (s[i] != '\0') {
		if (s[i] == e)
			putchar('Q');
		else
			putchar(s[i]);
		++i;
	}
}

main(int argc, char * argv[])
{
	char esc; /* user-configured escape character */
	int argv_file_index; /* which arg is the file */

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

#ifdef PTECH_DEBUG
	printf("ORIGINAL FILE:\n%s", msg);
	printf("PTECH OUTPUT:\n");
#endif /* PTECH_DEBUG */

	ptech_parse(esc, buffer);
}
