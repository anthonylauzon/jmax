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

#ifndef _SEQUENCE_scoob_H_
#define _SEQUENCE_scoob_H_

#include <fts/fts.h>
#include <fts/packages/data/data.h>
#include <fts/packages/sequence/event.h>

/** 
 * @defgroup scoob scoob: score object
 *  @ingroup  data
 *  @ingroup  fts_classes
 *  
 *  score object (generalization of note)
 */

/**
 * @typedef struct _scoob_ scoob_t
 * @brief score object (generalization of note)
 * @ingroup scoob
 */
/**
 * @struct _scoob_
 * @brief scoob struct
 * @ingroup scoob
 */
typedef struct _scoob_
{
  propobj_t propobj;/**< propobj ... */
  fts_symbol_t type;/**< scoob type ... */
  double pitch;/**< pitch ... */
  double interval;/**< interval ... */
  double duration;/**< duration ... */
} scoob_t;

/**
 * @enum scoob_type_enum
 * @brief scoob types enum
 * @ingroup scoob
 */
enum scoob_type_enum
{
  scoob_note = 0,/**< note type */
  scoob_interval,/**< interval type */
  scoob_rest,/**< rest type */
  scoob_trill,/**< trill type */
  scoob_unvoiced/**< unvoiced type */
};
/**
 * @enum scoob_propidx_enum
 * @brief scoob propidx enum
 * @ingroup scoob
 */
enum scoob_propidx_enum
{
  scoob_propidx_velocity = 0, /**< scoob_propidx_velocity ... */
  scoob_propidx_channel /**< scoob_propidx_channel ... */
};

/** 
 * @var fts_class_t *scoob_class
 * @brief scoob_class ... 
 * @ingroup scoob 
 */
FTS_API fts_class_t *scoob_class;
/** 
 * @var enumeration_t *scoob_type_enumeration
 * @brief scoob_type_enumeration ... 
 * @ingroup scoob 
 */
FTS_API enumeration_t *scoob_type_enumeration;

#ifdef AVOID_MACROS
/** 
 * @fn void scoob_set_type(scoob_t *scoob, fts_symbol_t type)
 * @brief set scoob type
 * @param scoob the scoob
 * @param type scoob type
 * @ingroup scoob 
 */
void scoob_set_type(scoob_t *scoob, fts_symbol_t type);
/** 
 * @fn fts_symbol_t scoob_get_type(scoob_t *scoob)
 * @brief get scoob type 
 * @param scoob the scoob
 * @return scoob type
 * @ingroup scoob 
 */
fts_symbol_t scoob_get_type(scoob_t *scoob);

/** 
 * @fn void scoob_set_pitch(scoob_t *scoob, double pitch)
 * @brief set scoob pitch
 * @param scoob the scoob
 * @param pitch scoob pitch
 * @ingroup scoob 
 */
void scoob_set_pitch(scoob_t *scoob, double pitch);
/** 
 * @fn double scoob_get_pitch(scoob_t *scoob)
 * @brief get scoob pitch 
 * @param scoob the scoob
 * @return scoob pitch
 * @ingroup scoob 
 */
double scoob_get_pitch(scoob_t *scoob);

/** 
 * @fn void scoob_set_interval(scoob_t *scoob, double interval)
 * @brief set scoob interval
 * @param scoob the scoob
 * @param interval scoob interval
 * @ingroup scoob 
 */
void scoob_set_interval(scoob_t *scoob, double interval);
/** 
 * @fn double scoob_get_interval(scoob_t *scoob)
 * @brief get scoob interval 
 * @param scoob the scoob
 * @return scoob interval
 * @ingroup scoob 
 */
double scoob_get_interval(scoob_t *scoob);

/** 
 * @fn void scoob_set_duration(scoob_t *scoob, double duration)
 * @brief set scoob duration
 * @param scoob the scoob
 * @param duration scoob duration
 * @ingroup scoob 
 */
void scoob_set_duration(scoob_t *scoob, double duration);
/** 
 * @fn double scoob_get_duration(scoob_t *scoob)
 * @brief get scoob duration 
 * @param scoob the scoob
 * @return scoob duration
 * @ingroup scoob 
 */
double scoob_get_duration(scoob_t *scoob);
#else
#define scoob_set_type(s, x) ((s)->type = (x))
#define scoob_get_type(s) ((s)->type)

#define scoob_set_pitch(s, x) ((s)->pitch = (x))
#define scoob_get_pitch(s) ((s)->pitch)

#define scoob_set_interval(s, x) ((s)->interval = (x))
#define scoob_get_interval(s) ((s)->interval)

#define scoob_set_duration(s, x) ((s)->duration = (x))
#define scoob_get_duration(s) ((s)->duration)
#endif

/** 
 * @fn void scoob_set_velocity(scoob_t *self, int velocity)
 * @brief set scoob velocity 
 * @param scoob the scoob
 * @param velocity new velocity
 * @ingroup scoob 
 */
FTS_API void scoob_set_velocity(scoob_t *self, int velocity);
/** 
 * @fn int scoob_get_velocity(scoob_t *self)
 * @brief get scoob velocity 
 * @param scoob the scoob
 * @return scoob velocity
 * @ingroup scoob 
 */
