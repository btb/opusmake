#
#			Example Makefile 2
#
#		Making all targets and selective targets
#

# Problem:
#	We have a list of targets and we usually want to make all of them.
#	Occasionally we want to do them selectively.
#
# Solution:
#	Set up a "dummy" target called "all" that depends on the list of
#	targets. This results in an implicit iteration for each target in
#	the list. Running "make" without any command-line targets results
#	in the "all" target getting made. Running something like
#	"make 1.exe 3.exe" makes only the 1.exe and 3.exe targets.

SRCS	= 1.c 2.c 3.c 4.c			# List of sources
EXES	= $(SRCS,.c=.exe)			# Convert to list of exes.

all : $(EXES)					# Dummy target drives the make

$(EXES) : $*.c					# Each .exe depends on its .c
	$(CC) $(CFLAGS) $(.SOURCE)		# $(.SOURCE) will be the .c file



# Modified Problem:
#	Some .exe files are updated differently.
#
# Solution:
#	In this case, use inference rules to update the majority of the
#	.exe files and explicit rules for the special .exe files.
#
#%.exe : %.c					# Inference rule
#	$(CC) $(CFLAGS) $(.SOURCE)		# General shell line
#
#1.exe : 1.c					# Explicit rule for 1.exe
#	$(CC) $(CFLAGS) -DFILE1 $(.SOURCE)	#  and its shell line
#
#3.exe : 3.c					# Explicit rule for 3.exe
#	%do "%.exe : %.c" CFLAGS+=" -DFILE3"	#  its shell line calls %do
