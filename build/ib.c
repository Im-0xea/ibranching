/*                                *
 *   ib (indentation branching)   *
 *                                */

/* todo:
 *   nothing somehow
 */


#define VERSION "0.2"

#define CONT_MAX 128
#define LINE_MAX 2000

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdnoreturn.h>

#include <string.h>
#include <unistd.h>
#include <getopt.h>


typedef struct line_type
{
	size_t comment;
	size_t tabs;
	char * str;
}
line_t;

typedef enum filetype
{
	gen,
	c,
	cpp,
	go,
	java
}
type;

typedef struct parser_context
{
	const type ftype;
	bool       mcom;
	size_t      gloc;
	line_t     line;
	line_t     l_line;
	size_t     tbranchc;
	size_t     tbranchs[CONT_MAX];
	size_t     nbranchc;
	size_t     nbranchs[CONT_MAX];
	size_t     ctermc;
	size_t     cterms[CONT_MAX];
	size_t     bmsgsc;
	size_t     bmsgst[CONT_MAX];
	char       bmsgss[CONT_MAX][LINE_MAX];
	size_t     fmsgsc;
	size_t     fmsgst[CONT_MAX];
	char       fmsgss[CONT_MAX][LINE_MAX];
}
context;


size_t spaces          = 0;
bool   to_stdout       = false;
bool   verbose         = false;
char   *overwrite_out  = NULL;


static void transfere_line(line_t *line, line_t *input_line)
{
	char * oth = line->str;
	line->str       = input_line->str;
	input_line->str = oth;
	line->comment   = input_line->comment;
	line->tabs      = input_line->tabs;
}

static size_t find_valid(const char *line, const char *tok)
{
	char *ptf = strchr(line, '"');
	char *ptr = strstr(line, tok);
	bool open = false;

	while (ptf && ptr)
	{
		if (open && ptf > ptr)
		{
			ptr = strstr(ptr + 1, tok);
		}
		ptf  = strchr(ptf + 1, '"');
		open = !open;
	}

	if (!ptr)
	{
		return strlen(line);
	}

	return (size_t) (ptr - line + 1);
}

static size_t find_end(const char *line, bool *mcom)
{
	size_t sline = find_valid(line, "//");
	size_t mline = find_valid(line, "/*");
	size_t cline = find_valid(line, "*/");

	if (!*mcom && mline < sline && cline == strlen(line))
	{
		*mcom = true;
	}

	if (*mcom)
	{
		if (cline < sline && cline < mline)
		{
			*mcom = false;
		}
		return 0;
	}

	return sline < mline ? sline : mline;
}

static size_t get_tabs(const char *line)
{
	return spaces == 0 ? strspn(line, "\t") : strspn(line, " ") / spaces;
}

static size_t get_spaces(const size_t tab)
{
	return spaces == 0 ? tab : tab * spaces;
}

static void strip_newline(char *str)
{
	size_t length = strlen(str);
	if (length > 0 && str[length - 1] == '\n')
	{
		str[length - 1] = '\0';
	}
}

static void strip_line(const char *in, char *str)
{
	strcpy(str, in + get_tabs(in));
	strip_newline(str);
}

static bool get_line(line_t *line, FILE *file, bool *mcom)
{
	if (!fgets(line->str, LINE_MAX, file))
	{
		line->tabs = 0;
		return false;
	}

	size_t size = strlen(line->str);

	while (line->str[size - 2] == '\\')
	{
		char apnd[LINE_MAX];
		if (!fgets(apnd, LINE_MAX, file))
		{
			break;
		}
		size += strlen(apnd);
		strcat(line->str, apnd);
	}

	bool empty = true;
	for (char* c = line->str; empty && *c != '\0'; c++)
	{
		empty &= *c <= ' ';
	}

	if (empty)
	{
		line->str[0]  = '\0';
	}
	else
	{
		line->tabs = get_tabs(line->str);
	}
	line->comment = find_end(line->str, mcom);

	return true;
}

