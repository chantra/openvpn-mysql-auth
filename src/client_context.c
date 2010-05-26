/**
 * vim: tabstop=2:shiftwidth=2:softtabstop=2:expandtab
 * client_context.c
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

#include "client_context.h"
#include "utils.h"


struct client_context *
client_context_new (void) {
	struct client_context *cc;
	cc = am_malloc (sizeof (struct client_context));
	if (cc)
		am_memset (cc, 0, sizeof (struct client_context));
	return cc;
}
void
client_context_free (struct client_context * cc) {
	if (cc == NULL)
		return;
	FREE_IF_NOT_NULL (cc->user_id);
	FREE_IF_NOT_NULL (cc);
}

