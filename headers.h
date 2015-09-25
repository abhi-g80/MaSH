# include <iostream>
# include <fstream>
# include <sys/stat.h>
# include <cerrno>
# include <unistd.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <cstring>
# include <cstdio>
# include <cstdlib>
# include <signal.h>
# include <termios.h>
# include <fcntl.h>
# include <sys/utsname.h>
# include <readline/readline.h>
# include <readline/history.h>

# define MAX 1024
# define PIPE_FOUND 1
# define REDIRECT_FOUND 2
# define EXIT -512

using namespace std ;

static const char *shell_version = {"MaSH, version 0.4.1(1)-release (x86_64-pc-linux)\nCopyright (C) 2014 Free Software Foundation, Inc. \nLicense GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\n\nThis is free software; you are free to change and redistribute it.\nThere is NO WARRANTY, to the extent permitted by law."} ;

static const char *builtins[5] = {"exit", "cd", "version", "history", "math"} ;

void change_directory (char **) ;

void pipe_command (char **, char **) ;

void init_shell () ;

void redirect_command (char **, char **) ;

int parse_cmd (char *, char **, char **, char **) ;

int run_cmd (char **) ;

int exit_mash () ;

int match (char *) ;

int run_builtins (char **) ;

char * trim_space (char *) ;

char * replace (char *, char) ;

void show_history (char **) ;

void history_command (char *) ;

void run_math_cmd (char **) ;

void set_init_dir () ;

void password () ;

void print_welcome () ;

static void sigchld_hdl (int sig) ;
