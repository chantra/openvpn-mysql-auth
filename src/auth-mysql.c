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


/*
 * Given an environmental variable name, search
 * the envp array for its value, returning it
 * if found or NULL otherwise.
 */
static const char *
get_env (const char *name, const char *envp[])
{
  if (envp)
    {
      int i;
      const int namelen = strlen (name);
      for (i = 0; envp[i]; ++i)
	{
	  if (!strncmp (envp[i], name, namelen))
	    {
	      const char *cp = envp[i] + namelen;
	      if (*cp == '=')
		return cp + 1;
	    }
	}
    }
  return NULL;
}

/*
 *  * Return the length of a string array
 *   */
static int
string_array_len (const char *array[])
{
  int i = 0;
  if (array){
    while (array[i])
      ++i;
  }
  return i;
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
  *type_mask = OPENVPN_PLUGIN_MASK (OPENVPN_PLUGIN_AUTH_USER_PASS_VERIFY);

  return (openvpn_plugin_handle_t) context;
}

OPENVPN_EXPORT int
openvpn_plugin_func_v1 (openvpn_plugin_handle_t handle, const int type, const char *argv[], const char *envp[])
{
  struct plugin_context *context = (struct plugin_context *) handle;
  struct user *u;

  /* get username/password from envp string array */
  const char *username = get_env ("username", envp);
  const char *password = get_env ("password", envp);

	if( type == OPENVPN_PLUGIN_AUTH_USER_PASS_VERIFY )
	{
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
  free (context->conf);
  free (context);
}