static bool check_word(const char *line, const char *word, const char *stop)
{
	char line_cpy[LINE_MAX];
	const char *stop_cpy = NULL;
	if (stop)
	{
		stop_cpy = line_cpy + (stop - line);
	}
	strcpy(line_cpy, line);

	char *save = NULL;
	char *tok  = strtok_r(line_cpy, " \t\n", &save);

	while (tok && (!stop ||tok <= stop_cpy))
	{
		if (!strcmp(tok, word))
		{
			return true;
		}

		tok = strtok_r(NULL, " \t\n", &save);
	}

	return false;
}

static void brackinate(char **outp, const size_t tabs, const char *br, const bool nl, const bool nopre)
{
	char *out = *outp;
	size_t t = 0;

	if (nl)
	{
		while(!spaces && t < tabs)
		{
			out[t++] = '\t';
		}

		while(spaces && t < get_spaces(tabs))
		{
			out[t++] = ' ';
		}
	}
	out[t] = '\0';

	strcat(out, br);
	if (!nopre)
	{
		strcat(out, "\n");
	}

	*outp += strlen(out);
}

static void branch_check(char *out, context *cont)
{
	size_t       *tabs = &cont->l_line.tabs;
	const size_t tar   = cont->line.tabs;
	const int  dir   = tar < *tabs ? -1 : (*tabs < tar ? 1 : 0);
	const size_t dec   = dir == 1 ? 1 : 0;
	const bool   after_empty = cont->l_line.str[0] == '\0';

	if (dir == 1 || !after_empty)
	{
		if (cont->ftype == go && dir == 1)
		{
			out[0] = ' ';
		}
		else
		{
			out[0] = '\n';
		}
		++out;
	}

	while (tar != *tabs)
	{
		*tabs += (size_t) dir;

		const bool goel = (cont->ftype == go && dir == -1 && *tabs == tar && (check_word(cont->line.str, "else", cont->line.str + cont->line.comment) || cont->line.str[get_tabs(cont->line.str)] == '('));
		if (goel)
		{
			memmove(cont->line.str + 1, cont->line.str + get_tabs(cont->line.str), strlen(cont->line.str + get_tabs(cont->line.str)) + 2);
			cont->line.str[0] = ' ';
		}
		if (cont->fmsgsc && cont->fmsgst[cont->fmsgsc - 1] == *tabs - dec)
		{
			if (dir == 1)
			{
				--cont->fmsgsc;
				brackinate(&out, *tabs - dec, cont->fmsgss[cont->fmsgsc], !(cont->ftype == go), goel);
			}
		}
		if (cont->bmsgsc && cont->bmsgst[cont->bmsgsc - 1] == *tabs - dec)
		{
			if (dir == -1)
			{
				--cont->bmsgsc;
				const char *line = cont->line.str;
				if (strncmp(line + get_spaces(*tabs), "#elif", 5) && \
				    strncmp(line + get_spaces(*tabs), "#elifdef", 8) && \
				    strncmp(line + get_spaces(*tabs), "#elifndef", 9) && \
				    strncmp(line + get_spaces(*tabs), "#else", 5))
				{
					brackinate(&out, *tabs - dec, cont->bmsgss[cont->bmsgsc], true, false);
				}
			}
			continue;
		}

		if (dir == -1)
		{
			if (cont->ctermc && cont->cterms[cont->ctermc - 1] == *tabs)
			{
				--cont->ctermc;
			}
			if (cont->tbranchc && cont->tbranchs[cont->tbranchc - 1] == *tabs)
			{
				if (cont->ctermc && cont->cterms[cont->ctermc - 1] == *tabs - 1)
				{
					brackinate(&out, *tabs - dec, "},", true, false);
				}
				else
				{
					brackinate(&out, *tabs - dec, "};", true, false);
				}
				--cont->tbranchc;
				continue;
			}
			if (cont->ctermc && cont->cterms[cont->ctermc - 1] == *tabs - 1)
			{
				brackinate(&out, *tabs - dec, "},", true, false);
				continue;
			}
		}

		if (cont->nbranchc && cont->nbranchs[cont->nbranchc - 1] == *tabs - dec)
		{
			if (dir == -1)
			{
				--cont->nbranchc;
			}
			continue;
		}

		brackinate(&out, *tabs - dec, dir == 1 ? "{" : "}", !(cont->ftype == go && dir == 1), goel);

	}

	if (dir != 1 && after_empty)
	{
		out[0] = '\n';
		++out;
	}
}

