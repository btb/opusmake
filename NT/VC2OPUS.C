/*
 * Opus VC makefile conversion utility.
 *
 * Converts VisualC++ 4.x makefiles into makefiles optimized for Opus Make.
 *
 * Usage:
 * 	vc2opus [-queue] [-init MakefileStub] VCInput [Makefile]
 *
 * where:
 *	VCInput		- the name of the input VC++ Makefile (in the
 *			  Project directory)
 *	Makefile	- the name of the Makefile that will include the
 *			  Opus makefile generated from <VCInput>. If not
 *			  given, "makefile" is assumed.
 *	MakefileStub	- a stub makefile that is copied to <Makefile>. This
 *			  stub makefile is provided so you can make global
 *			  changes to the stub and have these changes copied
 *			  to each new <Makefile>. This argument is supplied
 *			  only if the command-line flag "-init" is given.
 *
 * Options are:
 *	-queue		- modify shell lines so they can be queued
 *
 * Note: The <Makefile> also includes <vc2opus.ini>, an initialization file
 * you can use for customizing all copies of <Makefile>.
 *
 * Note: It is also possible to handle Visual C++ 4.x makefiles without
 * conversion by using the Opus Make -EN flag. However you won't get
 * optimizations like shell-line queuing.
 *
 * Author:	Don Kneller
 *		Opus Software, Inc
 *		415-664-5670
 */
#include <stdarg.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdarg.h>

#define TRUE	1
#define FALSE	0

#define CONVERTER	"VC2OPUS Makefile Converter v1.10"

static char *_usageMsg[] = {
"\nOpus Software " CONVERTER,
"\nSyntax:",
"  vc2opus [-init MakefileStub] [-queue] VCInput [Makefile]",
"\nResult:",
"  Generate an Opus-optimized makefile from <VCInput>. The Opus-optimized",
"  makefile <OpusOutput> has the same name as <VCInput> with extension \".omk\"",
"  <OpusOutput> is itself included from <Makefile>, a helper makefile whose",
"  default name is \"makefile\".",
"\nOptions:",
" -init       Copy <MakefileStub> to <Makefile>.",
" -queue      Enable shell-line queuing in <OpusOutput>",
NULL
};

/*************/
/* Tuneables */
/*************/

/*
 * Attributes for each target added to <OpusOutput>
 */
static char	_targetAttributes[] = ".ALWAYS .SILENT .MAKE";

/*
 * Extension of <OpusOutput> file
 */
static char	_opusExt[] = ".omk";

/*
 * Default name of <Makefile>.
 */
static char	_defaultMakefileName[] = "makefile";


/*
 * !MESSAGE directive argument in <VCInput> that preceeds the Visual C++
 * configurations.
 */
static char	_configBanner[] =
			"Possible choices for configuration";

/*
 * We macroize the { } that enable shell line queuing so that we
 * can turn on and off queuing without regenerating the <OpusOutput>.
 */
#define LTBRACE	"$(_VCl)"	/* use "{" for hard-coded brace */
#define RTBRACE	"$(_VCr)"	/* use "}" for hard-coded brace */


/*****************/
/* End Tuneables */
/*****************/


/*
 * Structure for each configuration of the VC Makefile
 */
typedef struct _config CONFIG;
struct _config {
	CONFIG	*next;		/* linked to next configuration */ 
	char	name[1];	/* configuration name */
};

/*
 * Structure for the command-line options.
 */
static struct {
	int	queue;
} options;

static CONFIG	*_configList;		/* linked list of configurations */
static char	*_vcInput;		/* the VC++ input makefile name */
static char	*_projectName;		/* the VC++ project name */
static char	*_opusOutput;		/* the Opus output file name */
static char	*_makefile;		/* the Opus makefile name */


/*
 * Print the usage message, then quit.
 */
static void
_usage() {
	char **cp = _usageMsg;

	while (*cp) {
		fputs(*cp++, stdout);
		fputs("\n", stdout);
	}
	exit(1);
}

/* 
 * Produce an error message and quit.
 */
static void
_error(const char *fmt, ...)
{
	va_list	arglist;

	fputs("VC2OPUS: ", stderr);
	va_start(arglist, fmt);
	vfprintf(stderr, fmt, arglist);
	va_end(arglist);

	exit(1);
}


/*
 * Allocate memory, erroring if there is not enough memory.
 */
static char *
_emalloc(unsigned int size)
{
	char *ptr;

	if (ptr = (char *) malloc(size))
		return ptr;
	_error("Out of memory\n");
}

/* 
 * Make a copy of the string.
 */
static char *
_estrdup(char *from)
{
	if (from == NULL)
		return from;
	return strcpy(_emalloc((unsigned int) strlen(from) + 1), from);
}

/*
 * If this is a !MESSAGE directive, return its argument. Otherwise,
 * return NULL.
 */
