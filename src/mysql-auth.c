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
 *  Copyright (C) 2010 Emmanuel Bretelle <chantra@debuntu.org>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 *  USA.
 */

/*
 * OpenVPN plugin to do mysql authentication
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "openvpn-plugin.h"
#include "am_mysql.h"
#include "plugin_conf.h"
#include "utils.h"
#include "debug.h"

#define DEFAULT_CONFIG		"mysql-auth.conf"
/*
 * Our context, where we keep our state.
 */
struct plugin_context {
	struct plugin_conf   *conf;
  char            debug;
};

OPENVPN_EXPORT openvpn_plugin_handle_t
openvpn_plugin_open_v2 (unsigned int *type_mask, const char *argv[],
                        const char *envp[], struct openvpn_plugin_string_list **return_list)
{
	const char *configfile = DEFAULT_CONFIG;
  struct plugin_context *context;
	int rc = 0;

  /*
   * init mysql library
   */
  if (am_mysql_library_init () != 0){
    ERROR("Could not initialize MySQL library");
    return NULL;
  }


  /*
   * Allocate our context
   */
  context = am_malloc (sizeof (struct plugin_context));
  if (context == NULL){
    ERROR("Could not allocate plugin_context memory");
    return NULL;
  }
  am_memset (context, 0, sizeof (struct plugin_context));

	/*
	* get our command line argument
	*/
	while ( ( rc = getopt ( string_array_len (argv), (char **)argv, "c:d" ) ) != - 1 ){
		switch (rc){
			case 'c':
				configfile = optarg;
				break;
      case 'd':
        context->debug = 1;
			default:
				break;
		}
	}
  /*
   * configure our mysql client
   */
  context->conf = plugin_conf_new (configfile);
  if (context->conf == NULL){
    ERROR ("Could not initialize plugin configuration from file %s", configfile);
    return NULL;
  }
  /*
   * We are only register the plugin for which
   * we have a query defined
   */
  *type_mask = 0;
  if (context->conf->tls_verify_query){
    *type_mask |= OPENVPN_PLUGIN_MASK ( OPENVPN_PLUGIN_TLS_VERIFY );
    DEBUG_PLUGIN(context, "Registered callback OPENVPN_PLUGIN_TLS_VERIFY\n");
  }
  if (context->conf->tls_final_query){
    *type_mask |= OPENVPN_PLUGIN_MASK ( OPENVPN_PLUGIN_TLS_FINAL );
    DEBUG_PLUGIN(context, "Registered callback OPENVPN_PLUGIN_TLS_FINAL\n");
  }
  if (context->conf->client_connect_query){
    *type_mask |= OPENVPN_PLUGIN_MASK ( OPENVPN_PLUGIN_CLIENT_CONNECT );
    DEBUG_PLUGIN(context, "Registered callback OPENVPN_PLUGIN_CLIENT_CONNECT\n");
  }
  if (context->conf->client_disconnect_query){
    *type_mask |= OPENVPN_PLUGIN_MASK ( OPENVPN_PLUGIN_CLIENT_DISCONNECT );
    DEBUG_PLUGIN(context, "Registered callback OPENVPN_PLUGIN_CLIENT_DISCONNECT\n");
  }
  if (context->conf->learn_address_query){
    *type_mask |= OPENVPN_PLUGIN_MASK ( OPENVPN_PLUGIN_LEARN_ADDRESS );
    DEBUG_PLUGIN(context, "Registered callback OPENVPN_PLUGIN_LEARN_ADDRESS\n");
  }
  /* only register PF callback if user or group rules are defined */
  if (plugin_conf_pf_enabled (context->conf)){
    *type_mask |= OPENVPN_PLUGIN_MASK ( OPENVPN_PLUGIN_ENABLE_PF );
    DEBUG_PLUGIN(context, "Registered callback OPENVPN_PLUGIN_ENABLE_PF\n");
  }
  if (context->conf->auth_user_pass_verify_query){
    *type_mask |= OPENVPN_PLUGIN_MASK ( OPENVPN_PLUGIN_AUTH_USER_PASS_VERIFY );
    DEBUG_PLUGIN(context, "Registered callback OPENVPN_PLUGIN_AUTH_USER_PASS_VERIFY\n");
  }

  return (openvpn_plugin_handle_t) context;
}

