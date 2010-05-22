/**
 *
 *  Copyright 2006-2010 Gouverneur Thomas
 *
 *  This file is part of auth-mysql plugin for OpenVPN.
 *
 *  Foobar is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Foobar is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with auth-mysql plugin.  If not, see <http://www.gnu.org/licenses/>.
 *
 **/

/**
 * sqlstuff.h - header
 */

#ifndef SQLSTUFF_H

#include "list.h"

#ifndef NULL
#define NULL 0
#endif

#define SQL_GETUSER "SELECT %s,%s,%s FROM %s WHERE %s='%s' and %s=%s"

/*
 * will contain information from sql db
 */

enum passwd_type
{
    PASSWD_PLAIN = 0,
    PASSWD_MD5,
    PASSWD_SHA1
};

struct user
{
	int  id;
	char *login;
	char *passw;
};

struct s_conf
{
	char *hostname;
	char *login;
	char *passw;
	char *db;
  char *table;
  char *id_field;
  char *login_field;
  char *passwd_field;
  enum passwd_type  passwd_type;
	char *s_path;
	int  port;
  /* QUERIES */
  char *tls_verify_query;
  char *tls_final_query;
  char *auth_user_pass_verify_query;
  char *client_connect_query;
  char *client_disconnect_query;
  char *learn_address_query;
  char *enable_pf_query;
};


/**
 * Execute a simple query
 * return the number of row returned or -1 on error
 */
extern int am_mysql_simple_query (struct s_conf *conf, char *raw_query, const char *envp[], const char *argv[]);
struct user * ret_user(struct s_conf *, const char *, const char *);

/*
 * Return a list containing our special expandable variable
 * like %username%, %password%, %now% ...
 */
extern am_list_t *create_expandable_variable_list (const char *envp[], const char *argv[]);
/* Free a list created by create_expandable_variable_list */
extern void free_expandable_variable_list (am_list_t *expandable_variable_list);

#define SQLSTUFF_H
#endif
