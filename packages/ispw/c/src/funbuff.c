/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * See file LICENSE for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 * Based on Max/ISPW by Miller Puckette.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#include <math.h>

#include <fts/fts.h>
#include "naming.h"
#include <fts/packages/data/data.h>

#define funbuff_first(FB) (FB)->head.next
  
struct zll
{
  long x;
  long y;
  struct zll *next;
  struct zll *prev;
};

typedef struct
{
  fts_object_t obj;
  struct zll head;	   	/* head of double linked list of function elts */
  struct zll *foot;		/* foot in the door pointer for list  */
  long gotoDelta;		/* used by goto and next */
  long selectX;		/* selected region start   */
  long selectW;		/* selected region width   */
  fts_symbol_t sym;		/* filename */
  long y;			/* y-value from inlet */
  char yvalid;		/* flag that y has been set since x has */
} funbuff_t;


/*    ZLL LOCAL FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  */

static struct zll *
zll_new(void)  
{
  struct zll *x = (struct zll *)fts_malloc(sizeof(struct zll));

  return x;
}

/*  deletes xfirst thru xlast and rejoines  */		
static void
zll_delete(struct zll *xfirst, struct zll *xlast)        
{
  if (xfirst->prev)
    xfirst->prev->next = xlast->next;     

  if (xlast->next)
    xlast->next->prev = xfirst->prev;

  xlast->next = 0;
}



/*  returns copy of region from x to x + w of list  or 0  */
static struct zll *
zll_copy(struct zll *list, long int x1, long int w)
{
  long x2;
  struct zll *head, *copy;	

  x2 = x1 + w;

  while (list && list->x < x1)
    list = list->next;

  if (!list || list->x > x2)
    {
      post("zll_copy: no data to copy\n");
      return 0;
    }

  head = copy = zll_new();
  head->prev = 0;
  head->x = list->x;
  head->y = list->y;

  list = list->next;

  for ( ;list && list->x <= x2;list = list->next)
    {
      copy->next = zll_new();
      copy->next->prev = copy;
      copy = copy->next;
      copy->x = list->x;
      copy->y = list->y;
    }

  copy->next = 0;
  return head;
}

static struct zll *
zll_cut(struct zll *list, long int x1, long int w)
{
  struct zll *head, *tail;
  long x2 = x1 + w;
  
  while (list && list->x < x1)
    list = list->next;

  if (!list || list->x > x2)
    return 0;

  head = list;

  for ( ;list->x <= x2;list = list->next)
    if (!list->next) break;

  tail = (list->x <= x2) ? list :list->prev;
  zll_delete(head, tail);

  return head;
}

static struct zll *
zll_last(struct zll *list)
{
  while (list->next)
    list = list->next;

  return list;
}		


static void
zll_freefun(struct zll *list)
{
  struct zll *p;

  while (list)
    {
      p = list;
      list = list->next;
      fts_free(p);
    }
}		


static long
zll_length(struct zll *list)
{
  long length = 0;

  for ( ; list; list = list->next)
    length++;

  return length;
}		

/* accepts a vector : x1 y1 x2 y2 x3 y3 ... places it in a zll list  returns list or 0 */
static struct zll *
zll_vecToZll(int ac, const fts_atom_t *av)
{
  struct zll *head, *current;	
  int count;
  
  if (ac & 1)
    {
      post("zll_vecToZll: bad input - odd arg count= %d\n", ac);
      return(0);
    }

  if (! ac)
    {
      post("zll_vecToZllf: bad input - that is,  no input\n");
      return(0);
    }

  head = current = zll_new();
  head->prev = 0;

  for (count = 0; count < 2; count++, av++)
    {
      /* pick off first element */
      if (fts_is_int(av))
	current->x = fts_get_int(av);
      else
	{
	  post("zll_vecToZll: bad input for X\n");
	  current->x = 0;
	}

      count++;
      av++;

      if (fts_is_int(av))
	current->y = fts_get_int(av);
      else
	{
	  post("zll_vecToZll: bad input for Y\n");
	  current->y = 0;
	}
    }

  for (count = 2; count < ac; count++, av++)
    {
      /* pick off any others */
      current->next = zll_new();
      current->next->prev = current;
      current = current->next;

      if (fts_is_int(av))
	current->x = fts_get_int(av);
      else
	{
	  post("zll_vecToZll: bad input for X\n");
	  current->x = 0;
	}

      count++;
      av++;

      if (fts_is_int(av))
	current->y = fts_get_int(av);
      else
	{
	  post("zll_vecToZll: bad input for Y\n");
	  current->y = 0;
	}
    }

  current->next = 0;
  return head ;
}


