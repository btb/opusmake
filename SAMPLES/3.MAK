#
#			Example Makefile 3
#
#		Using Make to drive an edit-compile loop
#

# Problem:
#	Use the BRIEF editor to find errors in our C files, but only when
#	the compile fails. If calling Make from inside of BRIEF, we want
#	to ensure we don't do recursive calls to BRIEF.
#
# Solution:
# 	The supplied "make.m" BRIEF macro calls Make and defines the
# 	command-line macro named "INBRIEF." By checking for this macro
#	we can tell if Make was invoked from the command line or by BRIEF.
#	Use the %if directive to contruct two difference "%.obj : %.c"
#	rules depending on whether Make is called from BRIEF or not.

#
# Inference rule to compile .c files to .obj files
#
%.obj : %.c
%if defined(INBRIEF)				# Read-time contional
	$(CC) $(CFLAGS) -c $(.SOURCE)
%else
	%while 1				# Loop until explicitly broken

		# Compile, ignore errors and redirect into an error file
		#
		--cl $(CFLAGS) -c $(.SOURCE) > $*.err 2>&1


		# If compile succeeds (the status is 0), break out of the loop
		#
		%if $(status) == 0		# A run-time conditional
			%break
		%endif

	
		# Otherwise, use BRIEF to edit the source file. After BRIEF
		# exits check the time of the source file against the time of
		# the error file. If the source file is older, it wasn't
		# editted and we don't bother restarting the edit loop and
		# we have Make produce an error message.
		#
		b $(.SOURCE) -mnext_error		
		%if %time($(.SOURCE)) < %time($*.err)
			@erase $*.err
			%error Compile of $(.SOURCE) failed
		%endif

	%end
	@erase $*.err
%endif
