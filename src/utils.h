/**
 * vim: tabstop=2:shiftwidth=2:softtabstop=2:expandtab
 * utils.h
 *
 * Copyright (C) 2009 Emmanuel Bretelle <chantra@debuntu.org>
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

#ifndef _UITLS_H_
#define _UTILS_H_
#include <stdlib.h>

/* memory allocation */
extern void *am_malloc ( size_t size );
extern void am_free ( void *ptr );
extern void *am_memset ( void *s, int c, size_t n );

/*
 *  Duplicates the string specified by the format-string [fmt].
 *  Returns the new string, or NULL if out of memory.
 *  The caller is responsible for freeing this new string.
 */
extern char *strdupf ( const char *fmt, ... );


/*
 * Create a new string with [substr] being replaced by [replacement] in [string]
 * Returns the new string, or NULL if out of memory.
 * The caller is responsible for freeing this new string.
 */
extern char *str_replace ( const char *string, const char *substr, const char *replacement );

/*
 * Given an environmental variable name, search
 * the envp array for its value, returning it
 * if found or NULL otherwise.
 */
extern const char *get_env ( const char *name, const char *envp[] );

/*
 * Given an environement array,
 * dump the envp values to stderr
 */
extern void dump_env ( const char *envp[] );

/*
 * Return the length of a string array
 */
extern int string_array_len ( const char *array[] );


#endif