OPENVPN_EXPORT int
openvpn_plugin_func_v2 (openvpn_plugin_handle_t handle, const int type,
                        const char *argv[], const char *envp[],
                        void *per_client_context,  struct openvpn_plugin_string_list **return_list)
{
  struct plugin_context *context = (struct plugin_context *) handle;
  struct client_context *client_context = (struct client_context *) per_client_context;

  if (type == OPENVPN_PLUGIN_CLIENT_CONNECT){
    DEBUG_PLUGIN(context, "OPENVPN_PLUGIN_CLIENT_CONNECT received\n");
    dump_env (envp);
    dump_env (argv);
    if (am_mysql_simple_query (context->conf->client_connect_query, context->conf, client_context, envp, argv) == 1){
      return OPENVPN_PLUGIN_FUNC_SUCCESS;
    }
    return OPENVPN_PLUGIN_FUNC_SUCCESS;
  }
  if (type == OPENVPN_PLUGIN_ENABLE_PF){
    /* TODO Enable firewall */
    DEBUG_PLUGIN(context, "OPENVPN_PLUGIN_ENABLE_PF received\n");
    /*
    dump_env (envp);
    dump_env (argv);
    */
    /** We should only land here if user or group rules are defined, but anyway
     * we better double check we have something defined
     */
    if (plugin_conf_pf_enabled (context->conf)){
        return OPENVPN_PLUGIN_FUNC_SUCCESS;
    }
    return OPENVPN_PLUGIN_FUNC_ERROR;
  }
  if (type == OPENVPN_PLUGIN_CLIENT_DISCONNECT){
    DEBUG_PLUGIN(context, "OPENVPN_PLUGIN_CLIENT_DISCONNECT received\n");
    if (am_mysql_simple_query (context->conf->client_disconnect_query, context->conf, client_context, envp, argv) == 1){
      return OPENVPN_PLUGIN_FUNC_SUCCESS;
    }
    return OPENVPN_PLUGIN_FUNC_SUCCESS;
  }

  if (type == OPENVPN_PLUGIN_LEARN_ADDRESS){
    DEBUG_PLUGIN(context, "OPENVPN_PLUGIN_LEARN_ADDRESS received\n");
    dump_env (envp);
    dump_env (argv);
    return OPENVPN_PLUGIN_FUNC_SUCCESS;
  }
	if (type == OPENVPN_PLUGIN_TLS_VERIFY){
    DEBUG_PLUGIN(context, "OPENVPN_PLUGIN_TLS_VERIFY received\n");
    /*
    dump_env (envp);
    dump_env (argv);
    */
    return OPENVPN_PLUGIN_FUNC_SUCCESS;
  }
	if (type == OPENVPN_PLUGIN_TLS_FINAL){
    DEBUG_PLUGIN(context, "OPENVPN_PLUGIN_TLS_FINAL received\n");
    //check if user or group is (still) allowed to connect
    dump_env (envp);
    dump_env (argv);
    return OPENVPN_PLUGIN_FUNC_SUCCESS;
  }
  if( type == OPENVPN_PLUGIN_AUTH_USER_PASS_VERIFY )
	{
    DEBUG_PLUGIN(context, "OPENVPN_PLUGIN_AUTH_USER_PASS_VERIFY received\n");
    /* This function will authenticate user, validate access permission and
     * finally set up firewall rules if needed
     */
    if (am_mysql_handle_auth_user_pass_verify (context->conf, client_context, envp, argv) > 0){
      return OPENVPN_PLUGIN_FUNC_SUCCESS;
    }
    return OPENVPN_PLUGIN_FUNC_ERROR;
	}
	return OPENVPN_PLUGIN_FUNC_ERROR;
}

/**
 * plugin_close_v1:
 * Called immediately prior to plug-in unload
 */
OPENVPN_EXPORT void
openvpn_plugin_close_v1 (openvpn_plugin_handle_t handle)
{
  struct plugin_context *context = (struct plugin_context *) handle;
  plugin_conf_free (context->conf);
  am_mysql_library_end();
  free (context);
}


/**
 * Construct a client context meant to store ids...
 */
OPENVPN_EXPORT void *
openvpn_plugin_client_constructor_v1 (openvpn_plugin_handle_t handle){
  struct plugin_context *context = (struct plugin_context *) handle;
  DEBUG_PLUGIN(context, "OPEN_PLUGIN_CLIENT_DESTRUCTOR_V1 received\n");
  return (void *)client_context_new ();
}

/**
 * Destroy client context
 */

OPENVPN_EXPORT void
openvpn_plugin_client_destructor_v1 (openvpn_plugin_handle_t handle, void *per_client_context){
  struct plugin_context *context = (struct plugin_context *) handle;
  struct client_context *client_context = per_client_context;
  DEBUG_PLUGIN(context, "OPEN_PLUGIN_CLIENT_DESTRUCTOR_V1 received\n");
  client_context_free (client_context);
}
