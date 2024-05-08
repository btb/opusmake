#
#			Example Makefile 4
#
#		Single set of source files and multiple binaries
#

# Problem:
#	Maintain 1 set of sources and 3 sets of binaries, for small, medium
#	and large compiler models.
#
# Solution:
#	Put the sources in a parent directory and make a separate subdirectory
#	for each model. Have Make change into the subdirectories and run itself
#	recursively with compiler model-specific information. Use the .MAKE
#	attribute to override a possible -n command-line flags (so we can
#	see what Make would do in each subdirectory) and use MFLAGS to transfer
#	any command-line flags to the recursive Make.
#
#	The default target "all" causes all models to be made (by default). You
#	can also give specific model(s) on the command-line to create only those
#	models. For example, "make s" makes only the small-model directory

OBJS	= 1.obj 2.obj 3.obj
CFLAGS	= -A$(MODEL)
ALLFLAGS= $(MODEL) -f ../${INPUTFILE}
SUBDIRS	= S M L


# The default target, "all". The model sub
#
all : $(SUBDIRS)


# Each model subdirectory has the same shell line. The .ALWAYS attribute forces
# Make to always make the subdirectory. Otherwise, because each subdirectory
# exists, and has no explicit sources, Make will think it is up to date.
#
# The multiple-command shell line first changes directory into the subdirectory
# with the same name as the model, then calls Make recursively. Notice the
# .PATH=.\;.. command-line macro definition. The "\;" in a multiple-command
# shell line evaluates to a literal ";". "\;" is necessary because ";" is
# the multiple-command shell line command separator.
#
$(SUBDIRS) .ALWAYS .MAKE :
	( cd $@ ; make ${ALLFLAGS} MODEL=$@ .PATH=.\;.. $@lib.lib )


# Note: The other way to forcibly make a file or subdirectory that exists is
# to make it dependent on another target that does not exist. For example:
#
# $(SUBDIRS) .MAKE : force
#	... shell line as above
#
# force :
#
# This will force all $(SUBDIRS) to be made, since "force" does not exist and
# Make will always have to make it.


# Alternative shell lines:
#
# Instead of using a multiple-command shell line, use the %chdir directive to
# do a change directory that is retained across shell lines.
#
#$(SUBDIRS) .ALWAYS .MAKE :
#	%chdir $@					# change to subdir.
#	make $(ALLFLAGS) MODEL=$@ .PATH=.;.. $@lib.lib
#	%chdir ..					# back up to parent dir.
