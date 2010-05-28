/**
 * vim: tabstop=2:shiftwidth=2:softtabstop=2:expandtab
 * plugin_conf.c Plugin configuration parser
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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "plugin_conf.h"
#include "utils.h"

static char
is_true (const char *value){
  if (!strcasecmp (value, "true")
      || !strcasecmp (value, "on")
      || !strcasecmp (value, "1"))
    return 1;
  return 0;
}

enum default_pf_rules
pf_default_drop_or_accept (const char *value){
  if (value != NULL && !strcasecmp (value, "accept"))
    return DEFAULT_PF_RULES_ACCEPT;
  return DEFAULT_PF_RULES_DROP;
}

struct plugin_conf *
plugin_conf_new(const char *file)
{
	FILE *ffd;
	struct plugin_conf *conf;
	char name[BUFSIZ], value[BUFSIZ];

	if ((ffd = fopen(file, "r")) == NULL) 
		return NULL;
	
	conf = am_malloc (sizeof (struct plugin_conf));
  am_memset (conf, 0, sizeof (struct plugin_conf));
	/* init default values */
  conf->pf_rules = pf_rules_new ();
  am_memset (conf->pf_rules, 0, sizeof (struct pf_rules));
  conf->pf_rules->default_pf_rules_clients = DEFAULT_PF_RULES_DROP;
  conf->pf_rules->default_pf_rules_subnets = DEFAULT_PF_RULES_DROP;


	while(!feof(ffd))
	{
		if (feof(ffd)) break;
		/* check that we match name and value or go onto next line*/
		if (fscanf(ffd, "%s %[^\n]\n", name, value) != 2) continue;
		
		if (!strcmp(name, "hostname")) /* can be null (if unix socket) */
		{
			if (strcmp (value, "none") == 0)
				conf->hostname = NULL;
			else
			{
				conf->hostname = am_malloc(sizeof(char) * strlen(value) + 1);
				strncpy(conf->hostname, value, strlen(value)+1);
			}
		}
		else if (!strcmp(name, "login"))
		{
			conf->login = (char*) malloc(sizeof(char) * strlen(value) + 1);
			strncpy(conf->login, value, strlen(value)+1);
		}
		else if (!strcmp(name, "password"))
		{
			conf->passw = (char*) malloc(sizeof(char) * strlen(value) + 1);
			strncpy(conf->passw, value, strlen(value)+1);
		}
		else if (!strcmp(name, "db"))
		{
			conf->db = (char*) malloc(sizeof(char) * strlen(value) + 1);
			strncpy(conf->db, value, strlen(value)+1);
		}
		else if (!strcmp(name, "port"))
		{
			conf->port = atoi(value);
		}
		else if (!strcmp(name, "s_path")) /* can be null */
		{
			if (strcmp (value, "none") == 0)
				conf->s_path = NULL;
			else
			{
				conf->s_path = (char*) malloc(sizeof(char) * strlen(value) + 1);
				strncpy(conf->s_path, value, strlen(value)+1);
			}   
		}else if (!strcmp (name, "debug_sql")){
      conf->debug_sql = is_true (value);
    }else if (!strcmp (name, "tls_verify_query")){
			conf->tls_verify_query = strdup (value);
		}else if (!strcmp (name, "tls_final_query")){
			conf->tls_final_query = strdup (value);
		}else if (!strcmp (name, "client_connect_query")){
			conf->client_connect_query = strdup (value);
		}else if (!strcmp (name, "client_disconnect_query")){
			conf->client_disconnect_query = strdup (value);
		}else if (!strcmp (name, "learn_address_query")){
			conf->learn_address_query = strdup (value);
    /* ENABLE PF */
    /* default rules */
		}else if (!strcmp (name, "default_pf_rules_clients")){
      conf->pf_rules->default_pf_rules_clients = pf_default_drop_or_accept (value);
		}else if (!strcmp (name, "default_pf_rules_subnets")){
      conf->pf_rules->default_pf_rules_subnets = pf_default_drop_or_accept (value);
		}else if (!strcmp (name, "pf_rules_clients")){
      conf->pf_rules->pf_rules_clients = strdup (value);
		}else if (!strcmp (name, "pf_rules_subnets")){
      conf->pf_rules->pf_rules_subnets = strdup (value);

    /* rules from mysql */
		}else if (!strcmp (name, "enable_pf_clients_user_default_rules_query")){
			conf->enable_pf_clients_user_default_rules_query = strdup (value);
		}else if (!strcmp (name, "enable_pf_clients_user_rules_query")){
			conf->enable_pf_clients_user_rules_query = strdup (value);
		}else if (!strcmp (name, "enable_pf_subnets_user_default_rules_query")){
			conf->enable_pf_subnets_user_default_rules_query = strdup (value);
		}else if (!strcmp (name, "enable_pf_subnets_user_rules_query")){
			conf->enable_pf_subnets_user_rules_query = strdup (value);
		}else if (!strcmp (name, "enable_pf_clients_group_default_rules_query")){
			conf->enable_pf_clients_group_default_rules_query = strdup (value);
		}else if (!strcmp (name, "enable_pf_clients_group_rules_query")){
			conf->enable_pf_clients_group_rules_query = strdup (value);
		}else if (!strcmp (name, "enable_pf_subnets_group_default_rules_query")){
			conf->enable_pf_subnets_group_default_rules_query = strdup (value);
		}else if (!strcmp (name, "enable_pf_subnets_group_rules_query")){
			conf->enable_pf_subnets_group_rules_query = strdup (value);
    /* AUTH USER PASS VERIFY */
		}else if (!strcmp (name, "auth_user_pass_verify_query")){
			conf->auth_user_pass_verify_query = strdup (value);
		}else if (!strcmp (name, "auth_user_pass_verify_user_access_query")){
			conf->auth_user_pass_verify_user_access_query = strdup (value);
		}else if (!strcmp (name, "auth_user_pass_verify_group_access_query")){
			conf->auth_user_pass_verify_group_access_query = strdup (value);
		}
		else
		{
			; /* unknown token ... do nothing ... */
		}
	}
	if (conf->hostname == NULL && conf->s_path == NULL)
	{
		/*
		 * error in your config file...
		 */
		am_free(conf);
		return NULL;
	}
	fclose (ffd);
	return conf;
}

