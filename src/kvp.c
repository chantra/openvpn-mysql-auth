/**
 * vim: tabstop=2:shiftwidth=2:softtabstop=2:expandtab
 * kvp.c
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

#include "kvp.h"
#include "utils.h"

kvp_t *
kvp_new (void){
	kvp_t *k;
	k = am_malloc (sizeof (kvp_t *));
	k->key = NULL;
	k->value = NULL;
	return k;
}

kvp_t *
kvp_new_with_kv (char *key, char *value){
  kvp_t *k = kvp_new ();
  if (k == NULL)
    return k;
  k->key = key;
  k->value = value;
  return k;
}

void
kvp_free (kvp_t *k){
	if (k == NULL) return;
	if (k->key != NULL) am_free (k->key);
	if (k->value != NULL) am_free (k->value);
	am_free (k);
}


