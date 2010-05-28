/**
 * vim: tabstop=2:shiftwidth=2:softtabstop=2:expandtab
 * am_mysql.c
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


#include <mysql/mysql.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "am_mysql.h"
#include "utils.h"
#include "list.h"
#include "kvp.h"
#include "debug.h"



#define NULL_OR_DUP(k) k==NULL ? NULL : strdup (k)
#define EV_DUP(k) strdupf("{{%s}}", k)
/*
 * Return a list containing our special expandable variable
 * like {{username}}, {{password}}, {{now}} ...
 */
static am_list_t *create_expandable_variable_list (MYSQL *mysql, const char *envp[], const char *argv[]);
/* Free a list created by create_expandable_variable_list */
static void free_expandable_variable_list (am_list_t *expandable_variable_list);


/**
 * Expands variable in a string given a list of expandable variable
 * If an expandable variable does not exist or is NULL
 * The expandable pattern {{myar}} will be unmodified
 */
char *
expand_query (char *query, am_list_t *expandable_var){
	char *tmp = NULL;
	char *res = NULL;
	char *key, *value;
	am_list_item_t *node;
	res = strdup (query);	

	for (node = expandable_var->first; node; node = node->next){
		if (node->data == NULL
				|| ((kvp_t *)node->data)->key == NULL
				|| ((kvp_t *)node->data)->value == NULL)
			continue;
		key = ((kvp_t *)node->data)->key;
		value = ((kvp_t *)node->data)->value;

		tmp = res;
		if (strstr (tmp, key) == NULL)
			continue;
		res = str_replace (tmp, key, value);
		am_free (tmp);
	}
	return res;
}

/**
 * Callback to free individual kvp nodes
 */
void
expandable_variable_list_free_func (void *data){
	kvp_t *k = data;
	kvp_free (k);
}

/**
 * Create a list of expandable variables from envp and possibly argv
 */
am_list_t *
create_expandable_variable_list (MYSQL *mysql, const char *envp[], const char *argv[]){
	time_t t;
	char      *escaped_username = NULL;
  char      *escaped_password = NULL;
	unsigned long escaped_username_length, escaped_password_length;
	const char *username, *password;
	am_list_t *l = am_list_new ();

	if (l == NULL)
		return l;

	t = time (NULL);

	username = get_env ("username", envp);
	password = get_env ("password", envp);
	if (username != NULL){
		escaped_username = (char *) am_malloc (sizeof(char) * (strlen (username) * 2 + 1));
		escaped_username_length = mysql_real_escape_string (mysql, escaped_username, username, strlen (username));
	}
	if (password != NULL){
		escaped_password = (char *) am_malloc (sizeof(char) * (strlen (password) * 2 + 1));
		escaped_password_length = mysql_real_escape_string (mysql, escaped_password, password, strlen (password));
	}

	am_list_append (l, kvp_new_with_kv (EV_DUP ("time_now"), strdupf ("%d", t)));	
	am_list_append (l, kvp_new_with_kv (EV_DUP ("username"), NULL_OR_DUP (username)));	
	am_list_append (l, kvp_new_with_kv (EV_DUP ("password"), NULL_OR_DUP (password)));	
	am_list_append (l, kvp_new_with_kv (EV_DUP ("escaped_username"), NULL_OR_DUP (escaped_username)));	
	am_list_append (l, kvp_new_with_kv (EV_DUP ("escaped_password"), NULL_OR_DUP (escaped_password)));	
	am_list_append (l, kvp_new_with_kv (EV_DUP ("trusted_port"), NULL_OR_DUP (get_env ("trusted_port", envp))));	
	am_list_append (l, kvp_new_with_kv (EV_DUP ("trusted_ip"), NULL_OR_DUP (get_env ("trusted_ip", envp))));	
	am_list_append (l, kvp_new_with_kv (EV_DUP ("time_unix"), NULL_OR_DUP (get_env ("time_unix", envp))));	
	am_list_append (l, kvp_new_with_kv (EV_DUP ("ifconfig_pool_remote_ip"), NULL_OR_DUP (get_env ("ifconfig_pool_remote_ip", envp))));	
	am_list_append (l, kvp_new_with_kv (EV_DUP ("ifconfig_pool_local_ip"), NULL_OR_DUP (get_env ("ifconfig_pool_local_ip", envp))));	
	am_list_append (l, kvp_new_with_kv (EV_DUP ("ifconfig_local"), NULL_OR_DUP (get_env ("ifconfig_local", envp))));	
	am_list_append (l, kvp_new_with_kv (EV_DUP ("time_duration"), NULL_OR_DUP (get_env ("time_duration", envp))));	
	am_list_append (l, kvp_new_with_kv (EV_DUP ("bytes_sent"), NULL_OR_DUP (get_env ("bytes_sent", envp))));	
	am_list_append (l, kvp_new_with_kv (EV_DUP ("bytes_received"), NULL_OR_DUP (get_env ("bytes_received", envp))));	

	am_free (escaped_username);
	am_free (escaped_password);
  return l;
}