/* ++++++++++++++   ZLLF object   (floting point zll) +++++++++++++*/

struct zllf	       
{
  long	x;
  float	y;					/* floating pt val for Y  */
  struct zllf *next;
  struct zllf *prev;
};


static struct zllf *
zllf_new(void)
{
  struct zllf *x = (struct zllf *) fts_malloc(sizeof(struct zllf));

  return x;
}


static void
zllf_freefun(struct zllf *list)
{
  struct zllf *p;

  while (list)
    {
      p = list;
      list = list->next;
      fts_free(p);
    }
}		


/* +++++++++++++++++++++++++++ ZLL DATA  REDUCTION ++++++++++++++++  */

static float
zll_distance(long int x, long int y, long int x1, long int y1, long int x2, long int y2)
{
  long x1_x2;
  long y2_y1;
  long x2y1_x1y2;

  x1_x2 = x1 - x2;
  y2_y1 = y2 - y1;
  x2y1_x1y2 = (x2 * y1) - (x1 * y2);

  return((float)fabs((double)((x * y2_y1) + (y * x1_x2) + x2y1_x1y2)));
}


static struct zllf *
zll_getslope(struct zll *function)
{
  struct zllf *slope_head, *slope;
  
  if (!function)
    {
      post(" zll_getslope: no data \n");
      return 0;
    }
  
  slope_head = slope = zllf_new();
  slope->prev = 0;
  slope->x = function->x;
  slope->y = 1.0f;				/*  assumes a slope of 1 on entry   */
  
  for (;function->next;function = function->next)
    {
      slope->next = zllf_new();
      slope->next->prev = slope;
      slope = slope->next;
      slope->x = function->next->x;
      slope->y = (function->next->y - function->y) / (function->next->x - function->x);
    }

  slope->next = 0;

  return slope_head;
}

static int
zll_relmaxmin(float slope1_left, float slope1_right, float slope2)
{
  if (slope1_left <= slope2)
    {
      if (slope1_right > slope2)
	return 1;
      else
	return 0;
    }

  if (slope1_right <= slope2)
    return 1;

  return 0;
}

static int zll_seek(struct zll *left, struct zll *right, struct zllf *slope_ptr, float ray_slope, float threshold);

static struct zll *
zll_hop1(struct zll *function, float precision)
{
  struct zll *outfun, *outfun_head;
  struct zll *ray_ptr = function->next;
  struct zllf *killptr, *slopes; 
  long x1,x2,y1,y2,x2_x1,y2_y1;
  float ray_slope, threshold;
  
  outfun = outfun_head = zll_new(); /* always  keep the first point  */
  outfun->prev = 0;
  outfun->x = function->x;
  outfun->y = function->y;
  killptr = slopes = zll_getslope(function); 
  
  for (; ray_ptr->next; ray_ptr = ray_ptr->next)
    {
      x1 = function->x;
      x2 = ray_ptr->next->x;
      y1 = function->y;
      y2 = ray_ptr->next->y;
      x2_x1 = x2 - x1;
      y2_y1 = y2 - y1;
      ray_slope = (float)(y2_y1 / x2_x1);
      threshold = precision * (float)sqrt((double)((x2_x1 * x2_x1) + (y2_y1 * y2_y1)));
    
      if ( zll_seek(function, ray_ptr->next, slopes->next, ray_slope, threshold))
	{
	  outfun->next = zll_new(); /* append newval and point to end of list  */
	  outfun->next->prev = outfun; /* build outfun from left to right   */
	  outfun = outfun->next;
	  outfun->next = 0;
	  outfun->x = ray_ptr->x;
	  outfun->y = ray_ptr->y;
      
	  for (; x2 > slopes->next->x; slopes = slopes->next)
	    ;
	  function = ray_ptr;
	}
    } 

  /*  no ray_ptr->next  so return  */
  outfun->next = zll_new();	/* append last point to end of output fun */
  outfun->next->prev = outfun;	/*  build outfun from left to right  */
  outfun = outfun->next;
  outfun->next = 0;
  outfun->x = ray_ptr->x;
  outfun->y = ray_ptr->y;
  zllf_freefun(killptr);

  return(outfun_head); /*  finally return pointer to reduced data */
}


