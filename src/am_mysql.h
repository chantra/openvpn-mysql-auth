/**
 * vim: tabstop=2:shiftwidth=2:softtabstop=2:expandtab
 * am_mysql.h
 *
 * Copyright (C) 2010 Emmanuel Bretelle <chantra@debuntu.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#ifndef _AM_MYSQL_H_
#define _AM_MYSQL_H_

#include "plugin_conf.h"
#include "client_context.h"

#ifndef NULL
#define NULL 0
#endif

extern int am_mysql_library_init (void);
extern void am_mysql_library_end (void);
/**
 * Execute a simple query
 * return the number of row returned or -1 on error
 */
extern int am_mysql_simple_query (char *raw_query, struct plugin_conf *conf,
                                  struct client_context *client_context,
                                  const char *envp[], const char *argv[]);

/**
 * Execute a query and return the value in
 * the first row result at column 0
 * Returns NULL if:
 *  - no row found
 *  - more than one available
 *  - an error occured
 */
extern char *am_mysql_query_return_row_0 (char *raw_query, struct plugin_conf *conf,
                                  struct client_context *client_context,
                                  const char *envp[], const char *argv[]);
/**
 * Handles user auth:
 *  - Check if user/pass is valid
 *  - Check if user can access if access needs to be checked
 *  - check if user group can access if access needed and no access for user was found
 *  - Handle firewall rules if query are set in config and user can access
 *  Return > 0 if access allowed
 *  = 0 if user matches but no access found
 *  < 0 if user could no be authenticated
 *  In case no queries are set to validate access permission, 1 will be returned
 */
extern int am_mysql_handle_auth_user_pass_verify (struct plugin_conf *conf, struct client_context *client_context,
                                                  const char *envp[], const char *argv[]);
#endif