/**
 * Create a list of expandable variable and include user_id from client
 * context
 */
am_list_t *
create_expandable_variable_list_with_user_id (MYSQL *mysql, struct client_context *cc,
                                            const char *envp[], const char *argv[]) {
  am_list_t *l = create_expandable_variable_list (mysql, envp, argv);
  /* Prepend user_id as we might look it up often */
  if (cc == NULL)
    am_list_prepend (l, kvp_new_with_kv (EV_DUP ("user_id"), NULL));
  else
    am_list_prepend (l, kvp_new_with_kv (EV_DUP ("user_id"), NULL_OR_DUP (cc->user_id)));
  return l;
}

void
free_expandable_variable_list (am_list_t *expandable_variable_list){
	am_list_free (expandable_variable_list, expandable_variable_list_free_func);
}

int
am_mysql_prepare_mysql (MYSQL *mysql, struct plugin_conf *conf){

	if (mysql_init(mysql) == NULL){
		LOGERROR ("Could not initialise MySQL: Insufficient Memory\n");
		return -1;
	}

	/* Potentially try to read group options if any are set */	
	if (mysql_options(mysql, MYSQL_READ_DEFAULT_GROUP, "openvpn-mysql-auth") != 0){
		LOGWARNING ("Could not read options for group *openvpn-mysql-auth*\n");
	}

	if (!mysql_real_connect(mysql, conf->hostname, conf->login, conf->passw, conf->db, conf->port, conf->s_path, 0)){
		LOGERROR ("Failed to connect to database: Error (%d): %s\n", mysql_errno(mysql), mysql_error(mysql));
		return -1; /* unable to establish connection */
	}
  return 0;
}

/**
 * Given a mysql handle, a non expanded query,
 * client_context and envp, argv
 * create expandable variable list, expand query
 * free expandable variable list and
 * Return formatted query, NULL on error
 */
char *
am_mysql_prepare_query (MYSQL *mysql, char *raw_query, 
                        struct client_context *client_context,
                        const char *envp[], const char *argv[]){
  char *q = NULL;
	/* Generate expandable variables */
	am_list_t *l = create_expandable_variable_list_with_user_id (mysql, client_context, envp, argv);
	/* Generate the actual query */
	q = expand_query (raw_query, l);
  /* Free up expandable variables */
  free_expandable_variable_list (l);
  return q;
}

/**
 * Same as am_mysql_simple_query but given an already
 * formatted query and mysql handle
 */
int
_am_mysql_simple_query (MYSQL *mysql, char *fmt_query){
  MYSQL_RES			*result = NULL;
	int rc = -1;
	/* Now for the query itself */
	if (mysql_query(mysql, fmt_query))
	{
		/* query failed */
		rc = -1;
		LOGERROR ("Failed to execute query: Error (%d): %s\n", mysql_errno(mysql), mysql_error(mysql));
		goto _simple_query_free;
	}

	if ((result = mysql_store_result(mysql)) == NULL)
	{
		/*
		* Failed to get result
		* 2 cases:
		*		- no result expected (after an insert statement...
		*		- error getting result
		*/
		if (mysql_field_count (mysql) == 0){
			/* insert like statement, no result expected!, return affected rows; */
			rc = mysql_affected_rows(mysql);
			goto _simple_query_free;
		}else{
			rc = -1;
			LOGERROR ("Failed to store query results: Error (%d): %s\n", mysql_errno(mysql), mysql_error(mysql));
			goto _simple_query_free;
		}
	}

	/* Finally, we just want to know how many rows were returned by query */
	rc = mysql_num_rows(result);

_simple_query_free:
	/* Free resources */
	if (result != NULL)
		mysql_free_result(result);
	return rc;
}

