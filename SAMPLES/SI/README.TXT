Example: SI

Opus Make can process Source Integrity project and sandbox files. This
example shows one way to have Opus Make support Source Integrity.

Project Support
---------------

The assumptions for this SI support are the following:

   1)	The project file (usually "project.pj") defines source files
	that may have paths on them.

   2)	The source files have Archive files in <SourceDirectory>/RCS.

   3)	The source files get checked out to becoming Working files in
	the current directory.

   4)	Source Integrity gets tolds where the Archive files reside for
   	local Working files.

	To set up Source Integrity, use the Project Menu / Project
	Information... dialog. Create a Variable called "rcspath" with value:

	   	<SourceDirectory>/RCS

	If there is more than one <SourceDirectory> the Value should be:

		<SourceDirectory-1>/RCS;<SourceDirectory-2>/RCS[; ...]

   5)	An SI_PROJECT macro is defined by you to name the project file. See
   	"makefile" for an example.

   6)	An SI_SRC_TYPES macro can be defined by you to select source files
	from the project. For example, you can define SI_SRC_TYPES as
	"*.c *.cpp" to tell Make to use the project's "*.c" and "*.cpp" files
	as source files. If you don't define SI_SRC_TYPES, the "si.mak" file
	defines it as "*.c *.cpp".


The includable "si.mak" file sets up:

    o)	The SI_FILES macro: the list of all files in the project.

    o)	The SRCS macro: the list of source files in the project.
    
    o)	The OBJS macro: same list as SRCS, with the file extensions replaced
    	by .obj.

    o)	The .RCS_STORAGE directives to allow Make to create local Working
    	files from the Archive directories.


Sandbox Support
---------------

To use this "si.mak" file with a sandbox all you need to do is set
the SI_PROJECT macro to the sandbox name. See "sbox\makefile" for
an example.
