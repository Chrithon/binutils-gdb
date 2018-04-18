/* Header file for GDB CLI command implementation library.
   Copyright (C) 2000-2018 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#if !defined (CLI_SCRIPT_H)
#define CLI_SCRIPT_H 1

struct ui_file;
struct cmd_list_element;

/* * Control types for commands.  */

enum misc_command_type
{
  ok_command,
  end_command,
  else_command,
  nop_command
};

enum command_control_type
{
  simple_control,
  break_control,
  continue_control,
  while_control,
  if_control,
  commands_control,
  python_control,
  compile_control,
  guile_control,
  while_stepping_control,
  define_control,
  invalid_control
};

struct command_line;

extern void free_command_lines (struct command_line **);

/* A deleter for command_line that calls free_command_lines.  */

struct command_lines_deleter
{
  void operator() (command_line *cmd_lines) const
  {
    free_command_lines (&cmd_lines);
  }
};

/* A reference-counted struct command_line.  */
typedef std::shared_ptr<command_line> counted_command_line;

/* * Structure for saved commands lines (for breakpoints, defined
   commands, etc).  */

struct command_line
{
  explicit command_line (command_control_type type_, char *line_ = nullptr)
    : line (line_),
      control_type (type_)
  {
    memset (&control_u, 0, sizeof (control_u));
  }

  DISABLE_COPY_AND_ASSIGN (command_line);

  struct command_line *next = nullptr;
  char *line;
  enum command_control_type control_type;
  union
    {
      struct
	{
	  enum compile_i_scope_types scope;
	  void *scope_data;
	}
      compile;
    }
  control_u;
  /* * For composite commands, the nested lists of commands.  For
     example, for "if" command this will contain the then branch and
     the else branch, if that is available.  */
  counted_command_line body_list_0;
  counted_command_line body_list_1;

private:

  friend void free_command_lines (struct command_line **);

  ~command_line ()
  {
    xfree (line);
  }
};

extern counted_command_line read_command_lines (const char *, int, int,
						void (*)(char *, void *),
						void *);
extern counted_command_line read_command_lines_1 (char * (*) (void), int,
						  void (*)(char *, void *),
						  void *);


/* Exported to cli/cli-cmds.c */

extern void script_from_file (FILE *stream, const char *file);

extern void show_user_1 (struct cmd_list_element *c,
			 const char *prefix,
			 const char *name,
			 struct ui_file *stream);

/* Exported to gdb/breakpoint.c */

extern enum command_control_type
	execute_control_command (struct command_line *cmd);

extern enum command_control_type
	execute_control_command_untraced (struct command_line *cmd);

extern counted_command_line get_command_line (enum command_control_type,
					      const char *);

extern void print_command_lines (struct ui_out *,
				 struct command_line *, unsigned int);

/* Exported to gdb/infrun.c */

extern void execute_user_command (struct cmd_list_element *c, const char *args);

/* If we're in a user-defined command, replace any $argc/$argN
   reference found in LINE with the arguments that were passed to the
   command.  Otherwise, treat $argc/$argN as normal convenience
   variables.  */
extern std::string insert_user_defined_cmd_args (const char *line);

/* Exported to top.c */

extern void print_command_trace (const char *cmd, ...)
  ATTRIBUTE_PRINTF (1, 2);

/* Exported to event-top.c */

extern void reset_command_nest_depth (void);

#endif /* !defined (CLI_SCRIPT_H) */