int
am_mysql_simple_query (char *raw_query, struct plugin_conf *conf,
                        struct client_context *client_context,
                        const char *envp[], const char *argv[]){
	MYSQL					mysql;
  char *q = NULL;
	int rc = -1;

  if (am_mysql_prepare_mysql (&mysql, conf) < 0 ){
    LOGERROR ("Could not prepare MySQL connection in am_mysql_simple_query\n");
    return -1;
  }

  q = am_mysql_prepare_query (&mysql, raw_query, client_context, envp, argv);

  if (q == NULL){
    rc = -1;
    LOGERROR ("Failed to expand query %s\n", raw_query);
    goto simple_query_free;
  }  
  rc = _am_mysql_simple_query (&mysql, q);

simple_query_free:
	/* Free resources */
	mysql_close(&mysql);
  if (q != NULL)
  	am_free (q);
	return rc;
}
/**
 * Same as am_mysql_query_return_row_0
 * but given the mysql handle and the formatted query
 * Return NULL on error, the values of row[0][0] on success
 * The caller will need to free the result.
 */
char *
_am_mysql_query_return_row_0 (MYSQL *mysql, char *fmt_query){
  MYSQL_RES			*result = NULL;
  MYSQL_ROW     row;
  unsigned long *lengths;
  char *res = NULL;
  my_ulonglong  num_rows;

 
	/* Now for the query itself */
	if (mysql_query(mysql, fmt_query))
	{
		/* query failed */
		res = NULL;
		LOGERROR ("Failed to execute query: Error (%d): %s\n", mysql_errno(mysql), mysql_error(mysql));
		goto _query_return_row_0_free;
	}

	if ((result = mysql_store_result(mysql)) == NULL)
	{
		/*
		* Failed to get result
		* 2 cases:
		*		- no result expected (after an insert statement...
		*		- error getting result
		*/
		if (mysql_field_count (mysql) == 0){
			/* insert like statement, should not happen!; */
			res = NULL;
			goto _query_return_row_0_free;
		}else{
			res = NULL;
			LOGERROR ("Failed to store query results: Error (%d): %s\n", mysql_errno(mysql), mysql_error(mysql));
			goto _query_return_row_0_free;
		}
	}
  num_rows = mysql_num_rows(result);
	/* If num_rows > 1, we consider it is an error */
	if (num_rows > 1 ){
    res = NULL;
    LOGERROR ("Many results were found while only 1 was expected!\n");
    goto _query_return_row_0_free;
  }
  if (num_rows == 0){
    /* no result found */
    res = NULL;
    goto _query_return_row_0_free;
  }
  /* If we dont have any fields, we should fail too */
  if (mysql_num_fields(result) < 1 ){
    res = NULL;
    LOGERROR ("No field encountered in MySQL result!\n");
    goto _query_return_row_0_free;
  }

  row = mysql_fetch_row(result);
  if (row == NULL){
    /* An error occurred */
    res = NULL;
    LOGERROR ("Failed to return first row, Error (%d): %s\n", mysql_errno(mysql), mysql_error(mysql));
    goto _query_return_row_0_free;
  }
  lengths = mysql_fetch_lengths(result);
  if ( lengths[0] != 0 ){
    res = am_malloc (lengths[0] + 1);
    if (res == NULL){
      LOGERROR ("Could not allocate memory for result\n");
    }else{
      memcpy (res, row[0], lengths[0]);
      res[lengths[0]] = '\0';
    }
  }else{
    res = NULL;
  }
_query_return_row_0_free:
  if (result != NULL)
    mysql_free_result (result);
  return res;
}

char *
am_mysql_query_return_row_0 (char *raw_query, struct plugin_conf *conf,
                                  struct client_context *client_context,
                                  const char *envp[], const char *argv[]) {
	MYSQL					mysql;

  char *q = NULL;
  char *res = NULL;

  if (am_mysql_prepare_mysql (&mysql, conf) < 0 ){
    LOGERROR ("Could not prepare MySQL connection in am_mysql_query_return_row_0\n");
    return NULL;
  }

  q = am_mysql_prepare_query (&mysql, raw_query, client_context, envp, argv);
  if (q == NULL){
    res = NULL;
    LOGERROR ("Failed to expand query %s\n", raw_query);
    goto query_return_row_0_free;
  } 
  res = _am_mysql_query_return_row_0 (&mysql, q);

query_return_row_0_free:
	/* Free resources */
	mysql_close(&mysql);
  if (q != NULL)
  	am_free (q);
	return res;
}