static int
zll_seek(struct zll *left, struct zll *right, struct zllf *slope_ptr, float ray_slope, float threshold)
{
  struct zll *current_point = left->next;
  int flag;
  
  for (; current_point->x < right->x; current_point = current_point->next, slope_ptr = slope_ptr->next)
    {
      flag = zll_relmaxmin(slope_ptr->y, slope_ptr->next->y, ray_slope);
      if (flag)
	{
	  if (threshold < zll_distance(current_point->x,
				       current_point->y,
				       left->x,
				       left->y,
				       right->x,
				       right->y))
	    return 1;
	}
    }

  return 0;
}


static struct zll *
zll_reduce(struct zll *zllist, float tolerance)
{
  struct zll *reduced_list;
  
  if (!zllist) 
    {
      post("no data, quoi\n");
      return 0;
    }
  
  if (!zllist->next)
    {
      post("too short to reduce, quoi\n");
      return 0;
    }

  reduced_list = zll_hop1(zllist, tolerance);

  return reduced_list;
}

/*    ++++++++++++++++++++++++++++++++++++++++++FUN-BUFFER OBJECT DEFINITION */

static struct zll *funbuff_global_undo = 0;	/*  GLOBAL funbuff undobuffer   */
static struct zll *funbuff_global_clip = 0;	/*  GLOBAL funbuff clipboard   */

/* select region x and width */
/* method select, inlet 0 */
static void
funbuff_select(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  funbuff_t *this = (funbuff_t *)o;
  long sel_x, sel_width;

  if (ac < 2)
    return;

  sel_x = (long) fts_get_int_arg(ac, at, 0, 0);
  sel_width = (long) fts_get_int_arg(ac, at, 1, 0);

  if (sel_width < 0)
    {
      /* don't change the selection if there is an error */
      post("funbuff select:  arg2 (width)  must be non-negative\n");
    }
  
  this->selectX = sel_x;
  this->selectW = sel_width;
}


/* returns next element and a difference value between its x value and the x value of the 
   event just before; UNLESS "GOTO n" has been called, in which case 
   the difference value is the x of next event - n */ 
/* method next, inlet 0 */
static void
funbuff_next(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  funbuff_t *this = (funbuff_t *)o;
  long delta_x, y;
  
  if (! funbuff_first(this))
    return;  /* funbuff:  no data, quoi  */

  if (!this->foot)
    {				
      /*  funbuff:   at end of data   send bang out outlet1 */

      fts_outlet_bang((fts_object_t *)this, 2);
      return;
    }

  if (this->foot == funbuff_first(this))
    {	
      delta_x = (this->gotoDelta > -1) ? this->gotoDelta : this->foot->x;
      y = this->foot->y;
    }
  else
    {
      delta_x = (this->gotoDelta > -1) ? this->gotoDelta : this->foot->x - this->foot->prev->x;
      y = this->foot->y;
    }

  this->foot = this->foot->next;
  this->gotoDelta = -1;

  fts_outlet_int((fts_object_t *)this, 1, delta_x);
  fts_outlet_int((fts_object_t *)this, 0, y);
}

/* warning: the name lies, the function have also side effects on this */
static struct zll *
funbuff_getElement(funbuff_t *this, long int index)
{
  struct zll *list = this->foot ? this->foot : funbuff_first(this); 
  
  if (!funbuff_first(this))
    return(0);

  if (index < list->x)
    {
      list = list->prev;

      for (;list->prev;list = list->prev)
	if (index >= list->x)
	  {
	    this->foot = list;
	    return list;
	  }

      this->foot = list->next;
      return list->next;
    }

  for (;list->x <= index;list = list->next)
    if (!list->next)
      {
	this->foot = list;
	return list;
      }

  this->foot = list->prev;
  return list->prev;
}

/*  works in conjunction with "NEXT".  Using "GOTO n",  the next call to "NEXT" will be 
    relative to N instead of the x value of the previous event returned by NEXT"  */

/* method goto, inlet 0 */
static void
funbuff_goto(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  funbuff_t *this = (funbuff_t *)o;
  long int index = fts_get_int(at);

  if (! funbuff_first(this))
    return;
  
  funbuff_getElement(this, index);

  if (index <= this->foot->x)
    this->gotoDelta = this->foot->x - index;
  else 
    {
      this->foot = this->foot->next;
      if (this->foot)
	this->gotoDelta = this->foot->x - index;
      else this->gotoDelta = 0;
    }
}

