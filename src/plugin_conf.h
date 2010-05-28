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

#include "pf.h"

struct plugin_conf
{
	char *hostname;
	char *login;
	char *passw;
	char *db;
  char *s_path;
	int  port;
  struct pf_rules *pf_rules;
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
  /* packet filter */
  char *enable_pf_clients_user_default_rules_query;
  char *enable_pf_clients_user_rules_query;
  char *enable_pf_subnets_user_default_rules_query;
  char *enable_pf_subnets_user_rules_query;
  char *enable_pf_clients_group_default_rules_query;
  char *enable_pf_clients_group_rules_query;
  char *enable_pf_subnets_group_default_rules_query;
  char *enable_pf_subnets_group_rules_query;

};



extern struct plugin_conf   *plugin_conf_new (const char *);
extern void                 plugin_conf_free (struct plugin_conf *conf);
extern int                  plugin_conf_pf_enabled (struct plugin_conf *conf);
extern int                  plugin_conf_pf_enabled_user (struct plugin_conf *conf);
extern int                  plugin_conf_pf_enabled_group (struct plugin_conf *conf);
extern enum default_pf_rules pf_default_drop_or_accept (const char *value);
#endif