/**
 * Given the current pf rule and a result from mysql query
 * calculate the new default rule
 */
enum default_pf_rules
_am_calculate_pf_rule (enum default_pf_rules current, char *mysql_result){
  if (current == DEFAULT_PF_RULES_UNKNOWN)
    return pf_default_drop_or_accept (mysql_result); 
  return current || pf_default_drop_or_accept (mysql_result);
}

/**
 * handle default pf_rules
 */
void
_am_mysql_handle_default_pf_rules (
    MYSQL                 *mysql,
    struct plugin_conf    *conf,
    am_list_t             *expandable_vars,
    enum default_pf_rules *default_pf_rules_clients,
    enum default_pf_rules *default_pf_rules_subnets)
{

  char *query_res = NULL;
  char *q = NULL;
  /* Default rules */
  /**
   * First we get the default rules from user query
   * If user as default rules define (accept or drop), the we skip group rules
   * Other wise we check group rules
   * Finally, if user rules are still not defined, 
   * we get them from the configuration defaults
   */
  /* set default rules to unknown */
  *default_pf_rules_clients = DEFAULT_PF_RULES_UNKNOWN;
  *default_pf_rules_subnets = DEFAULT_PF_RULES_UNKNOWN;
  /* 1) get the default rules for user */
  if (plugin_conf_pf_enabled_user (conf)){
    /* clients */
    q = expand_query (conf->enable_pf_clients_user_default_rules_query, expandable_vars);
    if (q == NULL){
      LOGERROR ("Could not set expandable variables for pf clients default rules at user level\n");
    }else{
      query_res = _am_mysql_query_return_row_0 (mysql, q);
      am_free (q);  
      if (query_res != NULL){
        *default_pf_rules_clients = _am_calculate_pf_rule (*default_pf_rules_clients, query_res);
        am_free (query_res);
      }
    }
    /* subnets */
    q = expand_query (conf->enable_pf_subnets_user_default_rules_query, expandable_vars);
    if (q == NULL){
      LOGERROR ("Could not set expandable variables for pf subnets default rules at user level\n");
    }else{
      query_res = _am_mysql_query_return_row_0 (mysql, q);
      am_free (q);  
      if (query_res != NULL){
        *default_pf_rules_subnets = _am_calculate_pf_rule (*default_pf_rules_subnets, query_res);
        am_free (query_res);
      }
    }
  }
  /* 2) get the default rules for groups if still UNKNOWN */
  if (plugin_conf_pf_enabled_group (conf)){
    if (*default_pf_rules_clients == DEFAULT_PF_RULES_UNKNOWN){
      q = expand_query (conf->enable_pf_clients_group_default_rules_query, expandable_vars);
      if (q == NULL){
        LOGERROR ("Could not set expandable variables for pf clients default rules at group level\n");
      }else{
        query_res = _am_mysql_query_return_row_0 (mysql, q);
        am_free (q);  
        if (query_res != NULL){
          *default_pf_rules_clients = _am_calculate_pf_rule (*default_pf_rules_clients, query_res);
          am_free (query_res);
        }
      }
    }
    if (*default_pf_rules_subnets == DEFAULT_PF_RULES_UNKNOWN){
      q = expand_query (conf->enable_pf_subnets_group_default_rules_query, expandable_vars);
      if (q == NULL){
        LOGERROR ("Could not set expandable variables for pf subnets default rules at group level\n");
      }else{
        query_res = _am_mysql_query_return_row_0 (mysql, q);
        am_free (q);  
        if (query_res != NULL){
          *default_pf_rules_subnets = _am_calculate_pf_rule (*default_pf_rules_subnets, query_res);
          am_free (query_res);
        }
      }
    }
  }
  /* 3) if still unknown get the default rules from config */
  if (*default_pf_rules_clients == DEFAULT_PF_RULES_UNKNOWN)
    *default_pf_rules_clients = conf->default_pf_rules_clients;
  if (*default_pf_rules_subnets == DEFAULT_PF_RULES_UNKNOWN)
    *default_pf_rules_subnets = conf->default_pf_rules_subnets;
}
/**
 * handle pf_rules
 */