/* support function: returns overwriten list  or 0. list must be 0 terminated  */

static struct zll *
funbuff_write(funbuff_t *this, struct zll *list)		
{
  struct zll *current;
  struct zll *kill, *splice1, *splice2 = 0, *start, *end;
  long x1, x2;
  
  if (!list)
    { 
      post("funbuff_write: no data to write\n");
      return(0);
    }
  
  start = list;
  end = zll_last(list);
  x1 = start->x;
  x2 = end->x;

  if (! funbuff_first(this))
    {
      /* if funbuff is empty then attach list     */
      this->head.next = list;
      list->prev = &this->head;
      this->foot = end;
      return 0;
    }

  for (current = funbuff_getElement(this, x1); current->next; current = current->next)
    if (current->x >= x1)
      break;

  this->foot = end;

  if (x1 > current->x)
    {
      /*  append list  */
      current->next = list;
      list->prev = current;
      return 0;
    }
  
  /*   insert list start   */
  splice1 = current->prev;		
  splice1->next = list;
  list->prev = splice1;

  if (x1 < current->x  &&  x2 < current->x)
    {
      /*  insert list end    no overwrite  */
      end->next = current;
      current->prev = end;
      return 0;
    }

  kill = current;
  for (; current; current = current->next)
    {
      if (current->x > x2)
	{
	  splice2 = current;
	  break;
	} 
    }

  if (splice2)
    {
      /*     insert list end  and 0 cap overwrite   */
      splice2->prev->next = 0;
      end->next = splice2;
      splice2->prev = end;
    }

  return(kill);
}

/* method int/float, inlet 1 */
static void
funbuff_number_1(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  funbuff_t *this = (funbuff_t *)o;

  this->y = (long) fts_get_int_arg(ac, at, 0, 0);
  this->yvalid = 1;
}

/* sends nearest value (<=)  index  or lowest val in buffer  */
/* method int/float, inlet 0 */
static void
funbuff_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  funbuff_t *this = (funbuff_t *)o;
  long int index = (long) fts_get_int_arg(ac, at, 0, 0);

  if (this->yvalid)
    {
      struct zll *list, *element = zll_new();
      element->x = index;
      element->y = this->y; 
      element->next = 0;
      
      list = funbuff_write(this, element);
      zll_freefun(list);
      this->yvalid = 0;
    }
  else
    {
      struct zll *list;
      list  = funbuff_getElement(this, index);
      if (list)	
	fts_outlet_int((fts_object_t *)this, 0, list->y);
    }
}


/* method interp, inlet 0 */
static void
funbuff_interp(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  funbuff_t *this = (funbuff_t *)o;
  long int n = fts_get_int(at);
  struct zll *z = funbuff_getElement(this,n), *z2;

  if (!z)
    fts_outlet_int((fts_object_t *)this, 0, 0L);
  else if (n <= z->x)
    fts_outlet_int((fts_object_t *)this, 0, z->y);
  else
    {
      long x2, y2;

      for (; ((z2 = z->next)); z = z->next)
	if (n <= (x2 = z2->x))
	  {
	    long dx = x2 - z->x, dy = (y2 = z2->y) - z->y;

	    fts_outlet_int((fts_object_t *)this, 0, y2 - ((x2 - n)*dy)/dx);
	    return;
	  }

      fts_outlet_int((fts_object_t *)this, 0, z->y);
    }
}

/* uses the function described in the table named by the symbol in the message-  this method 
   checks each time the length and max/min vals of the table SO the function in the tabled
   is always relative to itself,  that is it is only a shape   */

