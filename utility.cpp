# include "headers.h"

//static const char *history_file = {"/home/abhishek/Documents/project CS101/mash_test/logs.txt"} ;


char history_file[MAX] ;
char init_directory[MAX] ;

void change_directory (char **argv)
{
	if (argv[1] == NULL) 
                chdir (getenv ("HOME"));
	else 
	{
                if (chdir (argv[1]) == -1) 
                        perror ("\033[31mError: \033[0m") ;
        }
}

int exit_mash ()
{
	fprintf (stdout, "\033[1;36mLogging out\033[0m\n") ;
	return EXIT ;
}

void set_init_dir ()
{
	getcwd (init_directory, sizeof (init_directory)) ;
	return ;
}

int match (char *cmd)
{	
	for (int i = 0; i < 5; i++)
		if (strcmp (builtins[i], cmd) == 0)
			return 1 ;
	
	return 0 ;
}

void run_math_cmd (char **argcv)
{
	char dir[5000] ;

	pid_t child_pid ;
	int child_status ;

	strcpy (dir, init_directory) ;

	strcat (dir, "/bin/") ;

	if (argcv[1] == NULL)
	{
		cerr << "\033[31mError : No command given for Math\033[0m" << endl ;
		return ;
	}
	strcat (dir, argcv[1]) ;

	strcat (dir, ".o") ;

	char *cmd[] = {dir, (char *)0 } ;

	child_pid = fork () ;

	if (child_pid == 0)
	{
		if (execv (dir, cmd) < 0)
		{
			cerr << "\033[31mError : Command not found\033[0m" << endl ;
			exit (1) ;
		}
	}
	else
		while (wait (&child_status) != child_pid) ;

	return ;
}

int run_builtins (char **argcv)
{
	if (strcmp (argcv[0], builtins[0]) == 0)
		return exit_mash () ;
	else if (strcmp (argcv[0], builtins[1]) == 0)
	{
		change_directory (argcv) ;
		return 1 ;
	}
	else if (strcmp (argcv[0], builtins[2]) == 0)
	{
		fprintf (stdout, "%s\n", shell_version) ;
		return 1 ;
	}
	else if (strcmp (argcv[0], builtins[3]) == 0)
	{
		show_history (argcv) ;
		return 1 ;
	}
	else if (strcmp (argcv[0], builtins[4]) == 0)
	{
		run_math_cmd (argcv) ;
		return 1 ;
	}
}

void create_history_dir ()
{
	history_file[0] = '\0' ;
	strcpy (history_file, init_directory) ;
	strcat (history_file, "/logs.txt") ;
}


void history_command (char *cmd)
{
	FILE *fp ;

	create_history_dir () ;
	
	fp = fopen (history_file, "a") ;

	if (fp == NULL)
	{
		perror ("\033[31mError\033[0m") ;
		return ;
	}
	
	fprintf (fp, "%s\n", cmd) ;
	
	fclose (fp) ;
}

void show_history (char **argcv)
{
	FILE *fp ;
	char c ;

	create_history_dir () ;

	fp = fopen (history_file, "r") ;

	if (fp == NULL)
	{
		perror ("\033[31mError\033[0m") ;
		return ;
	}

	if (argcv[1] == NULL)
	{
		while ( (c = getc (fp)) != EOF)
			fputc (c, stdout) ;
	}
	else
		if (strcmp (argcv[1], "-c") == 0)
		{
			if ( remove (history_file) != 0)
			{
				perror ("\033[31mError\033[0m") ;
				return ;
			}
		}
}

/**
This init_shell() has been taken from GNU Job Control page
**/

void init_shell ()
{
	pid_t shell_pgid;
	struct termios shell_tmodes;
	int shell_terminal;
	int shell_is_interactive;

	shell_terminal = STDIN_FILENO;
	shell_is_interactive = isatty (shell_terminal);

	if (shell_is_interactive)
	{
      		while (tcgetpgrp (shell_terminal) != (shell_pgid = getpgrp ()))
        		kill (- shell_pgid, SIGTTIN);

      		signal (SIGINT, SIG_IGN);
      		signal (SIGQUIT, SIG_IGN);
      		signal (SIGTSTP, SIG_IGN);
      		signal (SIGTTIN, SIG_IGN);
      		signal (SIGTTOU, SIG_IGN);
      		signal (SIGCHLD, sigchld_hdl);

      		shell_pgid = getpid ();
      		if (setpgid (shell_pgid, shell_pgid) < 0)
        	{
          		perror ("\033[31mCouldn't put the shell in its own process group \033[0m") ;
          		exit (1);
        	}

      		tcsetpgrp (shell_terminal, shell_pgid);

      		tcgetattr (shell_terminal, &shell_tmodes);
    	}
	else
	{
		perror ("\033[31Couldn't make shell interactive \033[0m") ;
		exit (1) ;
	}	
}

char * replace (char * str, char s)
{
	int l = strlen (str), j = 0 ;

	for (int i = 0; i < l; i++)
		if (str[i] != s)
			str[j++] = str[i] ;

	str[j] = '\0' ;

	return str ;
}

