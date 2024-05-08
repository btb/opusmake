

			OPUS Software README
			--------------------

This README file contains the most recent information about the OPUS
MAKE package. This README is for Opus Make & MKMF v6.12.

This README contains the following information:
	1)  Information for SourceSafe 6.0 users.
	2)  Enhancements since v6.11
	3)  Bug Fixes since v6.11
	4)  Sample enhancements


==============================================================================
=                                                                            =
=               1) Information for SourceSafe 6.0 users                      =
=                                                                            =
==============================================================================

As of 11/16/98 the Opus Make version 6.12 integration with the new database
storage format in SourceSafe 6.0 has not been completed. If you are upgrading
to SourceSafe  6.0 from an earlier version of SourceSafe you have the option 
of updating the SourceSafe database. In order to keep the integration between 
Opus Make and SourceSafe we suggest that you do not upgrade the database. The 
integration with the SourceSafe database will be made available to you at no 
charge as a patch from our web site. Please periodically check with our web 
site (www.opussoftware.com). Or if you wish, please email us at 
biz@opussoftware.com and put "SourceSafe 6.0 compatibility" in the subject 
line and we will notify you as soon as the patch is available.



==============================================================================
=                                                                            =
=               2) Enhancements since v6.11                                  =
=                                                                            =
==============================================================================

   o)	New ".OPTIONS : QueueUnlimit" removes the 32-file limit from the
	shell-line queuing, enabling an arbitrary number of shell lines to be
	queued (Page 118).

   o)	New "SH" macro modifier executes a shell line and returns the text
   	result into the macro. This works the same as setting up a macro with
	a file name, %exec()ing a shell line with redirection into the file,
	then using the "@" macro modifier to read the contents of the file.

   o)	For Win95 and WinNT, better support of mixed-case file names.
	See the ".OPTIONS : TargetKeepCase" keyword (Page 118).

   o)	The .SHELL directive's new .NOOPUSERRH keyword prevents our OPUSERRH 
	"helper" utility from being called. This utility is used by Win95
	Opus Make to make up for shortcomings in COMMAND.COM but is
	unnecessary when running 4DOS.

   o)	In inline response files, \! can now be used to protect an
	initial ! character (Page 148).

   o)	Support for Microsoft NMAKE 1.62 (as shipped with MS VC++ 5.0).

   o)	Better detection of read-time indenting.

   o)	Better handling of leading/trailing whitespace around continued
	makefile lines during PM/CB & NMAKE emulation.
   
   o)	To differentiate between a drive-letter colon and a
	dependency colon, Make required a ":", ";", "<space>"
	or "<nothing>" after the colon.  This requirement has
	been relaxed and now these characters are needed only
	when the thing before the colon is a single letter.
	For example:
		D:foo
	is parsed as the single target "D:foo", but:
		all:CLEAN
	is parsed as "all depends on CLEAN".




==============================================================================
=                                                                            =
=               3) Bug Fixes since v6.11                                     =
=                                                                            =
==============================================================================


In Make and MKMF
----------------
   o)	Spaces in Visual SourceSafe files and/or subprojects were not
	handled properly.


   o)	^Zs on the end of MS-DOS makefiles were not properly handled.


   o)	On %set name<op>value lines, <op> was detected at
	"=", "+" or ":" when it should have been more selective,
	detecting "=", "+=" or ":=".


   o)	Mixing read-time %set and "=" macro definitions did
	not work well. %set macro definitions could not be
	redefined with an "=" macro definition.


   o)	The following:
		NAME = $$
		%set NAME += $$
		%echo NAME=$(NAME)
	would evaluate to
		NAME=$$ $
	rather than
		NAME=$ $
	because the "%set ...+=" macro did not expand out properly
	the existing value.


In Make Only
------------
   o)	For Win95 Make, "make -x foo" would causes a SHARE error
	when OPUSERRH.EXE tried to open "foo" for error output
	because the calling Make already had "foo" open. Now
	the calling Make closes the redirected files, calls
	OPUSERRH, then reopens the redirected files.


   o)	In automatic response files, "/<any> <any>" is now
	kept together in the response file. This is basically
	for MSVC++, where "/D" or "/I" on one line followed
	by the argument on the next line would fail in the
	compiler.


   o)	\t inside a regular expression "set" would not match
	tab. Instead it would match a literal t.


   o)	Targets defined in MAKE.INI, then showing up as:
		<target> :
	in the makefile would be "reset", their shell lines
	forgotten. Fixed so only rules are reset when they
	appear in this fashion.
		

In MKMF Only
------------
   o)	MKMF did not understand numbers that ended in "U" or "L".


   o)	MKMF did not trap divide-by-zero or modulus-by-zero errors
	when interpreting directives.


   o)	Changing the .TYPE.C regex to an expression that did not
	start with '#' did not work.


   o)	With parameterized paths, ".." relative paths did not work.


   o)	#defines longer than 4K characters would halt MKMF with an
   	"can't allocate <number greater than 4k> bytes".  This limit
	has been doubled to 8K.


During NMAKE emulation
----------------------

   o)	For the .SUFFIXES directive, NMAKE adds additional extensions
	to the end of .SUFFIXES while Opus Make added them to the
	start of .SUFFIXES. In NMAKE emulation mode Opus Make now
	acts like NMAKE.


   o)	NMAKE allows comment-only lines following a continued line
	to be ignored without stopping the continued state. That is:
		A = one \
		# an ignored line
		two
		!message A=$A
	evaluates to:
		A = one  two
	Now, so does MAKE, with NMAKE emulation.


   o)	For NMAKE emulation, environment variables modified in
	the makefile with a macro definition were not expanding
	out the right side of the macro definition. For example
		INC = xyzzy
		INCLUDE = $(INCLUDE);$(INC)
		x :
			echo %%INCLUDE%%
	would echo "<old value of INCLUDE>;$(INC)" instead of
	"<old value of INCLUDE>;xyzzy".


   o)	For NMAKE emulation, "cd xxx" was translated to a
	"%cd xxx" directive.  However, this failed with a
	compound statement "cd xxx & yyy".  Now, Opus Make
	detects the compound statement and passes it to
	the shell (as per NMAKE).




==============================================================================
=                                                                            =
4) Sample enhancments                            =
=                                                                            =
==============================================================================

   o) The SourceSafe example has been filled out, showing in detail how
      to self-extract an entire SourceSafe project (including makefiles
      and subprojects).  That is, with this example, a top-level
      "make -M <SourceSafeNameAndVer>" will entirely reconstruct the
      project on disk. The <SourceSafeNameAndVer> is a macro definition
      specifying the SourceSafe project and (optionally) its version label.
