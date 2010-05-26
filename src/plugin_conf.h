/**
 * vim: tabstop=2:shiftwidth=2:softtabstop=2:expandtab
 * plugin_conf.h
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

#ifndef _PLUGIN_CONF_H_
#define _PLUGIN_CONF_H_


enum default_pf_rules {
  DEFAULT_PF_RULES_DROP = 0,
  DEFAULT_PF_RULES_ACCEPT
};

struct plugin_conf
{
	char *hostname;
	char *login;
	char *passw;
	char *db;
  char *s_path;
	int  port;
  enum default_pf_rules default_pf_rules_clients; 
  enum default_pf_rules default_pf_rules_subnets;

  /* DEBUG */
  char  debug_sql;
  /* QUERIES */
  char *tls_verify_query;
  char *tls_final_query;
  char *auth_user_pass_verify_query;
  char *auth_user_pass_verify_user_access_query;
  char *auth_user_pass_verify_group_access_query;
  char *client_connect_query;
  char *client_disconnect_query;
  char *learn_address_query;
  char *enable_pf_clients_default_rules_query;
  char *enable_pf_clients_rules_query;
  char *enable_pf_subnets_default_rules_query;
  char *enable_pf_subnets_rules_query;
};



struct plugin_conf    *plugin_conf_new (const char *);
void                  plugin_conf_free (struct plugin_conf *conf);

#endif
