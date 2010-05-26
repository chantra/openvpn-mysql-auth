/**
 * vim: tabstop=2:shiftwidth=2:softtabstop=2:expandtab
 *
 * debug.c
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
#include <stdarg.h>
#include <stdio.h>

#include "debug.h"

void _debug( int level, const char *file, int line, const char *func, const char *fmt, ... ){
  va_list argp;
  if(level){
    fprintf( stderr, "DEBUG: %s:%d %s() ", file, line, func );
    va_start( argp, fmt );
    vfprintf( stderr, fmt, argp );
    va_end( argp );
    fprintf( stderr, "\n" );
  }
}

void _printdebug( int debug, const char *fmt, ... ){
  va_list argp;
  if( !debug ) return;
  fprintf( stderr, "DEBUG: ");
  va_start( argp, fmt );
  vfprintf( stderr, fmt, argp );
  va_end( argp );
}

void _warn( const char *file, int line, const char *func, const char *fmt, ... ){
  va_list argp;
  fprintf( stderr, "WARN: %s:%d %s() ", file, line, func );
  va_start( argp, fmt );
  vfprintf( stderr, fmt, argp );
  va_end( argp );
  fprintf( stderr, "\n" );
}


void _error( const char *file, int line, const char *func, const char *fmt, ... ){
  va_list argp;
  fprintf( stderr, "ERROR: %s:%d %s() ", file, line, func );
  va_start( argp, fmt );
  vfprintf( stderr, fmt, argp );
  va_end( argp );
  fprintf( stderr, "\n" );
}

void _log( const char *level, const char *fmt, ... ){
  va_list argp;
  fprintf( stderr, "MYSQL-AUTH: [%s] ", level);
  va_start( argp, fmt );
  vfprintf( stderr, fmt, argp );
  va_end( argp );
}



