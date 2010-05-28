/**
 * vim: tabstop=2:shiftwidth=2:softtabstop=2:expandtab
 * utils.c
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

#include "utils.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

void *
am_malloc ( size_t size ){
  return malloc ( size );
}

void
am_free ( void *ptr ){
  if (ptr != NULL)
    free ( ptr );
}

void *
am_memset ( void *s, int c, size_t n ){
  return memset ( s, c, n );
}

char *
strdupf ( const char *fmt, ... ){
  va_list  vargs;
  char     buf[BUFSIZ];
  char    *p;

  if ( !fmt ) {
    return ( NULL );
  }
  va_start ( vargs, fmt );
  vsnprintf ( buf, sizeof ( buf ), fmt, vargs );
  va_end ( vargs );

  buf[sizeof ( buf ) - 1] = '\0';        /* ensure buf is NUL-terminated */

  if ( !( p = strdup ( buf ) ) ) {
    return ( NULL );
  }
  return ( p );
}

char *
str_replace ( const char *string, const char *substr, const char *replacement ){
  char *tok = NULL;
  char *newstr = NULL;
  char *oldstr = NULL;

  /* if either substr or replacement is NULL, duplicate string a let caller handle it */
  if ( substr == NULL || replacement == NULL ) return strdup (string);
  newstr = strdup (string);
  while ( (tok = strstr ( newstr, substr ))){
    oldstr = newstr;
    newstr = am_malloc ( strlen ( oldstr ) - strlen ( substr ) + strlen ( replacement ) + 1 );
    /*failed to alloc mem, free old string and return NULL */
    if ( newstr == NULL ){
      am_free (oldstr);
      return NULL;
    }
    memcpy ( newstr, oldstr, tok - oldstr );
    memcpy ( newstr + (tok - oldstr), replacement, strlen ( replacement ) );
    memcpy ( newstr + (tok - oldstr) + strlen( replacement ), tok + strlen ( substr ), strlen ( oldstr ) - strlen ( substr ) - ( tok - oldstr ) );
    memset ( newstr + strlen ( oldstr ) - strlen ( substr ) + strlen ( replacement ) , 0, 1 );
    am_free (oldstr);
  }
  return newstr;
}

const char *
get_env ( const char *name, const char *envp[] )
{
  if ( envp ){
    int i;
    const int namelen = strlen ( name );
    for ( i = 0; envp[i]; ++i ){
      if ( !strncmp ( envp[i], name, namelen ) )
      {
        const char *cp = envp[i] + namelen;
        if ( *cp == '=' )
          return cp + 1;
      }
    }
  }
  return NULL;
}

void
dump_env ( const char *envp[] )
{

  fprintf ( stderr, "//START of dump_env\\\\\n" );
  if ( envp ){
    int i;
    for ( i = 0; envp[i]; ++i )
      fprintf ( stderr, "%s\n", envp[i] );
  }
  fprintf ( stderr, "//END of dump_env\\\\\n" );
}

int
string_array_len ( const char *array[] )
{
  int i = 0;
  if ( array ){
    while ( array[i] )
      ++i;
  }
  return i;
}


