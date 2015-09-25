/******************************************

    "main.cpp" - main shell program
    Copyright (C) 2014  Abhishek Guha

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>

******************************************/


# include "headers.h"

int main (int argc, char **argv)
{
	int childPid, sig = 0 ;
	char * cmd_line, *argcv[MAX], *cmd1[MAX], *cmd2[MAX] ;
	char *user_name, host_name[MAX], *const_msg, cwd[MAX] ;

	init_shell () ;
	set_init_dir () ;

	const_msg = NULL ;
  user_name = getenv ("USER") ;
  const_msg = user_name ;
	gethostname (host_name, sizeof(host_name)) ;
	strcat (const_msg, ":") ;
	strcat (const_msg, host_name) ;

	system ("clear") ;

	password () ;

	while (true)
	{
		if (getcwd (cwd, sizeof(cwd)) == NULL)
		{
			perror ("\033[1;31mError: \033[0m") ;
			break ;
		}

		fprintf (stdout, "\033[1;34m[\033[0m\033[1;32m%s\033[0m\033[1;34m]\033[0m@~\033[36m%s\033[0m\n", const_msg, cwd) ;		

	    cmd_line = readline ("$ ") ;
		
		history_command (cmd_line) ;

		cmd_line = trim_space (cmd_line) ;
		
		if (strlen (cmd_line) != 0)
		{
			add_history (cmd_line) ;

			sig = parse_cmd (cmd_line, argcv, cmd1, cmd2) ;
		
			if (sig == PIPE_FOUND)
				pipe_command (cmd1, cmd2) ;
			else if (sig == -4)
				cerr << "\033[33mMore than one piping not implemented\033[0m\n" ;
			else if (sig == REDIRECT_FOUND)
				redirect_command (cmd1, cmd2) ;
			else if (sig == -1)
				cerr << "\033[33mBoth piping and redirection simultaneously not implemented\033[0m\n" ;
			else if (match (argcv[0]) == 1)
			{
				if (run_builtins (argcv) == EXIT)
					break ;		
			}
			else if (match (argcv[0]) == 0)
				run_cmd (argcv) ;		
		}
	}

	return 0 ;
}
