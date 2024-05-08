#
#			Example Makefile 5
#
#
#		Produce Normal and CodeView executables
#

# Problem:
#	Maintain normally- and CodeView-compiled .obj and .exe files to
#	minimize the time we spent recompiling when tracking a bug.
#
# Solution:
#	Similar to example 4, this example uses VPATH (which is similar to
#	.PATH, but implicitly includes the current directory). The .ALWAYS
#	and .MAKE attributes are as in example 4. The new feature is the
#	use of conditional reading of the makefile to set several macros.
#
#	The "normal" subdirectory is made by default. Executing the command
#	"make cv" causes Make to make the "cv" directory, which contains
#	the CodeView version of the executable.


# Macros
#
OBJS	= 1.obj 2.obj 3.obj
EXE	= foo.exe
ALLFLAGS= $(MFLAGS) -f ../$(INPUTFILE)


# Use conditionals to set CFLAGS and LDFLAGS
#
%if defined(CV)
	CFLAGS	= -Od -Zi
	LDFLAGS	= /CO
%else
	CFLAGS	= -Os -W3
	LDFLAGS	=
%endif


# The default target.
#
normal .ALWAYS .MAKE :
	( cd $@ ; make $(ALLFLAGS) VPATH=.. $(EXE) )


cv .ALWAYS .MAKE :
	( cd $@ ; make $(ALLFLAGS) VPATH=.. CV= $(EXE) )


$(EXE) : $(OBJS)
	link $(.SOURCES), $(.TARGET) $(LDFLAGS);



# Alternative directory layout.
#
# Keep the sources and normal .obj and normal .exe file in the current
# directory and only use a single "cv" subdirectory for CodeView .obj and
# .exe files.
#
#
# The default target
#
# $(EXE) : $(OBJS)
#	link $(.SOURCES), $(.TARGET) $(LDFLAGS);
#
# cv .ALWAYS .MAKE :
#	( cd $@ ; make $(ALLFLAGS) VPATH=.. CV= )
