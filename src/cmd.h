/*
 * Copyright (c) 2013-2014 Bertrand Janin <b@janin.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _CMD_H_
#define _CMD_H_

#include <stdbool.h>
#include <wchar.h>

enum command {
	COMMAND_VERSION,
	COMMAND_USAGE,
	COMMAND_GET,
	COMMAND_EDIT,
	COMMAND_GENERATE,
	COMMAND_QUERY
};

extern wchar_t		*cmd_add_prefix;
extern char		*cmd_config_path;
extern char		*cmd_gpg_key_id;
extern char		*cmd_profile_name;
extern bool		 cmd_regex;
extern bool		 cmd_raw;
extern unsigned int	 cmd_character_count;
extern unsigned int	 cmd_password_count;

enum command		 cmd_parse(int, char **);
int			 cmd_parse_core(int, char **);
void			 cmd_parse_add(int, char **);
void			 cmd_parse_edit(int, char **);
void			 cmd_parse_generate(int, char **);
void			 cmd_parse_get(int, char **);
void			 cmd_parse_prompt(int, char **);
void			 cmd_usage_core(void);
void			 cmd_usage_core_with_commands(void);
bool			 command_match(const char *, const char *, size_t);

#endif /* _CMD_H_ */