FTS_API int scoob_get_velocity(scoob_t *self);
/** 
 * @fn void scoob_set_channel(scoob_t *self, int channel)
 * @brief set scoob channel 
 * @param scoob the scoob
 * @param channel new channel
 * @ingroup scoob 
 */
FTS_API void scoob_set_channel(scoob_t *self, int channel);
/** 
 * @fn int scoob_get_channel(scoob_t *scoob)
 * @brief get scoob channel 
 * @param scoob the scoob
 * @return scoob channel
 * @ingroup scoob 
 */
FTS_API int scoob_get_channel(scoob_t *scoob);

#ifdef AVOID_MACROS
/** 
 * @fn int scoob_get_type_index(scoob_t *scoob)
 * @brief get index in scoob_type_enum of scoob type 
 * @param scoob the scoob
 * @return index in scoob_type_enum
 * @ingroup scoob 
 */
int scoob_get_type_index(scoob_t *scoob);
/** 
 * @fn void scoob_set_type_by_index(scoob_t *scoob, int index)
 * @brief set scoob type by index in scoob_type_enum
 * @param scoob the scoob
 * @param scoob type index in scoob_type_enum
 * @ingroup scoob 
 */
void scoob_set_type_by_index(scoob_t *scoob, int index);
#else
#define scoob_get_type_index(s) (enumeration_get_index(scoob_type_enumeration, (s)->type))
#define scoob_set_type_by_index(s, i) ((s)->type = enumeration_get_name(scoob_type_enumeration, (i)))
#endif

/* scoob default MIDI properties */
/** 
 * @fn void scoob_set_velocity(scoob_t *scoob, int velocity)
 * @brief set scoob velocity 
 * @param scoob the scoob
 * @param velocity new velocity
 * @ingroup scoob 
 */
FTS_API void scoob_set_velocity(scoob_t *scoob, int velocity);
/** 
 * @fn int scoob_get_velocity(scoob_t *scoob)
 * @brief get scoob velocity 
 * @param scoob the scoob
 * @return scoob velocity
 * @ingroup scoob 
 */
FTS_API int scoob_get_velocity(scoob_t *scoob);
/** 
 * @fn void scoob_set_channel(scoob_t *scoob, int channel)
 * @brief set scoob channel 
 * @param scoob the scoob
 * @return channel new channel
 * @ingroup scoob 
 */
FTS_API void scoob_set_channel(scoob_t *scoob, int channel);
/** 
 * @fn int scoob_get_channel(scoob_t *scoob)
 * @brief get scoob channel 
 * @param scoob the scoob
 * @return scoob channel
 * @ingroup scoob 
 */
FTS_API int scoob_get_channel(scoob_t *scoob);

/* scoob properties */
#ifdef AVOID_MACROS
/** 
 * @fn void scoob_property_get(scoob_t *scoob, fts_atom_t name, fts_atom_t *ret)
 * @brief get scoob property by name 
 * @param scoob the scoob
 * @param name property name
 * @param ret fts_atom to store return value
 * @ingroup scoob 
 */
void scoob_property_get(scoob_t *scoob, fts_atom_t name, fts_atom_t *ret);
/** 
 * @fn void scoob_property_set(scoob_t *scoob, fts_atom_t name, const fts_atom_t *value)
 * @brief set scoob property by name 
 * @param scoob the scoob
 * @param name property name
 * @param value new property value
 * @ingroup scoob 
 */
void scoob_property_set(scoob_t *scoob, fts_atom_t name, const fts_atom_t *value);

/** 
 * @fn void scoob_property_get_by_index(scoob_t *scoob, int index, fts_atom_t *ret)
 * @brief get scoob property by index 
 * @param scoob the scoob
 * @param index property index
 * @param ret fts_atom to store return value
 * @ingroup scoob 
 */
void scoob_property_get_by_index(scoob_t *scoob, int index, fts_atom_t *ret);
/** 
 * @fn void scoob_property_set_by_index(scoob_t *scoob, int index, const fts_atom_t *value)
 * @brief set scoob property by index 
 * @param scoob the scoob
 * @param index property index
 * @param value new property value
 * @ingroup scoob 
 */
void scoob_property_set_by_index(scoob_t *scoob, int index, const fts_atom_t *value);
#else
#define scoob_property_get(s, n, p) propobj_get_property_by_name((propobj_t *)(s), (n), (p))
#define scoob_property_set(s, n, v) propobj_set_property_by_name((propobj_t *)(s), (n), (v))

#define scoob_property_get_by_index(s, i, p) propobj_get_property_by_index((propobj_t *)(s), (i), (p))
#define scoob_property_set_by_index(s, i, v) propobj_set_property_by_index((propobj_t *)(s), (i), (v))
#endif

/** 
 * @fn void scoob_copy(scoob_t *org, scoob_t *copy)
 * @brief copy content from org scoob to copy scoob 
 * @param scoob original scoob
 * @param copy copy scoob
 * @ingroup scoob 
 */
void scoob_copy(scoob_t *org, scoob_t *copy);

#endif
