#include <stdlib.h>
#include <stdio.h>

#define ESC '\\'

/* #define PANIC(...) fprintf(stderr, __VA_ARGS__); exit(EXIT_FAILURE) */
#define PANIC \
	fprintf(stderr, "ERROR: %s\n", strerror(errno)); \
	exit(EXIT_FAILURE);

enum states {
	NORM,
	DVAR,
	BLOK,
};

const char * getFile(const char *);
void ptech_parse(const char *);
void pHelp(void);

main(int argc, char * argv[])
{
	const char * msg = getFile(argv[1]);
	ptech_parse(msg);
	if (argc < 2) {
		pHelp();
		return EXIT_SUCCESS;
	}


	return EXIT_SUCCESS;
}

const char * getFile(const char * name)
{
	char * buffer = 0;
	long length;
	FILE * f = fopen (name, "rb");
	if (f == NULL) PANIC;
	
	/* TODO: more error handling */
	fseek (f, 0, SEEK_END);
	length = ftell (f);
	fseek (f, 0, SEEK_SET);
	buffer = malloc (length);
	fread (buffer, 1, length, f);
	fclose (f);
	buffer[length] = '\0';

	return buffer;
}

void ptech_parse(const char * in) {
	int i = 0,
		lines = 0,
		pstate = NORM;

	while (in[i] != '\0') {
		switch (pstate) {
			case NORM:
				if (in[i] == ESC)
					pstate = DVAR;
				else
					putchar(in[i]);
				break;
			case DVAR:
				if (in[i] == ESC)
					pstate = NORM;
				break;
		}
		++i;
	}
}

void pHelp(void) {
	printf(
			"usage: ptech <file>\n"
		  );
}
