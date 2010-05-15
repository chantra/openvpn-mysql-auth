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
 * rconf.c - configuration parser
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "rconf.h"
#include "sqlstuff.h" /* s_conf */

#define FREE_NOT_NULL(a) if (a!=NULL) free (a)

struct s_conf * rconf(const char *file)
{
	FILE *ffd;
	struct s_conf *conf;
	char name[255], value[255];

	if ((ffd = fopen(file, "r")) == NULL) 
		return NULL;
	
	conf = (struct s_conf *) malloc(sizeof(struct s_conf));

	/* init default values */
	conf->hostname = NULL;
	conf->login = NULL;
	conf->passw = NULL;
	conf->db = NULL;
	conf->s_path = NULL;
	conf->port = 0;

	while(!feof(ffd))
	{
		if (feof(ffd)) break;
		fscanf(ffd, "%s %s\n", name, value);
		
		if (!strcmp(name, "hostname")) /* can be null (if unix socket) */
		{
			if (check_none(value))
				conf->hostname = NULL;
			else
			{
				conf->hostname = (char*) malloc(sizeof(char) * strlen(value) + 1);
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
                        if (check_none(value))
                                conf->s_path = NULL;
                        else
                        {
                                conf->s_path = (char*) malloc(sizeof(char) * strlen(value) + 1);
                                strncpy(conf->s_path, value, strlen(value)+1);
                        }   
                }
								else if (!strcmp (name, "table"))
								{
									conf->table = (char*) malloc(sizeof(char) * strlen(value) + 1);
                  strncpy(conf->table, value, strlen(value)+1);
								}else if (!strcmp (name, "id_field"))
								{
									conf->id_field = (char*) malloc(sizeof(char) * strlen(value) + 1);
                  strncpy(conf->id_field, value, strlen(value)+1);
								}else if (!strcmp (name, "login_field"))
								{
									conf->login_field = (char*) malloc(sizeof(char) * strlen(value) + 1);
                  strncpy(conf->login_field, value, strlen(value)+1);
								}else if (!strcmp (name, "passwd_field"))
								{
									conf->passwd_field = (char*) malloc(sizeof(char) * strlen(value) + 1);
                  strncpy(conf->passwd_field, value, strlen(value)+1);
								}else if (!strcmp (name, "passwd_type"))
								{
									if (!strcasecmp (value, "plain"))
									{
										conf->passwd_type = PASSWD_PLAIN;
									}else if (!strcasecmp (value, "md5"))
									{
										conf->passwd_type = PASSWD_MD5;
									}else if (!strcasecmp (value, "sha1")){
										conf->passwd_type = PASSWD_SHA1;
									}
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
		free(conf);
		return NULL;
	}
	return conf;
}


int	check_none(const char *value)
{
	if (!strcmp(value, "none")) 
		return 1;
	else 
		return 0;
}

void rconf_free (struct s_conf *conf)
{
	if (conf == NULL)
	{
		return;
	}
	FREE_NOT_NULL (conf->hostname);	
	FREE_NOT_NULL (conf->login);	
	FREE_NOT_NULL (conf->passw);	
	FREE_NOT_NULL (conf->db);	
	FREE_NOT_NULL (conf->table);	
	FREE_NOT_NULL (conf->id_field);	
	FREE_NOT_NULL (conf->login_field);	
	FREE_NOT_NULL (conf->passwd_field);	
	FREE_NOT_NULL (conf->s_path);	
	FREE_NOT_NULL (conf);	
}