int parse_cmd (char *argument, char **argv, char **cmd1, char **cmd2)
{
	char delims[] = " " ;
	int argc = 0, pipe = 0, redirect = 0, split = 0, end = 0 ;

	argv[argc] = strtok (argument, delims) ;

	while (argv[argc] != NULL)
		argv[++argc] = strtok (NULL, delims) ;
 
	argv[argc] = NULL ;

	for (int i = 0; i < argc; i++)
		if (argv[i][0] == '$')
		{
			char * temp = getenv (replace (argv[i], '$')) ;	
			if (temp != NULL)
				argv[i] = temp ;
		}

	int count = 0 ;

	for (int i = 0; i < argc; i++)
	{
		if (strcmp (argv[i], "|") == 0)
		{
			split = i ;
			pipe = PIPE_FOUND ;
			count++ ;
		}
		else if (strcmp (argv[i], ">>") == 0)
		{
			split = i ;
			redirect = REDIRECT_FOUND ;
		}
	}

	if (pipe == PIPE_FOUND && count > 1)
		return -4 ;

	if (pipe == PIPE_FOUND && redirect == REDIRECT_FOUND)
		return -1 ;

	if (pipe == PIPE_FOUND || redirect == REDIRECT_FOUND)
	{
		for (int i = 0; i < split; i++)
			cmd1[i] = argv[i] ;

		for (int i = split + 1; i < argc; i++)
			cmd2[end++] = argv[i] ;
		
		cmd1[split] = NULL ;
		cmd2[end] = NULL ;
	}

	if (pipe == 1)
		return 1 ;
	else if (redirect == 2)
		return 2 ;
	else
		return 0 ;
}
	

int run_cmd (char **argv)
{
	pid_t child_pid, tpid;
	int child_status;

	child_pid = fork ();
	
	if (child_pid < 0)
	{
		perror ("\033[31mError 'fork()' \033[0m") ;
		exit (1) ;
	}
	
	else if (child_pid == 0)
	{
    		if (execvp (*argv, argv) < 0)
		{
			perror ("\033[31mError 'execvp()' \033[0m") ;
			exit (1) ;
		}
  	}

  	else
		while (wait (&child_status) != child_pid) ;

     return child_status;
}

void pipe_command (char **cmd1, char **cmd2)
{
	int fds[2] ;
  	pipe (fds) ;
  	pid_t pid ;

  	if (fork () == 0) 
	{
    		dup2 (fds[0], 0) ;
    		close (fds[1]) ;

    		if (execvp (cmd2[0], cmd2) < 0)
    			perror ("\033[1;31mError 'execvp()' \033[0m") ;
  	} 
	
	else if ((pid = fork()) == 0) 
	{
    		dup2 (fds[1], 1) ;
    		close (fds[0]) ;

	    	if (execvp (cmd1[0], cmd1) < 0)
    			perror ("\033[1;31mError 'execvp()' \033[0m") ;
  	}

	else
    		waitpid (pid, NULL, 0) ;
}

void redirect_command (char **cmd, char **file)
{
	cerr << "\033[33mRedirection has not being implemented\033[0m\n" ;
}

void password ()
{
	char *pass ;

	while (true)
	{
		cout << endl ;
		pass = getpass ("Enter password : ") ;
	
		if (strcmp (pass, "hello world") == 0)
		{
			print_welcome () ;
			break ;
		}
		else
			cout << "Incorrect password" ;
	}
}

void print_welcome ()
{
	static const char *msg = "Welcome to MaSH, version 0.4.1(1) (GNU/Linux)\nYet another mintty created to kill boredom\nFor manual please refer to the one given\nwith the package." ;

	fprintf (stdout,"\n\n\033[1;33m%s\033[0m\n", msg) ;

	struct utsname u_name ;

	int n = uname (&u_name) ;

	if (n == -1)
	{
		perror ("\033[1;31mError 'uname()': \033[0m") ;
		exit (1) ;
	}

	cout << "\n\033[1;4;31mSystem Information\033[0m\n\n" ;

	cout << "\033[1;34mSystem : \033[0m" << u_name.sysname << endl ;
	cout << "\033[1;34mNodename : \033[0m" << u_name.nodename << endl ;
	cout << "\033[1;34mRelease : \033[0m" << u_name.release << endl ;
	cout << "\033[1;34mVersion : \033[0m" << u_name.version << endl ;
	cout << "\033[1;34mMachine : \033[0m" << u_name.machine << endl << endl ;
}

char * trim_space (char *str)
{
	char *end ;

  	while (isspace (*str)) 
		str++ ;

  	if (*str == 0)
    		return str ;

  	end = str + strlen (str) - 1 ;

  	while (end > str && isspace (*end)) 
		end-- ;

  	*(end + 1) = 0 ;

  	return str ;
}

static void sigchld_hdl (int sig)
{
	while (waitpid(-1, NULL, WNOHANG) > 0) 
	{
	}
}
