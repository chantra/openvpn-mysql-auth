/**
 * vim: tabstop=2:shiftwidth=2:softtabstop=2:expandtab
 * pf.h
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
#ifndef _PF_H_
#define _PF_H_

enum default_pf_rules {
  DEFAULT_PF_RULES_UNKNOWN = -1,
  DEFAULT_PF_RULES_DROP = 0,
  DEFAULT_PF_RULES_ACCEPT
};


struct pf_rules{
  enum default_pf_rules default_pf_rules_clients;
  enum default_pf_rules default_pf_rules_subnets;
  char *pf_rules_clients;
  char *pf_rules_subnets;
};

extern struct pf_rules  *pf_rules_new (void);
extern void             pf_rules_free (struct pf_rules *pf_rules);
extern int              pf_rules_to_file (struct pf_rules *pf_rules, const char *pf_file);

#endif