static char *
_getMessageArgument(char *line)
{
	if (strncmp(line, "!MESSAGE", 8) == 0) {
		char	*cp = line + 8;

		while (*cp == ' ' || *cp == '\t')
			cp++;
		return cp;
	}
	return NULL;
}

/*
 * Add the configuration <arg> to the list of configurations.
 */
static void
_addConfig(const char *arg)
{
	char	buf[BUFSIZ], *bp;
	CONFIG	*cp, *last_cp;

	if (*arg++ == '"') {

		/*
		 * Truncate the configuration at the next '"'
		 */
		bp = buf;
		while (*arg && *arg != '"')
			*bp++ = *arg++;
		*bp = '\0';

		/*
		 * Go to the end of the list.
		 */
		last_cp = 0;
		for (cp = _configList; cp; cp = cp->next)
			last_cp = cp;

		/*
		 * Make a new configuration.
		 */
	 	cp = (CONFIG *) _emalloc(sizeof(CONFIG) + strlen(buf));
		if (last_cp)
			last_cp->next = cp;
		else
			_configList = cp;
		cp->next = 0;
		strcpy(cp->name, buf);
	}
}

/*
 * Return the first configuration with the string <match> in it.
 */
static CONFIG *
_findConfig(const char *match)
{
	CONFIG	*cp;

	if (match) {
		for (cp = _configList; cp; cp = cp->next) {
			if (strstr(cp->name, match))
				return cp;
		}
	}
	return NULL;
}

/*
 * In the buffer <buf> replace the first occurrence of <from> with <to>,
 * returning TRUE if <from> was found.
 */
static int
_replace(char *buf, const char *from, const char *to)
{
	char	*cp;
	char	tBuf[BUFSIZ];

	if (cp = strstr(buf, from)) {
		strcpy(tBuf, cp+strlen(from));	/* copy old tail to tBuf */
		strcpy(cp, to);			/* insert new string */
		strcpy(cp + strlen(to), tBuf);	/* append old tail */

		return TRUE;
	}
	return FALSE;
}

/*
 * Copy from the <MakefileStub> to the <Makefile>
 */
static void
_copyMakefileFromStub(const char *toName, const char *fromName)
{
	FILE	*in, *out;
	char	buf[BUFSIZ];
	char	configNoTarget[BUFSIZ];
	int	replaceConfigDefault = (_configList != NULL);
	int	replaceConfigVCDir = replaceConfigDefault;
	int	replaceProject = TRUE;
	CONFIG	*configDebug = NULL;
	CONFIG	*configRelease = NULL;

	/*
	 * The <config_no_target> place-holder in vc2opus.stb is
	 * replaced by the first remembered configuration, with
	 * the "Release" or "Debug" removed.
	 */
	if (replaceConfigDefault) {
		strcpy(configNoTarget, _configList->name);
		if (! _replace(configNoTarget, " Release", "")
		&&  ! _replace(configNoTarget, " Debug", ""))
			replaceConfigVCDir = FALSE;

		/*
		 * Find first Release and Debug configurations
		 */
 		configRelease = _findConfig("Release");
 		configDebug = _findConfig("Debug");
	}

	if (in = fopen(fromName, "r")) {
	    if (out = fopen(toName, "w")) {
		while (fgets(buf, sizeof buf, in)) {
			/*
			 * Replace the project with the known project.
			 */
		 	if (replaceProject
			&&  _replace(buf, "<project>", _projectName))
				replaceProject = FALSE;

			/*
			 * Replace the <config> place-holder with the
			 * first configuration.
			 */
		 	if (replaceConfigDefault
			&&  _replace(buf, "<cfg_default>", _configList->name))
				replaceConfigDefault = FALSE;

			/*
			 * Replace the <cfg_vcdir> place-holder
			 * with the first configuration (from which
			 * "Release" or "Debug" has been removed).
			 */
		 	else if (replaceConfigVCDir
			&&  _replace(buf, "<cfg_vcdir>", configNoTarget))
				replaceConfigVCDir = FALSE;

			else if (configDebug
			&&  _replace(buf, "<cfg_debug>", configDebug->name))
				configDebug = NULL;

			else if (configRelease
			&&  _replace(buf, "<cfg_release>", configRelease->name))
				configRelease = NULL;

			fputs(buf, out);
		}
		fclose(out);
	    }
	    else {
		_error("Can't open %s for writing\n", toName);
	    }
	    fclose(in);
	}
	else {
		_error("Can't open %s for reading\n", fromName);
	}
}

/*
 * Copy <in> to <out>, replacing and augmenting
 */
