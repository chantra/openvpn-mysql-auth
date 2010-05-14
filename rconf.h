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
 * rconf.h - config parser header
 */

#ifndef RCONF_H
#include "sqlstuff.h"

struct s_conf * rconf(const char *);
int     check_none(const char *);


#define RCONF_H
#endif
