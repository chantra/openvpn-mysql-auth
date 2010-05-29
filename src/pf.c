/**
 * vim: tabstop=2:shiftwidth=2:softtabstop=2:expandtab
 * pf.c
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


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "utils.h"
#include "debug.h"
#include "pf.h"

struct pf_rules *
pf_rules_new (){
	struct pf_rules *p = am_malloc (sizeof (struct pf_rules));
	return p;
}

void
pf_rules_free (struct pf_rules *pf_rules){
	if (pf_rules == NULL)
		return;
	FREE_IF_NOT_NULL (pf_rules->pf_rules_clients);
	FREE_IF_NOT_NULL (pf_rules->pf_rules_subnets);
	am_free (pf_rules);
}

int
pf_rules_to_file (struct pf_rules *pf_rules, const char *pf_file){
	char *s;
  int fd = open (pf_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
  if (fd == -1){
    LOGERROR ("Could not open file %s in write mode Error (%d) %s\n", pf_file, errno, strerror (errno)); 
    return -1;
  }
	/* TODO handle exception */
	s = strdupf ("[CLIENTS %s]\n", pf_rules->default_pf_rules_clients == DEFAULT_PF_RULES_ACCEPT ? "ACCEPT" : "DROP");
	write (fd, s, strlen(s));
	am_free (s);
	if (pf_rules->pf_rules_clients != NULL)
		write (fd, pf_rules->pf_rules_clients, strlen (pf_rules->pf_rules_clients));

	s = strdupf ("\n[SUBNETS %s]\n", pf_rules->default_pf_rules_subnets == DEFAULT_PF_RULES_ACCEPT ? "ACCEPT" : "DROP");
	write (fd, s, strlen(s));
	am_free (s);
	if (pf_rules->pf_rules_subnets != NULL)
		write (fd, pf_rules->pf_rules_subnets, strlen (pf_rules->pf_rules_subnets));

	s = strdup ("\n[END]\n");
	write (fd, s, strlen(s));
	am_free (s);
	
  close (fd);
	return 0;
}
