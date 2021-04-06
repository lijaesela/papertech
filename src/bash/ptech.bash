#!/usr/bin/env bash

# WARNING: the usage and error handling of this script differ from the c implementation. I will fix this  l a t e r

#
# hyper-minimal macro language
# concatenates multiple files if given
#

set -e

# CONFIG:

escapechar="\\"
verbose="false"

# -- UTILS --

# print to stderr
err () {
	echo "$@" >&2
}

# verbose output
if [ "$verbose" = "true" ]; then
	vb () {
		echo "$@" >&2
	}
else
	vb () {
		true
	}
fi

# overengineered purely aesthetic change for fake enums
enum () {
	_prefix="$1"
	shift

	while [ -n "$*" ]; do

		if [ "$2" = "-" ]; then
			if [ "$_val" ]; then
				_val=$((_val + 1))
			else
				_val=0
			fi
		else
			_val="$2"
		fi

		eval "${_prefix}_${1}=\"\$_val\""
		shift 2
	done

	unset _prefix _val
}

# -- MAIN PROGRAM --

# exit before anything else if input files are not readable
for file in $@; do
	if [ ! -r "$file" ]; then
		err "failed to read one or more inputs"
		exit 1
	fi
done

# parser states

# normal state
# dereferencing a variable
# inside a declare block
# defining a variable

enum p \
	NORM - \
	VAR  - \
	DECL - \
	DVAR -

# read file into array
mapfile -t -O 1 buffer < "$file"

# current state of parser
pstate=$p_NORM

# buffer to store variables in
varbuf=""

# parser
for (( i=1; i<=${#buffer[@]}; i++ )); do
	vb "loop beginning for line $i"

	# go in and out of declare state accordingly
	if [ "${buffer[i]}" = "$escapechar" ]; then
		if [ "$pstate" = "$p_NORM" ]; then
			pstate=$p_DECL
			vb "switching to DECL at line $i"
		else
			pstate=$p_NORM
			vb "switching to NORM at line $i"
		fi
		continue
	fi

	# comments in declare blocks
	if [ "$pstate" = "$p_DECL" ]; then
		if [[ ! ${buffer[i]} == *"$escapechar"* ]]; then
			vb "line $i is a comment, skipping"
			continue
		fi
	fi

	# iterate through the line to dereference variables
	vb "iterating through ${#buffer[i]} characters in pstate $pstate"
	for (( j=0; j<${#buffer[i]}; j++ )); do

		case "$pstate" in
			# pass characters along, go into VAR state once macro character is hit
			$p_NORM)
				case "${buffer[i]:$j:1}" in
					"$escapechar")
						pstate=$p_VAR
						vb "switching to VAR at $i $j"
						;;
					*) echo -n "${buffer[i]:$j:1}";;
				esac
				;;
			# read the var name into buffer, and output its contents once the closing macro key is hit
			$p_VAR)
				case "${buffer[i]:$j:1}" in
					"$escapechar")
						eval "echo -n \"\$${varbuf}\""
						varbuf=""
						pstate=$p_NORM
						vb "switching to NORM at $i $j"
						;;
					*) varbuf="${varbuf}${buffer[i]:$j:1}";;
				esac
				;;
			# part of declaring variables
			$p_DECL)
				case "${buffer[i]:$j:1}" in
					"$escapechar")
						pstate=$p_DVAR
						vb "switching to DVAR at $i $j, declbuf is $declbuf"
						;;
					" ") true;; # ignore optional spaces in declarations
					*) declbuf="${declbuf}${buffer[i]:$j:1}";;
				esac
				;;
			# another part of declaring variables
			$p_DVAR)
				case "${buffer[i]:$j:1}" in
					"$escapechar")
						eval "${pfx}${declbuf}=\"\$dvarbuf\""
						dvarbuf=""
						declbuf=""
						pstate=$p_DECL
						vb "switching to DECL at $i $j, dvarbuf is $dvarbuf"
						;;
					*) dvarbuf="${dvarbuf}${buffer[i]:$j:1}";;
				esac
				;;
		esac


	done

	# add newline after each line
	[ "$pstate" = "$p_NORM" ] && echo

	vb "line $i complete"
done