/* method interptab, inlet 0 */
static void
funbuff_interptab(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  funbuff_t *this = (funbuff_t *)o;
  
  if(ac == 2 && fts_is_int(at) && fts_is_symbol(at + 1))
    {
      int xn  = fts_get_int(at + 0);
      fts_symbol_t sym = fts_get_symbol(at + 1);
      struct zll *z;
      long x1, x2, tab_domain, tab_range, tab_x, tab_y;
      long  low = 0, high = 0;
      ivec_t *tab = 0;
      fts_object_t *obj;
      
      obj = ispw_get_object_by_name(sym);
      
      if(obj && (fts_object_get_class_name(obj) == ivec_symbol))
	tab = (ivec_t *)obj;
      
      if (! tab)
	{
	  post("funbuff interptab: %s is not a known table\n", sym);
	  return;
	}
      
      z = funbuff_getElement(this,xn);
      
      if (! z)
	{
	  /*  no data in funbuff output zero */
	  fts_outlet_int((fts_object_t *)this, 0, 0L);
	  return;
	}
      else if ((xn <= z->x) || (!z->next)) 
	{
	  /*  no data in funbuff output zero?????  */
	  fts_outlet_int((fts_object_t *)this, 0, z->y);
	  return;
	}
      
      tab_domain = ivec_get_size(tab);
      low  = ivec_get_min_value(tab);
      high = ivec_get_max_value(tab);
      tab_range = high - low;
      
      if (tab_range == 0)
	{
	  post("funbuff_interptab: Table must be non zero\n");
	  return;
	}
      
      x1 = z->x;
      x2 = z->next->x;
      
      tab_x = tab_domain * ((xn - x1) / (float)(x2 - x1));
      tab_y = ivec_get_element(tab, tab_x) - low;
      xn = z->y + (z->next->y - z->y) * (float)tab_y / tab_range;
      
      fts_outlet_int((fts_object_t *)this, 0, xn);
    }
}


/* method set, inlet 0 */
static void
funbuff_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  funbuff_t *this = (funbuff_t *)o;
  struct zll *list, *overwrite;

  list = zll_vecToZll(ac, at);

  if (! list)
    {
      post("funbuff_set:  no data to set\n");
      return;
    }

  overwrite = funbuff_write(this, list);
  this->foot = funbuff_first(this);

  if (overwrite)
    {
      if (funbuff_global_undo)
	zll_freefun(funbuff_global_undo);

      funbuff_global_undo = overwrite;
    }
}


/* method find, inlet 0 */
static void
funbuff_find(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  funbuff_t *this = (funbuff_t *)o;
  long int yval   = fts_get_int(at);
  struct zll *list;
  
  if (! funbuff_first(this))
    {
      post("funbuff_find: nothing to find\n");
      return;
    }
  
  for(list = funbuff_first(this); list; list = list->next)
    if (list->y == yval)
      fts_outlet_int((fts_object_t *)this, 0, list->x);
}


/*  returns *copy of selected region of list or 0  */
/* method copy, inlet 0 */
static void
funbuff_copy(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  funbuff_t *this = (funbuff_t *)o;
  struct zll *copy;	
  
  copy = zll_copy(funbuff_first(this), this->selectX, this->selectW);

  if (funbuff_global_clip)
    zll_freefun(funbuff_global_clip);

  funbuff_global_clip = copy;
}



/* cuts zll in selected region returns cut zll or 0  */
/* method cut, inlet 0 */
static void
funbuff_cut(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  funbuff_t *this = (funbuff_t *)o;
  struct zll *head;
  
  this->foot = 0;
  
  head = zll_cut(funbuff_first(this), this->selectX, this->selectW);

  if (!head)
    {
      post("funbuff_cut:  no data in selected region\n");
      return;
    }

  if (funbuff_global_clip)
    zll_freefun(funbuff_global_clip);

  funbuff_global_clip = head;
}


/* method clear, inlet 0 */
/* clears zll in selected region */
static void
funbuff_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  funbuff_t *this = (funbuff_t *)o;
  struct zll *list;
  
  this->foot = 0;
  list = zll_cut(funbuff_first(this), this->selectX, this->selectW);
  zll_freefun(list);
}

/* method reduce, inlet 0 */
/* reduces zll in selected region */
static void
funbuff_reduce(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  funbuff_t *this = (funbuff_t *)o;
  float  factor = fts_get_float(at);
  struct zll *newlist, *oldlist;
  
  this->foot = 0;
  
  if (! funbuff_first(this))
    {
      post("funbuff_reduce: buffer empty\n");
      return;
    }

  oldlist = zll_cut(funbuff_first(this), this->selectX, this->selectW);

  if (! oldlist)
    {
      post("funbuff_reduce: no data to reduce in selected region\n");
      return;
    }
  
  newlist = zll_reduce(oldlist, factor);
  funbuff_write(this, newlist);

  if (funbuff_global_undo)
    zll_freefun(funbuff_global_undo);

  funbuff_global_undo = oldlist;
}


