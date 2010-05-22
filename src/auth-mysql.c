/**
 * vim: tabstop=2:shiftwidth=2:softtabstop=2:expandtab
 *  OpenVPN -- An application to securely tunnel IP networks
 *             over a single TCP/UDP port, with support for SSL/TLS-based
 *             session authentication and key exchange,
 *             packet encryption, packet authentication, and
 *             packet compression.
 *
 * auth-mysql.c
 * OpenVPN MySQL authentication plugin
 *
 *  Copyright 2006-2010 Gouverneur Thomas
 *  Copyright (C) 2010 Emmanuel Bretelle <chantra@debuntu.org>
 *
 *  This file is part of auth-mysql plugin for OpenVPN.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with auth-mysql plugin.  If not, see <http://www.gnu.org/licenses/>.
 *
 **/

/*
 * OpenVPN plugin to do mysql authentication
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <openvpn/openvpn-plugin.h>
#include "sqlstuff.h"
#include "rconf.h"
#include "utils.h"

#define DEFAULT_CONFIG		"auth-mysql.conf"
/*
 * Our context, where we keep our state.
 */
struct plugin_context {
	struct s_conf *conf;
};

/*
 * free an user struct
 */
void free_u(struct user *u)
{
	free (u->login);
	free (u->passw);
	free (u);
	return;
}


OPENVPN_EXPORT openvpn_plugin_handle_t
openvpn_plugin_open_v1 (unsigned int *type_mask, const char *argv[], const char *envp[])
{
	const char *configfile = DEFAULT_CONFIG;
  struct plugin_context *context;
	int rc = 0;

  /*
   * Allocate our context
   */
  context = (struct plugin_context *) calloc (1, sizeof (struct plugin_context));

	/*
	* get our command line argument
	*/
	while ( ( rc = getopt ( string_array_len (argv), (char **)argv, "c:" ) ) != - 1 ){
		switch (rc){
			case 'c':
				configfile = optarg;
				break;
			default:
				break;
		}
	}
  /*
   * configure our mysql client
   */
  context->conf = rconf(configfile);
  
  /*
   * We are only interested in intercepting the
   * --auth-user-pass-verify callback.
   */
  *type_mask = OPENVPN_PLUGIN_MASK ( OPENVPN_PLUGIN_AUTH_USER_PASS_VERIFY )
							| OPENVPN_PLUGIN_MASK ( OPENVPN_PLUGIN_CLIENT_CONNECT )
              | OPENVPN_PLUGIN_MASK ( OPENVPN_PLUGIN_CLIENT_DISCONNECT )
              | OPENVPN_PLUGIN_MASK ( OPENVPN_PLUGIN_LEARN_ADDRESS )
							| OPENVPN_PLUGIN_MASK ( OPENVPN_PLUGIN_TLS_VERIFY )
							| OPENVPN_PLUGIN_MASK ( OPENVPN_PLUGIN_ENABLE_PF )
							| OPENVPN_PLUGIN_MASK ( OPENVPN_PLUGIN_TLS_FINAL );

  return (openvpn_plugin_handle_t) context;
}

OPENVPN_EXPORT int
openvpn_plugin_func_v1 (openvpn_plugin_handle_t handle, const int type, const char *argv[], const char *envp[])
{
  struct plugin_context *context = (struct plugin_context *) handle;
  struct user *u;

  if (type == OPENVPN_PLUGIN_CLIENT_CONNECT){
    fprintf (stderr, "OPENVPN_PLUGIN_CLIENT_CONNECT received:\n");
    dump_env (envp);
    dump_env (argv);
    return OPENVPN_PLUGIN_FUNC_SUCCESS;
  }
  if (type == OPENVPN_PLUGIN_ENABLE_PF){
    /* TODO Enable firewall */
    fprintf (stderr, "OPENVPN_PLUGIN_ENABLE_PF received:\n");
    /*
    dump_env (envp);
    dump_env (argv);
    */
    return OPENVPN_PLUGIN_FUNC_ERROR;
  }
  if (type == OPENVPN_PLUGIN_CLIENT_DISCONNECT){
    fprintf (stderr, "OPENVPN_PLUGIN_CLIENT_DISCONNECT received:\n");
    dump_env (envp);
    dump_env (argv);
    return OPENVPN_PLUGIN_FUNC_SUCCESS;
  }
  if (type == OPENVPN_PLUGIN_LEARN_ADDRESS){
    fprintf (stderr, "OPENVPN_PLUGIN_LEARN_ADDRESS received:\n");
    dump_env (envp);
    dump_env (argv);
    return OPENVPN_PLUGIN_FUNC_SUCCESS;
  }
	if (type == OPENVPN_PLUGIN_TLS_VERIFY){
    fprintf (stderr, "OPENVPN_PLUGIN_TLS_VERIFY received:\n");
    /*
    dump_env (envp);
    dump_env (argv);
    */
    return OPENVPN_PLUGIN_FUNC_SUCCESS;
  }
	if (type == OPENVPN_PLUGIN_TLS_FINAL){
    //check if user or group is (still) allowed to connect
    fprintf (stderr, "OPENVPN_PLUGIN_TLS_FINAL received:\n");
    dump_env (envp);
    dump_env (argv);
    return OPENVPN_PLUGIN_FUNC_SUCCESS;
  }
  if( type == OPENVPN_PLUGIN_AUTH_USER_PASS_VERIFY )
	{
    fprintf (stderr, "OPENVPN_PLUGIN_AUTH_USER_PASS_VERIFY received:\n");
		/* get username/password from envp string array */
		const char *username = get_env ("username", envp);
		const char *password = get_env ("password", envp);
    am_mysql_simple_query (context->conf, context->conf->auth_user_pass_verify_query, envp, argv); 
		/* do mysql stuff here */
		if ((u = ret_user(context->conf, username, password)) == NULL)
		{
		return OPENVPN_PLUGIN_FUNC_ERROR;
		}
		free_u(u);
		return OPENVPN_PLUGIN_FUNC_SUCCESS;
	}
	return OPENVPN_PLUGIN_FUNC_ERROR;
}

OPENVPN_EXPORT void
openvpn_plugin_close_v1 (openvpn_plugin_handle_t handle)
{
  struct plugin_context *context = (struct plugin_context *) handle;
  rconf_free (context->conf);
  free (context);
}
