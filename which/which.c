/*
    which v2.0 -- print full path of executables
    Copyright (C) 1999  Carlo Wood <carlo@runaway.xs4all.nl>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 1, or (at your option)
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <libiberty.h>
#include "bash.h"

static void print_usage(const char *progname)
{
  fprintf(stderr, "Usage: %s [options] [--] programname [...]\n", progname);
  fprintf(stderr, "Options: --version    Print version and exit successfully.\n");
  fprintf(stderr, "         --skip-dot   Skip directories in PATH that start with a dot.\n");
  fprintf(stderr, "         --skip-tilde Skip directories in PATH that start with a tilde.\n");
  fprintf(stderr, "         --show-dot   Don't expand a dot to current directory in output.\n");
}

static void print_version(void)
{
  fprintf(stdout, "GNU which v2.0 -- by Carlo Wood\n");
}

static void print_fail(const char *progname, const char *name, const char *path_list)
{
  fprintf(stderr, "%s: no %s in (%s)\n", progname, name, path_list);
}

enum opts {
  opt_version,
  opt_skip_dot,
  opt_skip_tilde,
  opt_show_dot
};

int skip_dot = 0, skip_tilde = 0;

int main(int argc, char *argv[])
{
  const char *progname = argv[0];
  const char *path_list = getenv ("PATH");
  char cwd[256];
  int option, fail_count = 0, show_dot = 0;
  struct option longopts[] = {
  	{ "version", 0, &option, opt_version },
	{ "skip-dot", 0, &option, opt_skip_dot },
	{ "skip-tilde", 0, &option, opt_skip_tilde },
	{ "show-dot", 0, &option, opt_show_dot },
	{ NULL, 0, NULL, 0 } };

  while(!getopt_long(argc, argv, "", longopts, NULL))
  {
    switch(option)
    {
      case opt_version:
        print_version();
	return 0;
      case opt_skip_dot:
        skip_dot = 1;
        break;
      case opt_skip_tilde:
        skip_tilde = 1;
        break;
      case opt_show_dot:
        show_dot = 1;
        break;
    }
  }

  if (!show_dot)
  {
    if (!getcwd(cwd, sizeof(cwd)))
    {
      const char *pwd = getenv("PWD");
      if (!pwd)
	strcpy(cwd, ".");
      else
	strcpy(cwd, pwd);
    }
  }

  argv += optind;

  if (!*argv)
  {
    print_usage(progname);
    return -1;
  }

  for (; *argv; ++argv)
  {
    const char *result = NULL;
    if (path_list && *path_list != '\0')
    {
      result = find_user_command_in_path(*argv, path_list, FS_EXECABLE);
      if (result)
      {
        if (!show_dot && *result == '.')
	{
	  ++result;
	  fprintf(stdout, "%s", cwd);
	}
	fprintf(stdout, "%s\n", result);
      }
    }
    if (!result)
    {
      print_fail(progname, *argv, path_list);
      ++fail_count;
    }
  }

  return fail_count;
}