static void
_generateOutput(FILE *out, FILE *in)
{
	char	buf[BUFSIZ], *arg;
	int	configLen = strlen(_configBanner);
	int	removeInline = FALSE;

	fputs("#\n# Converted by " CONVERTER "\n#\n", out);
	while (fgets(buf, sizeof buf, in)) {

		/*
		 * We replace:
		 *	$(CPP) @<<
		 *	...
		 *	<<
		 *
		 * with:
		 *	$(CPP) \
		 *	... \
		 *	<emptyline>
		 */
		if (removeInline) {
			if (strcmp(buf, "<<\n") == 0) {
				buf[0] = '\n';
				buf[1] = '\0';
				removeInline = FALSE;
			}
			else {
				int	len = strlen(buf);

				if (len > 2 && len < sizeof(buf) - 1
				&&  buf[len - 1] == '\n'
				&&  buf[len - 2] != '\\') {
					buf[len - 1] = '\\';
					buf[len] = '\n';
					buf[len + 1] = '\0';
				}
			}
		}

		/*
		 * Gather the configurations. These are found
		 * in comment lines of the form:
		 *
		 !MESSAGE <_configBanner> ...
		 !MESSAGE 
		 !MESSAGE "Make - Win32 Release" ...
		 !MESSAGE "Make - Win32 Debug" ...
		 !MESSAGE "SSafe4 - Win32 Release" ...
		 !MESSAGE "SSafe4 - Win32 Debug" ...
		 !MESSAGE 
		 */
		if ((arg = _getMessageArgument(buf))
		&&  strncmp(arg, _configBanner, configLen) == 0) {
			int	sawConfig = FALSE;

			fputs(buf, out);
			while (fgets(buf, sizeof buf, in)) {
				fputs(buf, out);
				if (arg = _getMessageArgument(buf)) {
					if (*arg == '"') {
						_addConfig(arg);
						sawConfig = TRUE;
					}
					else if (sawConfig)
						break;
				}
			}
		}

		/*
		 * Add { and } around the source file to enable queueing.
		 */
		else if (options.queue
		&&       (buf[0] == ' ' || buf[0] == '\t')
		&&	 (_replace(buf,"$(SOURCE)",LTBRACE "$(SOURCE) " RTBRACE)
			  || _replace(buf, "$<", LTBRACE "$< " RTBRACE))) {
			fputs(buf, out);
		}
		else {
			/*
			 * Replace occurrences of:
			 *
			 *  o)	the project makefile name with the actual
			 *	makefile name.
		 	 */
			_replace(buf, _vcInput, _makefile);

		 	/*
			 *  o)	Replace occurrences of /$(MAKEFLAGS) with
			 *	$(MFLAGS). On these lines also replace /F
			 *	with /f
			 */
			if (_replace(buf, "/$(MAKEFLAGS)", "$(MFLAGS)"))
				_replace(buf, "/F", "/f");

			/*
			 *  o)	To support queuing of shell lines, replace
			 *	occurrences of $(CPP) @<< with $(CPP) \
			 *	and note that we are removing this inline
			 *	response.
			 */
			if (options.queue && removeInline == FALSE
			&& _replace(buf, "$(CPP) @<<", "$(CPP) \\"))
				removeInline = TRUE;

			fputs(buf, out);
		}
	}

	if (_configList == NULL)
		_error("No configurations found in VCInput!\n");

}

main(int ac, char **av)
{
	FILE	*fvc, *fopus;
	char	*cp;
	char	*makefileStub = NULL;


	/*
	 * Process the options
	 */
	for (++av, --ac; *av && (*av[0] == '-' || *av[0] == '/'); av++, ac--) {
		if (strcmp(*av + 1, "queue") == 0)
			options.queue++;
		else if (strcmp(*av + 1, "init") == 0) {
			if (av[1]) {
				makefileStub = *++av;
				ac--;
			}
		}
	}

	/*
	 * There can be one or two arguments left.
	 */
	if (! (ac == 1 || ac == 2))
		_usage();

	/*
	 * First argument is VC++ makefile name.
	 */
 	_vcInput = _estrdup(av[0]);

	/*
	 * Strip extension from _vcInput name to get project name.
	 */
 	_projectName = _estrdup(_vcInput);
	if (cp = strrchr(_projectName, '.'))
		*cp = '\0';

	/*
	 * Add <_opusExt> to get the output file name
	 */
 	_opusOutput = _emalloc(strlen(_projectName) + strlen(_opusExt) + 1);
	strcat(strcpy(_opusOutput, _projectName), _opusExt);

	/*
	 * Second argument (if given) is including makefile name.
	 */
	if (ac == 2)
		_makefile = av[2];
	else
		_makefile = _defaultMakefileName;


	/*
	 * Open the files for reading and writing.
	 */
	if ((fvc = fopen(_vcInput, "r")) == NULL)
		_error("Can't open %s for reading\n", _vcInput);

	if ((fopus = fopen(_opusOutput, "w")) == NULL)
		_error("Can't open %s for writing\n", _opusOutput);

	_generateOutput(fopus, fvc);
	fclose(fopus);
	fclose(fvc);


	/*
	 * If initializing, copy the stub makefile into place.
	 */
 	if (makefileStub)
		_copyMakefileFromStub(_makefile, makefileStub);

	exit(0);
}
