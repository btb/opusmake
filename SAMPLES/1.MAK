#
#			Example Makefile 1
#
#		Build a response file manually
#

# Problem:
# 	The command line for TLINK is too long, so generate a response file.
#
# Solution:
# 	An inline response file is used to build the response file at run time.
#	See also the automatic response file support, Page 122.

OBJS	= 1.obj 2.obj 3.obj
EXE	= foo.exe
MODEL	= S
LINK	= tlink

$(EXE) : $(OBJS)
	$(LINK) @<<
$(.SOURCES,W+\n)
$(.TARGET);
<<
