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

#include "openvpn-plugin.h"
#include "sqlstuff.h"
#include "rconf.h"

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

OPENVPN_EXPORT openvpn_plugin_handle_t
openvpn_plugin_open_v1 (unsigned int *type_mask, const char *argv[], const char *envp[])
{
  struct plugin_context *context;

  /*
   * Allocate our context
   */
  context = (struct plugin_context *) calloc (1, sizeof (struct plugin_context));

  /*
   * configure our mysql client
   */
  
  context->conf = rconf("auth-mysql.conf");
  
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

  /* do mysql stuff here */
  if ((u = ret_user(context->conf, username)) == NULL)
  {
	return OPENVPN_PLUGIN_FUNC_ERROR;
  }
  if (username && !strncmp(username, u->login, 51) && password && !strncmp(password, u->passw, 256))
  {
	
	free_u(u);
	return OPENVPN_PLUGIN_FUNC_SUCCESS;
  }
  free_u(u);
  return OPENVPN_PLUGIN_FUNC_ERROR;
}

OPENVPN_EXPORT void
openvpn_plugin_close_v1 (openvpn_plugin_handle_t handle)
{
  struct plugin_context *context = (struct plugin_context *) handle;
  free (context->conf);
  free (context);
}