/* method undo, inlet 0 */
/* takes contents of global undo buffer and writes it into funbuff*/
static void
funbuff_undo(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  funbuff_t *this = (funbuff_t *)o;
  struct zll *relist;
  
  if (!funbuff_global_undo)
    return;

  relist = funbuff_global_undo;
  funbuff_global_undo = funbuff_write(this, relist);
}


/* pastes a copy corresponding to my selected region from clipboard into myself */
/* method paste, inlet 0 */
static void
funbuff_paste(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  funbuff_t *this = (funbuff_t *)o;
  struct zll *list, *overwrite;
  
  if (! funbuff_global_clip)
    {
      post("funbuff: clipboard empty\n");
      return;
    }
      
  list = zll_copy(funbuff_global_clip, this->selectX, this->selectW);
  overwrite = funbuff_write(this, list);

  if (funbuff_global_undo)
    zll_freefun(funbuff_global_undo);

  funbuff_global_undo = overwrite;
}

/* method bang inlet 0 */
/*    DEBUG HERE +++++++++++++++++++++++++++++++++   */
static void
funbuff_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  funbuff_t *this = (funbuff_t *)o;

  post("funbuff info:    %ld elements long\n", zll_length(funbuff_first(this)));

  if (! funbuff_first(this))
    return;
  else
    {
      struct zll *list = funbuff_first(this);
      long lowX, highX, lowY, highY;
	
      lowX = highX = list->x ,lowY = highY = list->y;
      for(list = list->next;list;list = list->next)
	{
	  if(list->x < lowX)
	    lowX = list->x;
	  else if (list->x > highX)
	    highX = list->x;
	  if(list->y < lowY)
	    lowY = list->y;
	  else if (list->y > highY)
	    highY = list->y;
	  }

      post("	->	minX= %ld maxX= %ld\n", lowX, highX);
      post("	->  minY = %ld maxY = %ld\n", lowY, highY);
      post("	->	domain= %ld range = %ld\n", highX - lowX, highY - lowY  );
    }
}

/* Method init, system inlet; args = optional symbol */
/* should have a method delete !!! ??? !!! */
static void
funbuff_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  funbuff_t *this = (funbuff_t *)o;

  this->head.prev = 0;
  this->head.next = 0;
  this->head.x = -0x0FFFFFFFF;
  this->head.y = -0x0FFFFFFFF;
  this->selectX = -0x040000000;
  this->selectW = 0x07FFFFFFF;
  this->foot = 0;
  this->yvalid = 0;
  this->gotoDelta = -1L;
}


static void
funbuff_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(funbuff_t), funbuff_init, 0);

  fts_class_message_varargs(cl, fts_s_set, funbuff_set);

  fts_class_inlet_bang(cl, 0, funbuff_bang);
  fts_class_inlet_int(cl, 0, funbuff_number);
  fts_class_inlet_float(cl, 0, funbuff_number);

  fts_class_message_varargs(cl, fts_new_symbol("next"), funbuff_next);
  fts_class_message_varargs(cl, fts_new_symbol("goto"), funbuff_goto);
  fts_class_message_varargs(cl, fts_new_symbol("reduce"), funbuff_reduce);
  fts_class_message_varargs(cl, fts_new_symbol("interp"), funbuff_interp);

  fts_class_message_varargs(cl, fts_s_clear, funbuff_clear);
  fts_class_message_varargs(cl, fts_new_symbol("interptab"), funbuff_interptab);
  fts_class_message_varargs(cl, fts_new_symbol("cut"), funbuff_cut);
  fts_class_message_varargs(cl, fts_new_symbol("copy"), funbuff_copy);
  fts_class_message_varargs(cl, fts_new_symbol("paste"), funbuff_paste);
  fts_class_message_varargs(cl, fts_new_symbol("find"), funbuff_find);
  fts_class_message_varargs(cl, fts_new_symbol("undo"), funbuff_undo);

  fts_class_message_varargs(cl, fts_new_symbol("select"), funbuff_select);

  fts_class_inlet_int(cl, 1, funbuff_number_1);
  fts_class_inlet_float(cl, 1, funbuff_number_1);

  fts_class_outlet_int(cl, 0);
  fts_class_outlet_int(cl, 1);
  fts_class_outlet_bang(cl, 2);
}

void
funbuff_config(void)
{
  fts_class_install(fts_new_symbol("funbuff"),funbuff_instantiate);
}

