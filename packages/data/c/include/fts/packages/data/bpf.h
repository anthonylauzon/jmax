/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

#ifndef _DATA_BPF_H_
#define _DATA_BPF_H_

#include <fts/packages/data/data.h>


/** 
 *  @file bpf.h
 *  @brief break point functions
 *  @ingroup data
 *  @ingroup  fts_classes
 *
 *  break point functions
 */

/** @defgroup bpf bpf: break point functions
 *  @ingroup  data
 *  @ingroup  fts_classes
 *  
 *  break point functions
 */

/*************************************************
 *
 *  single break point
 *
 */

/**
 * @typedef struct _bp_ bp_t
 * @brief single break point  
 * @ingroup bpf
 */
/**
 * @struct _bp_
 * @brief single break point struct
 * @ingroup bpf
 */
typedef struct _bp_
{
  double time; /**< absolute break point time */
  double value; /**< break point value */
  double slope; /**< slope to next value */
} bp_t;

/*************************************************
 *
 *  break point function (array of break points)
 *
 */
/**
 * @typedef struct _bpf_ bpf_t
 * @brief break point function (array of break points) 
 * @ingroup bpf
 */
/**
 * @struct _bpf_
 * @brief break point function (array of break points) struct
 * @ingroup bpf
 */
typedef struct _bpf_
{
  fts_object_t o; /**< fts_object ... */
  bp_t *points; /**< break points ... */
  int alloc; /**< alloc ... */
  int size; /**< size ... */
  int index; /**< index cache for get_interpolated method */
  int opened; /**< non zero if editor open */
} bpf_t;

/** 
 * @var fts_symbol_t bpf_symbol
 * @brief bpf_symbol ... 
 * @ingroup bpf 
 */
DATA_API fts_symbol_t bpf_symbol;
/** 
 * @var fts_class_t *bpf_class
 * @brief bpf class ... 
 * @ingroup bpf 
 */
DATA_API fts_class_t *bpf_class;
/**
 * @def bpf_type bpf_class
 * @brief bpf type
 * @ingroup bpf
 */
#define bpf_type bpf_class
 
#ifdef AVOID_MACROS
/**
 * @fn double bpf_get_interpolated(bpf_t *bpf, double time)
 * @brief get interpolated ...
 * @param bpf the bpf ...
 * @param time the time ...
 * @return ....
 * @ingroup bpf
 */
int bpf_get_size(bpf_t *bpf);
/**
 * @fn double bpf_get_time(bpf_t *bpf, int index)
 * @brief get time of point at given index 
 * @param bpf the bpf
 * @param index point index
 * @return time of point at given index
 * @ingroup bpf
 */
double bpf_get_time(bpf_t *bpf, int index);
/**
 * @fn double bpf_get_value(bpf_t *bpf, int index)
 * @brief get value of point at given index 
 * @param bpf the bpf
 * @param index point index
 * @return value of point at given index
 * @ingroup bpf
 */
double bpf_get_value(bpf_t *bpf, int index);
/**
 * @fn double bpf_get_slope(bpf_t *bpf, int index)
 * @brief get slope of point at given index 
 * @param bpf the bpf
 * @param index point index
 * @return slope of point at given index
 * @ingroup bpf
 */
double bpf_get_slope(bpf_t *bpf, int index);
/**
 * @fn double bpf_get_duration(bpf_t *bpf)
 * @brief get bpf duration  
 * @param bpf the bpf
 * @return bpf duration
 * @ingroup bpf
 */
double bpf_get_duration(bpf_t *bpf);
/**
 * @fn double bpf_get_target(bpf_t *bpf)
 * @brief get bpf target  
 * @param bpf the bpf
 * @return bpf target
 * @ingroup bpf
 */
double bpf_get_target(bpf_t *bpf);
#else
#define bpf_get_size(b) ((b)->size)

#define bpf_get_time(b, i) ((b)->points[i].time)
#define bpf_get_value(b, i) ((b)->points[i].value)
#define bpf_get_slope(b, i) ((b)->points[i].slope)

#define bpf_get_duration(b) ((b)->size > 0? (b)->points[(b)->size - 1].time: 0.0)
#define bpf_get_target(b) ((b)->size > 0? (b)->points[(b)->size - 1].value: 0.0)
#endif
/**
 * @fn double bpf_get_interpolated(bpf_t *bpf, double time)
 * @brief get interpolated ...
 * @param bpf the bpf ...
 * @param time the time ...
 * @return ....
 * @ingroup bpf
 */
DATA_API double bpf_get_interpolated(bpf_t *bpf, double time);
/**
 * @fn void bpf_clear(bpf_t *bpf)
 * @brief clear bpf ...
 * @param bpf the bpf ...
 * @ingroup bpf
 */
DATA_API void bpf_clear(bpf_t *bpf);
/**
 * @fn void bpf_copy(bpf_t *bpf, bpf_t *copy)
 * @brief copy first bpf content to second bpf
 * @param bpf the original bpf ...
 * @param copy the bpf copy
 * @ingroup bpf
 */
DATA_API void bpf_copy(bpf_t *bpf, bpf_t *copy);
/**
 * @fn void bpf_append_point(bpf_t *bpf, double time, double value)
 * @brief append new point to bpf
 * @param bpf the bpf
 * @param time time of new point
 * @param value value of new point
 * @ingroup bpf
 */
DATA_API void bpf_append_point(bpf_t *bpf, double time, double value);
/**
 * @fn void bpf_set_point(bpf_t *bpf, int index, double time, double value)
 * @brief set given point in bpf
 * @param bpf the bpf
 * @param index the point index
 * @param time new time
 * @param value new value
 * @ingroup bpf
 */
DATA_API void bpf_set_point(bpf_t *bpf, int index, double time, double value);
/**
 * @fn void bpf_insert_point(bpf_t *bpf, double time, double value)
 * @brief insert new point in bpf
 * @param bpf the bpf
 * @param time time of new point
 * @param value value of new point
 * @ingroup bpf
 */
DATA_API void bpf_insert_point(bpf_t *bpf, double time, double value);
/**
 * @fn void bpf_remove_points(bpf_t *bpf, int index, int n)
 * @brief remove n points in bpf from given index 
 * @param bpf the bpf
 * @param index first removed point index
 * @param n removed points count
 * @ingroup bpf
 */
DATA_API void bpf_remove_points(bpf_t *bpf, int index, int n);
/**
 * @fn void bpf_simplify(bpf_t *bpf, double time, double value)
 * @brief bpf simplify ... 
 * @param bpf the bpf
 * @param time ...
 * @param value ...
 * @ingroup bpf
 */
DATA_API void bpf_simplify(bpf_t *bpf, double time, double value);

#endif