void
_am_mysql_handle_pf_rules (
    MYSQL                 *mysql,
    struct plugin_conf    *conf,
    am_list_t             *expandable_vars,
    char **pf_rules_clients,
    char **pf_rules_subnets)
{

  char *query_res = NULL;
  char *q = NULL;
  /* Default rules */
  /**
   * First we get the rules from user query
   * If no user rules are found
   * we check group rules
   * Finally, if user rules are still not defined, 
   * we get them from the configuration defaults
   */
  /* set default rules to unknown */
  *pf_rules_clients = NULL;
  *pf_rules_subnets = NULL;
  /* 1) get the default rules for user */
  if (plugin_conf_pf_enabled_user (conf)){
    /* clients */
    q = expand_query (conf->enable_pf_clients_user_rules_query, expandable_vars);
    if (q == NULL){
      LOGERROR ("Could not set expandable variables for pf clients rules at user level\n");
    }else{
      query_res = _am_mysql_query_return_row_0 (mysql, q);
      am_free (q);  
      if (query_res != NULL){
        *pf_rules_clients = query_res;
      }
    }
    /* subnets */
    q = expand_query (conf->enable_pf_subnets_user_rules_query, expandable_vars);
    if (q == NULL){
      LOGERROR ("Could not set expandable variables for pf subnets rules at user level\n");
    }else{
      query_res = _am_mysql_query_return_row_0 (mysql, q);
      am_free (q);  
      if (query_res != NULL){
        *pf_rules_subnets = query_res;
      }
    }
  }
  /* 2) get the default rules for groups if still UNKNOWN */
  if (plugin_conf_pf_enabled_group (conf)){
    if (*pf_rules_clients == NULL){
      q = expand_query (conf->enable_pf_clients_group_rules_query, expandable_vars);
      if (q == NULL){
        LOGERROR ("Could not set expandable variables for pf clients rules at group level\n");
      }else{
        query_res = _am_mysql_query_return_row_0 (mysql, q);
        am_free (q);  
        if (query_res != NULL){
          *pf_rules_clients = query_res;
        }
      }
    }
    if (*pf_rules_subnets == NULL){
      q = expand_query (conf->enable_pf_subnets_group_rules_query, expandable_vars);
      if (q == NULL){
        LOGERROR ("Could not set expandable variables for pf subnets rules at group level\n");
      }else{
        query_res = _am_mysql_query_return_row_0 (mysql, q);
        am_free (q);  
        if (query_res != NULL){
          *pf_rules_subnets = query_res;
        }
      }
    }
  }
  /* 3) if still unknown get the default rules from config */
  if (*pf_rules_clients == NULL && conf->pf_rules_clients != NULL)
    *pf_rules_clients = strdup (conf->pf_rules_clients);
  if (*pf_rules_subnets == NULL && conf->pf_rules_subnets != NULL)
    *pf_rules_subnets = strdup (conf->pf_rules_subnets);
}
/**
 * write pf_file given the rules
 */
void
_am_mysql_write_pf_file (const char *pf_file,
                        enum default_pf_rules default_pf_rules_clients,
                        enum default_pf_rules default_pf_rules_subnets,
                        char *pf_rules_clients,
                        char *pf_rules_subnets)
{
  int fd = open (pf_file, O_WRONLY);
  if (fd == -1){
    LOGERROR ("Could not open file %s in write mode\n", pf_file); 
    return;
  }
  /* TODO write rules to file
 * [CLIENTS DROP|ACCEPT]
 * {+|-}common_name1
 * {+|-}common_name2
 * . . .
 * [SUBNETS DROP|ACCEPT]
 * {+|-}subnet1
 * {+|-}subnet2
 * . . .
 * [END]
 */
  close (fd);

}

/**
 * handle user auth
 */