static void terminate(line_t *line, const char tchar, context *cont)
{
	if (line->comment <= get_spaces(line->tabs) + 1 || line->str[line->comment - 2] == ':')
	{
		return;
	}

	if (line->str[line->comment - 2] == ';')
	{
		char log[LINE_MAX];
		strip_line(line->str, log);
		fprintf(stderr, "line is already terminated %ld: %s", cont->gloc - 1, log);
	}

	memmove(line->str + line->comment, line->str + line->comment - 1, strlen(line->str + line->comment - 2));
	line->str[line->comment - 1] = tchar;
}

static void term_check(context *cont)
{
	line_t *line = &cont->line;
	line_t *l_line = &cont->l_line;
	if ((cont->ftype == c || cont->ftype == cpp) && l_line->str[get_spaces(l_line->tabs)] == '#')
	{
		if (line->tabs > l_line->tabs)
		{
			strcpy(cont->bmsgss[cont->bmsgsc], "#endif");
			cont->bmsgst[cont->bmsgsc++] = l_line->tabs;
		}

		return;
	}
	if (line->tabs <= l_line->tabs)
	{
		if (cont->ctermc && cont->cterms[cont->ctermc - 1] <= l_line->tabs - 1)
		{
			if(line->tabs == l_line->tabs)
			{
				terminate(l_line, ',', cont);
			}

			return;
		}
		if (cont->ftype != go)
		{
			terminate(l_line, ';', cont);
		}
	}

	else if (!strchr(l_line->str, '(') && l_line->comment > get_spaces(l_line->tabs) + 1 && \
	         !check_word(l_line->str, "else", l_line->str + l_line->comment) && \
	         !check_word(l_line->str, "do"  , l_line->str + l_line->comment) && \
	         ((cont->ftype != cpp && cont->ftype != java) || !check_word(l_line->str, "try"  , l_line->str + l_line->comment)))
	{
		if (l_line->str[l_line->comment - 2] == ':')
		{
			cont->nbranchs[cont->nbranchc++] = l_line->tabs;
			return;
		}

		if (l_line->str[l_line->comment - 2] == '=' || \
		    check_word(l_line->str, "enum", l_line->str + l_line->comment))
		{
			cont->cterms[cont->ctermc++] = l_line->tabs;
		}
		if (cont->ftype != go && !check_word(l_line->str, "typedef", l_line->str + l_line->comment))
		{
			cont->tbranchs[cont->tbranchc++] = l_line->tabs;
		}
		if (cont->ftype == go && check_word(l_line->str, "import", l_line->str + l_line->comment))
		{
			strcpy(cont->fmsgss[cont->fmsgsc], "(");
			cont->fmsgst[cont->fmsgsc++] = l_line->tabs;
			strcpy(cont->bmsgss[cont->bmsgsc], ")");
			cont->bmsgst[cont->bmsgsc++] = l_line->tabs;
		}
	}
}

static void parser(FILE *out, FILE *src, const type ftype)
{
	context cont =
	{
		.ftype    = ftype,
		.tbranchc = 0,
		.nbranchc = 0,
		.ctermc   = 0,
		.bmsgsc   = 0,
		.mcom     = 0,
		.gloc     = 1
	};
	line_t *line = &cont.line;
	line_t *l_line = &cont.l_line;
	line->str   = malloc(sizeof(char) * 2000);
	l_line->str = malloc(sizeof(char) * 2000);

	get_line(l_line, src, &cont.mcom);
	while (true)
	{
		const bool cond = get_line(line, src, &cont.mcom);
		++cont.gloc;

		if (line->comment != 0 || l_line->comment != 0)
		{
			term_check(&cont);
		}
		strip_newline(l_line->str);
		fprintf(out, "%s", l_line->str);

		if (line->comment != 0 || l_line->comment != 0)
		{
			char branch_line[LINE_MAX] = "";
			branch_check(branch_line, &cont);
			if (branch_line[0] != '\0')
			{
				fputs(branch_line, out);
			}
			else
			{
				fputc('\n', out);
			}
		}
		else
		{
			fputc('\n', out);
		}

		if (!cond)
		{
			break;
		}

		transfere_line(l_line, line);
	}
}

