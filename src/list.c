/**
 * list.c
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

#include "list.h"
#include "utils.h"
#include "debug.h"

/**
 * list_item_new
 */
am_list_item_t *
am_list_item_new( void ){
  am_list_item_t *i = am_malloc( sizeof( am_list_item_t ) );
  if( i )
    am_memset( i, 0, sizeof( am_list_item_t ) );
  return i;
}

/**
 * am_list_new
 */
am_list_t *
am_list_new( void ){
  am_list_t *l = NULL;
  l = am_malloc( sizeof( am_list_t ) );
  if( l )
    am_memset( l, 0, sizeof( am_list_t ) );
  return l;
}

/**
 * am_list_free
 */
void
am_list_free( am_list_t *l, void (*item_free_func)( void *data ) ){
  am_list_item_t *i, *cur;
  if( !l ) return;
  i = l->first;
  while( i ){
    cur = i;
    i = i->next;
    if( item_free_func ) item_free_func( cur->data );
    am_free( cur );
  }
  am_free( l );

}

/**
 * list_free_item
 */
void
am_list_free_item( am_list_t *l, am_list_item_t *i, void (*item_free_func)( void *data ) ){

  if( l->counter == 0 ) return;
  if( l->first == i ) l->first = i->next;
  if( l->last == i ) l->last = i->prev;
  if( i->next ) i->next->prev = i->prev;
  if( i->prev ) i->prev->next = i->next;
  if( item_free_func ) item_free_func( i->data );
  am_free( i );
  l->counter--;
}

/**
 * am_list_remove_item
 * Remove an item from the list. The item cell is freed
 * BUT the item data is not freed 
 * The caller will need to free it, or memory will be lost 
 */
void *
am_list_remove_item( am_list_t *l, am_list_item_t *i ){
  void *data = NULL;
  if( !i ) return NULL;
  data = i->data;
  am_list_free_item( l, i, NULL );
  return data; 
}
/**
 * am_list_remove_item_at
 * 
 */

void *
am_list_remove_item_at( am_list_t *l, uint32_t index ){
  am_list_item_t *i;
  i = am_list_item_at(l, index );
  if( !i ) return NULL;
  return am_list_remove_item( l, i ); 
}

/**
 * list_item_at
 */
am_list_item_t *
am_list_item_at( am_list_t *l, uint32_t index ){
  am_list_item_t *it = NULL;
  uint32_t i = 0;
  if( !l || index < 0 || index >= l->counter ) return NULL;
  it = am_list_first( l );
  if( !it ) return NULL;
  for( ; i < index && it ; i++ ){
    it = it->next;
  }
  return it;
}

/**
 * am_list_append
 */
am_list_item_t *
am_list_append( am_list_t *l, void *data ){
  am_list_item_t *i = am_list_item_new( );
  if( !i ) return NULL;
  i->data = data;
  if( l->counter == 0 ){
    l->first = l->last = i;
  }else{
    if( l->last ){
      l->last->next = i;
      i->prev = l->last;
      l->last = i;
    }else{
      ERROR("Appending to list with elements, but last is not set" );
    }
  }
  l->counter++;
  return i; 
}

/**
 * am_list_prepend
 */

am_list_item_t *
am_list_prepend( am_list_t *l, void *data ){
  am_list_item_t *i = am_list_item_new( );
  if( !i ) return NULL;
  i->data = data;
  if( l->counter == 0 ){
    l->first = l->last = i;
  }else{
    if( l->first ){
      l->first->prev = i;
      i->next = l->first;
      l->first = i;
    }else{
      ERROR("Prepending to list with elements, but first is not set" );
    }
  }
  l->counter++;
  return i; 
}


/**
 * am_list_length
 */
uint32_t
am_list_length( am_list_t *l ){
  if( l ) return l->counter;
  return 0;
}

/**
 * am_list_first
 */
am_list_item_t *
am_list_first( am_list_t *l ){
  if( l ) return l->first;
  return NULL;
}

/**
 * am_list_last
 */
am_list_item_t *
am_list_last( am_list_t *l ){
  if( l ) return l->last;
  return NULL;
}


/**
 * am_list_item_next
 */

am_list_item_t *
am_list_item_next( am_list_item_t *i ){
  if( i ) return i->next;
  return NULL;
}

/**
 * am_list_item_prev
 */

am_list_item_t *
am_list_item_prev( am_list_item_t *i ){
  if( i ) return i->prev;
  return NULL;
}