int
am_mysql_handle_auth_user_pass_verify (struct plugin_conf *conf, struct client_context *client_context, 
                                        const char *envp[], const char *argv[]){

	MYSQL					mysql;
  MYSQL_RES			*result = NULL;
  am_list_t     *l;

  const char *pf_file = NULL;
  char *q = NULL;
  char rc = -1;
  enum default_pf_rules default_pf_rules_clients;
  enum default_pf_rules default_pf_rules_subnets; 
  char *pf_rules_clients = NULL;
  char *pf_rules_subnets = NULL;

  if (am_mysql_prepare_mysql (&mysql, conf) < 0 ){
    LOGERROR ("Could not prepare MySQL connection in am_mysql_handle_auth_user_pass_verify\n");
    return -1;
  }
  l = create_expandable_variable_list (&mysql, envp, argv);
  if (l == NULL){
    LOGERROR ("Could not set expandable variables in am_mysql_handle_auth_user_pass_verify\n");
    rc = -1;
    goto handle_auth_user_pass_verify_free;
  }
  /* 1) authenticate user and get the user id */
  q = expand_query (conf->auth_user_pass_verify_query, l);
  if (q == NULL){
    LOGERROR ("Could not set expandable variables for user authentication\n");
    rc = -1;
    goto handle_auth_user_pass_verify_free;
  }
  client_context->user_id = _am_mysql_query_return_row_0 (&mysql, q);
  if (client_context->user_id == NULL){
    DEBUG (conf->debug_sql, "User %s could not authenticate using query %s", get_env("username", envp), conf->auth_user_pass_verify_query);
    rc = 0;
    goto handle_auth_user_pass_verify_free;
  }
  am_list_prepend (l, kvp_new_with_kv (EV_DUP ("user_id"), NULL_OR_DUP (client_context->user_id)));

  if (conf->auth_user_pass_verify_user_access_query == NULL && conf->auth_user_pass_verify_user_access_query){
    /** There is no access control, return success */
    rc = 1;
    goto handle_auth_user_pass_verify_allowed;
  }

  /* 2) individual user access permission */
  if (conf->auth_user_pass_verify_user_access_query != NULL){
    am_free (q);
    q = expand_query (conf->auth_user_pass_verify_user_access_query, l);
    if (q == NULL){
      LOGERROR ("Could not set expandable variables for user access permission\n");
      rc = -1;
      goto handle_auth_user_pass_verify_free;
    }
    rc = _am_mysql_simple_query (&mysql, q);
    if (rc > 0){
      /* User is allowed */
      goto handle_auth_user_pass_verify_allowed;
    }
    DEBUG (conf->debug_sql, "User %s, id %s is not granted access permission using  %s",
            get_env("username", envp), client_context->user_id, conf->auth_user_pass_verify_user_access_query);
  }
  /* 3) group access permission */
  if (conf->auth_user_pass_verify_group_access_query != NULL){
    am_free (q);
    q = expand_query (conf->auth_user_pass_verify_group_access_query, l);
    if (q == NULL){
      LOGERROR ("Could not set expandable variables for group access permission\n");
      rc = -1;
      goto handle_auth_user_pass_verify_free;
    }
    rc = _am_mysql_simple_query (&mysql, q);
    if (rc > 0){
      /* User is allowed */
      goto handle_auth_user_pass_verify_allowed;
    }
    DEBUG (conf->debug_sql, "User %s, id %s is not granted group access permission using  %s",
            get_env("username", envp), client_context->user_id, conf->auth_user_pass_verify_group_access_query);
  }
handle_auth_user_pass_verify_allowed:
  /**
  * And now for PF rules if enabled
  */
  if ( rc > 0 && plugin_conf_pf_enabled (conf)){
    pf_file = get_env ("pf_file", envp);
    if (pf_file == NULL){
      /* Should not happen, but better be safe again */
      LOGWARNING ("PF enabled, but no pf_file available in envp\n");
      goto handle_auth_user_pass_verify_free;
    }
    /* Default rules */
    _am_mysql_handle_default_pf_rules (&mysql, conf, l, &default_pf_rules_clients, &default_pf_rules_subnets);
    /* And now, for the rules themselves */
    _am_mysql_handle_pf_rules (&mysql, conf, l, &pf_rules_clients, &pf_rules_subnets);
    if (pf_rules_clients)
      fprintf(stderr, "client rules: %s\n", pf_rules_clients);

    if (pf_rules_subnets)
      fprintf(stderr, "subnets rules: %s\n", pf_rules_subnets);
    /* write the rules to pf file */
    _am_mysql_write_pf_file (pf_file, default_pf_rules_clients, default_pf_rules_subnets, pf_rules_clients, pf_rules_subnets);
  } 
handle_auth_user_pass_verify_free:
  /* Free resources */
  if (l != NULL)
    free_expandable_variable_list (l); 
  if (result != NULL)
    mysql_free_result(result);
  mysql_close(&mysql);
  if (q != NULL)
    am_free (q);
  if (pf_rules_clients != NULL)
    am_free (pf_rules_clients);
  if (pf_rules_subnets != NULL)
    am_free (pf_rules_subnets);
  return rc;
}

int
am_mysql_library_init (void){
  return mysql_library_init (0, NULL, NULL);
}

void
am_mysql_library_end (void){
  mysql_library_end ();
}
