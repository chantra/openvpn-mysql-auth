/**
 * vim: tabstop=2:shiftwidth=2:softtabstop=2:expandtab
 * debug.h
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

#ifndef _DEBUG_H_
#define _DEBUG_H_



#define DEBUG_SQL 1
#define DEBUG_USER 0
#define DEBUG_MAIN 1

void _printdebug( int debug, const char *fmt, ... );

void _warn( const char *file, int line, const char *func, const char *fmt, ... );
#define WARN( fmt, args... ) _warn( __FILE__, __LINE__, __FUNCTION__, fmt, ##args )

void _error( const char *file, int line, const char *func, const char *fmt, ... );
#define ERROR( fmt, args... ) _error( __FILE__, __LINE__, __FUNCTION__, fmt, ##args )

void _debug( int level, const char *file, int line, const char *func, const char *fmt, ... );
#define DEBUG( level, fmt, args... ) _debug( level, __FILE__, __LINE__, __FUNCTION__, fmt, ##args )

void _log( const char *level, const char *fmt, ... );

#define LOGERROR( fmt, args... ) _log( "ERROR", fmt, ##args )

#define LOGWARNING( fmt, args... ) _log( "WARNING", fmt, ##args )

#define LOGINFO( fmt, args... ) _log( "INFO", fmt, ##args )

#define LOGDEBUG( fmt, args... ) _log( "DEBUG", fmt, ##args )

#define DEBUG_PLUGIN( ctx, fmt, args... ) if(ctx->debug) LOGDEBUG( fmt, ##args )

#endif /* _DEBUG_H_ */

