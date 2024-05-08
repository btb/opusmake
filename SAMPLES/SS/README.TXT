
			Example: SS


		Support SourceSafe Self-Extracting Projects


Requirements:
    1)	Recreate an SourceSafe project tree on disk, where the layout
	of files on disk mirrors the layout of files in the project.

    2)	The project should self-extract, so you can change to the base
	directory of the project, type "make -M" and have the entire
	project recreated and rebuilt on disk.


Assumptions:
    1)	The current directory is mapped to the base directory of the
	SourceSafe project and subprojects store subdirectories.

    2)	As part of the project design, each subproject/subdirectory has its
	own makefile and the makefile in the parent directory of each
	subdirectory recurses into the subdirectory to perform the build.


Comments:
    1)	That the project self-extracts means the use of the "make
	makefile" mode of Make. This ensures each makefile is checked
	out of SourceSafe when it is needed. In order to check out
	the correct version we need the SourceSafe project to be
	specified on the "make -M" command line.
	
	The downside of the "make makefile" mode is that the rule for
	checking out the makefile must be contained in "make.ini" (or
	in a file passed to Make with the "-f" option on the command line).
	However we can conditionally check for "make -M" from inside
	of "make.ini" to ensure we get set up to checkout the makefile.


    2)	A succession of makefiles will need SourceSafe support and
	we abstract the SourceSafe support into an includable makefile,
	"ssmake.ini"



