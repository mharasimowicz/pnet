#!/bin/sh
#
# gen_errno.sh - Generate the table that maps platform-specific errno codes
#                to platform-independent errno codes.
#
# Copyright (C) 2000, 2002  Southern Storm Software, Pty Ltd.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
# gcc_errno.sh il_errno.h cc [flags] >errno_map.c

ILERRNO="$1"
shift

SRC=/tmp/gerr$$.c
trap "rm -f $SRC; exit 1" 1 2 15

# Create a dummy program that includes <errno.h> for us.
echo '#include <errno.h>' >"$SRC"
echo 'int main() { return 0; }'>>"$SRC"

# Find the real location of the error codes, as they may
# not necessarily be in <errno.h> itself.
FILE=""
ALL_LINES=`$* -E "$SRC" | grep '^# ' | \
	sed -e '1,$s/^[^"]*"\([^"]*\)".*$/\1/g'`
for f in $ALL_LINES; do
	if test -n "$f"; then
		if test -z "$FILE"; then
			CHKLINE=`grep EINVAL "$f"`
			if test -n "$CHKLINE"; then
				FILE="$f"
			fi
		fi
	fi
done
rm -f "$SRC"

# Generate the error number mapping tables.
if [ -n "$FILE" ]; then
	echo '/* This file is automatically generated by "gen_errno.sh" - do not edit */'
	echo '#include "il_errno.h"'
	echo '#include <errno.h>'
	echo '#ifdef __cplusplus'
	echo 'extern "C" {'
	echo '#endif'
	awk '/^#define[ \t]+E[A-Z0-9_]+[ \t]+[0-9]+/{
			if($1 == "#define")
		   	{
		   		print "#ifndef IL_ERRNO_" $2
				print "#define IL_ERRNO_" $2 " IL_ERRNO_EPERM"
				print "#endif"
			}
		 }' "$FILE"
	echo 'static int const errnoMapTable[] = {'
	echo '0,'
	awk 'BEGIN{err = 1}
	     /^#define[ \t]+E[A-Z0-9_]+[ \t]+[0-9]+/{
		 	errnext=$3
			if(errnext >= err)
			{
				while(errnext > err)
				{
					print "IL_ERRNO_EPERM,"
					++err
				}
				print "IL_ERRNO_" $2 ","
				++err
			}
		 }' "$FILE"
	echo '};'
	echo '#define errnoMapSize (sizeof(errnoMapTable) / sizeof(int))'
	echo 'int ILSysIOConvertErrno(int error)'
	echo '{'
	echo '	if(error >= 0 && error < errnoMapSize)'
	echo '		return errnoMapTable[error];'
	echo '	else'
	echo '		return IL_ERRNO_EPERM;'
	echo '}'
	awk '/^#define[ \t]+IL_ERRNO_E[A-Z0-9_]+[ \t]+[0-9]+/{
			if($1 == "#define")
		   	{
		   		print "#ifndef " substr($2, 10)
				print "#define " substr($2, 10) " -1"
				print "#endif"
			}
		 }' "$ILERRNO"
	echo 'static int const errnoFromMapTable[] = {'
	echo '0,'
	awk 'BEGIN{err = 1}
	     /^#define[ \t]+IL_ERRNO_E[A-Z0-9_]+[ \t]+[0-9]+/{
		 	errnext=$3
			if(errnext >= err)
			{
				while(errnext > err)
				{
					print "-1,"
					++err
				}
				print substr($2, 10) ","
				++err
			}
		 }' "$ILERRNO"
	echo '};'
	echo '#define errnoFromMapSize (sizeof(errnoFromMapTable) / sizeof(int))'
	echo 'int ILSysIOConvertFromErrno(int error)'
	echo '{'
	echo '	if(error >= 0 && error < errnoFromMapSize)'
	echo '		return errnoFromMapTable[error];'
	echo '	else'
	echo '		return -1;'
	echo '}'
	echo '#ifdef __cplusplus'
	echo '};'
	echo '#endif'
	exit 0
else
	echo "$0: cannot locate the errno.h file for the target platform"
	exit 1
fi
