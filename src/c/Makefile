# requires 'uthash', a header file available on most distros

CFLAGS=--std=c90

make:
	cc $(CFLAGS) ptech.c -o ptech

fmt: ptech.c ptech_file_io.h ptech_hash_vars.h config.h
	astyle --project=.astylerc ptech.c ptech_file_io.h ptech_hash_vars.h config.h
