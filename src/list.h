/**
 * list.h
 * vim: tabstop=2 softtabstop=2 shiftwidth=2 expandtab
 * Copyright (C) 2009 Emmanuel Bretelle <chantra@debuntu.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef __LIST_H__
#define __LIST_H__

#include <stdint.h>

typedef struct am_list_item{
  void                  *data;
  struct am_list_item   *prev;
  struct am_list_item   *next;
} am_list_item_t;

typedef struct am_list {
  am_list_item_t  *first;
  am_list_item_t  *last;
  uint32_t        counter;
} am_list_t;

extern am_list_t        *am_list_new( void );
extern void             am_list_free( am_list_t *l, void (*item_free_func)( void *data ) );
extern void             am_list_free_item( am_list_t *l, am_list_item_t *i, void (*item_free_func)( void *data ) );
extern void             *am_list_remove_item( am_list_t *l, am_list_item_t *i );
extern void             *am_list_remove_item_at( am_list_t *l, uint32_t index );
extern am_list_item_t   *am_list_item_at( am_list_t *l, uint32_t index );

extern am_list_item_t   *am_list_append( am_list_t *l, void *data );
extern am_list_item_t   *am_list_prepend( am_list_t *l, void *data );
extern uint32_t         am_list_length( am_list_t *l );
extern am_list_item_t   *am_list_first( am_list_t *l );
extern am_list_item_t   *am_list_last( am_list_t *l );

extern am_list_item_t   *am_list_item_next( am_list_item_t *i );
extern am_list_item_t   *am_list_item_prev( am_list_item_t *i );

#endif /* __LIST_H__ */