static type modeset(const char *path)
{
	const char *dot = strrchr(path, '.');

	if (dot)
	{
		if (!strcmp(dot, ".c")   || !strcmp(dot, ".h"))   return c;
		if (!strcmp(dot, ".cpp") || !strcmp(dot, ".hpp")) return cpp;
		if (!strcmp(dot, ".go"))   return go;
		if (!strcmp(dot, ".java")) return java;
	}

	fprintf(stderr, "unable to detect filetype, disabling all language specific rules %s", path);
	return gen;
}

static void file_loader(char *path)
{
	FILE *out, *src = fopen(path, "r");

	if (!src)
	{
		perror("failed to open source");
		exit(1);
	}

	size_t in_path_length = strlen(path);
	char out_path[256] = "/tmp/dummy.unib";
	size_t out_path_length = in_path_length - 3;

	if (in_path_length < 4 || strcmp(path + out_path_length, ".ib"))
	{
		fprintf(stderr, "defined input is not a ib file %s", path);

		// Try to determine output path
		if (in_path_length < 255 - 5)
		{
			strcpy(out_path, path);
			strcat(out_path, ".unib");
		}
		else if (in_path_length < 255)
		{
			strcpy(out_path, path);
			strcat(out_path, "_");
		}
		else
		{
			fprintf(stderr, "path of input file has the maximum allowed length");
		}
		path = out_path;
	}
	else
	{
		//	at this point we know, that path ends with ".ib";
		path[out_path_length] = '\0';
	}

	if (to_stdout)
	{
		out = stdout;
	}
	else if (overwrite_out)
	{
		out = fopen(overwrite_out, "w");
	}
	else
	{
		out = fopen(path, "w");
	}

	if (!out)
	{
		perror("failed to open destination");
		exit(1);
	}

	parser(out, src, modeset(path));

	fclose(src);
	if (!to_stdout)
	{
		fclose(out);
	}
}

static void help(void)
{
	puts("Usage ib: [FILE] ...\n\n" \
	     "ib is a transpiler for languages which are not line based\n" \
	     "\n" \
	     " -h --help      -> print this page\n" \
	     " -v --verbose   -> output alot of info\n" \
	     " -V --version   -> show current version\n" \
	     " -o --output    -> overwrite output path for *ALL* defined ib files\n" \
	     " -s --spaces    -> use defined amount of spaces as indentation\n" \
	     " -t --tabs      -> turns spaces mode off *again*\n" \
	     " -S --stdout    -> output to stdout instead of file\n");
}

int main(const int argc, char **argv)
{
	const char * short_options = "hvVs:o:St";
	const struct option long_options[] =
	{
		{ "help",    no_argument,       0, 'h' },
		{ "verbose", no_argument,       0, 'v' },
		{ "version", no_argument,       0, 'V' },
		{ "spaces",  required_argument, 0, 's' },
		{ "output",  required_argument, 0, 'o' },
		{ "stdout",  no_argument,       0, 'S' },
		{ "tab",     no_argument,       0, 't' },
		{ 0, 0, 0, 0 },
	};

	while (true)
	{
		const int opt = getopt_long(argc, argv, short_options, long_options, NULL);

		if (opt == -1) break;

		switch (opt)
		{
			case -1:
				break;
			case '?':
				fprintf(stderr, "invalid option %c", opt);
				exit(1);
			case 'h':
				help();
				exit(0);
			case 'V':
				fputs("Ib version "VERSION"\n", stdout);
				exit(0);
			case 'v':
				verbose = true;
				continue;
			case 'o':
				overwrite_out = optarg;
				continue;
			case 's':
				spaces = atoi(optarg);
				continue;
			case 'S':
				to_stdout = true;
				continue;
			case 't':
				spaces = 0;
				continue;
		}
	}

	if (optind >= argc)
	{
		fprintf(stderr, "no files defined\n");
		help();
		exit(0);
	}

	while (optind < argc)
	{
		file_loader(argv[optind]);
		++optind;
	}

	exit(0);
}