void
plugin_conf_free (struct plugin_conf *conf)
{
	if (conf == NULL)
		return;

	FREE_IF_NOT_NULL (conf->hostname);	
	FREE_IF_NOT_NULL (conf->login);	
	FREE_IF_NOT_NULL (conf->passw);	
	FREE_IF_NOT_NULL (conf->db);	
	FREE_IF_NOT_NULL (conf->s_path);	
	FREE_IF_NOT_NULL (conf->tls_verify_query);
	FREE_IF_NOT_NULL (conf->tls_final_query);
	FREE_IF_NOT_NULL (conf->client_connect_query);
	FREE_IF_NOT_NULL (conf->client_disconnect_query);
	FREE_IF_NOT_NULL (conf->learn_address_query);
  pf_rules_free (conf->pf_rules);
  /* PF */
	FREE_IF_NOT_NULL (conf->enable_pf_clients_user_default_rules_query);
	FREE_IF_NOT_NULL (conf->enable_pf_clients_user_rules_query);
	FREE_IF_NOT_NULL (conf->enable_pf_subnets_user_default_rules_query);
	FREE_IF_NOT_NULL (conf->enable_pf_subnets_user_rules_query);
	FREE_IF_NOT_NULL (conf->enable_pf_clients_group_default_rules_query);
	FREE_IF_NOT_NULL (conf->enable_pf_clients_group_rules_query);
	FREE_IF_NOT_NULL (conf->enable_pf_subnets_group_default_rules_query);
	FREE_IF_NOT_NULL (conf->enable_pf_subnets_group_rules_query);
  /* AUTH */
	FREE_IF_NOT_NULL (conf->auth_user_pass_verify_query);
	FREE_IF_NOT_NULL (conf->auth_user_pass_verify_user_access_query);
	FREE_IF_NOT_NULL (conf->auth_user_pass_verify_group_access_query);
	
	FREE_IF_NOT_NULL (conf);	
}

int
plugin_conf_pf_enabled_user (struct plugin_conf *conf){
  if ( conf != NULL
      &&
      (
        (conf->enable_pf_clients_user_default_rules_query && conf->enable_pf_clients_user_rules_query
        && conf->enable_pf_subnets_user_default_rules_query && conf->enable_pf_subnets_user_rules_query)
      ))
    return 1;
  return 0;
}

int
plugin_conf_pf_enabled_group (struct plugin_conf *conf){
  if ( conf != NULL
      &&
      (
        (conf->enable_pf_clients_group_default_rules_query && conf->enable_pf_clients_group_rules_query
        && conf->enable_pf_subnets_group_default_rules_query && conf->enable_pf_subnets_group_rules_query)
      ))
    return 1;
  return 0;
}
int
plugin_conf_pf_enabled (struct plugin_conf *conf){
  if ( plugin_conf_pf_enabled_user (conf) || plugin_conf_pf_enabled_group (conf))
    return 1;
  return 0;
}
