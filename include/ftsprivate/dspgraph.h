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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * 
 */

#ifndef _FTS_PRIVATE_DSPGRAPH_H_
#define _FTS_PRIVATE_DSPGRAPH_H_

FTS_API fts_dsp_signal_t *sig_zero;

/*************************************************
 *
 *  DSP graph node
 *
 */

/*************************************************
 *
 *  DSP graph
 *
 */

typedef struct _fts_dsp_graph_
{
  enum {status_reset, status_compiled} status;

  /* the objects */
  fts_dsp_object_t *objects;

  /* the dsp chain */
  ftl_program_t *chain;

  /* table of signal connections */
  fts_signal_connection_table_t signal_connection_table;

  /* DSP parameters */
  int tick_size;
  double sample_rate;

} fts_dsp_graph_t;

#define fts_dsp_graph_is_compiled(g) ((g)->status == status_compiled)

#define fts_dsp_graph_get_tick_size(g) ((g)->tick_size)
#define fts_dsp_graph_get_sample_rate(g) ((g)->sample_rate)

#define fts_dsp_graph_set_tick_size(g, x) ((g)->tick_size = (x))
#define fts_dsp_graph_set_sample_rate(g, x) ((g)->sample_rate = (x))

#define fts_dsp_graph_get_time(g) ((g)->time)

extern void fts_dsp_graph_init(fts_dsp_graph_t *graph, int vector_size, double sample_rate);
extern void fts_dsp_graph_compile(fts_dsp_graph_t *graph);
extern void fts_dsp_graph_reset(fts_dsp_graph_t *graph);
extern void fts_dsp_graph_run(fts_dsp_graph_t *graph);

extern void fts_dsp_graph_add_object(fts_dsp_graph_t *graph, fts_dsp_object_t *obj);
extern void fts_dsp_graph_remove_object(fts_dsp_graph_t *graph, fts_dsp_object_t *obj);

#endif
