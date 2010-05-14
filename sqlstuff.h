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
 * sqlstuff.h - header
 */

#ifndef SQLSTUFF_H

#ifndef NULL
#define NULL 0
#endif

#define SQL_GETUSER "SELECT id,login,password FROM users WHERE login='%s'"

/*
 * will contain information from sql db
 */
struct user
{
	int  id;
	char *login;
	char *passw;
};

struct s_conf
{
	char *hostname;
	char *login;
	char *passw;
	char *db;
	char *s_path;
	int  port;
};

struct user * ret_user(struct s_conf *, const char *);

#define SQLSTUFF_H
#endif
