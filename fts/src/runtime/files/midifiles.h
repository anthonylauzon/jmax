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
 * These MIDI file functions are base on the midifilelib code by Tim Thompson and Michael Czeiszperger.
 *
 */
/* MIDI status commands most significant bit is 1 */
#define note_off         	0x80
#define note_on          	0x90
#define poly_aftertouch  	0xa0
#define control_change    	0xb0
#define program_chng     	0xc0
#define channel_aftertouch      0xd0
#define pitch_wheel      	0xe0
#define system_exclusive      	0xf0
#define delay_packet	 	(1111)

/* 7 bit controllers */
#define damper_pedal            0x40
#define portamento	        0x41 	
#define sostenuto	        0x42
#define soft_pedal	        0x43
#define general_4               0x44
#define	hold_2		        0x45
#define	general_5	        0x50
#define	general_6	        0x51
#define general_7	        0x52
#define general_8	        0x53
#define tremolo_depth	        0x5c
#define chorus_depth	        0x5d
#define	detune		        0x5e
#define phaser_depth	        0x5f

/* parameter values */
#define data_inc	        0x60
#define data_dec	        0x61

/* parameter selection */
#define non_reg_lsb	        0x62
#define non_reg_msb	        0x63
#define reg_lsb		        0x64
#define reg_msb		        0x65

/* Standard MIDI Files meta event definitions */
#define	meta_event		0xFF
#define	sequence_number 	0x00
#define	text_event		0x01
#define copyright_notice 	0x02
#define sequence_name    	0x03
#define instrument_name 	0x04
#define lyric	        	0x05
#define marker			0x06
#define	cue_point		0x07
#define channel_prefix		0x20
#define	end_of_track		0x2f
#define	set_tempo		0x51
#define	smpte_offset		0x54
#define	time_signature		0x58
#define	key_signature		0x59
#define	sequencer_specific	0x74

/* Manufacturer's ID number */
#define Seq_Circuits (0x01) /* Sequential Circuits Inc. */
#define Big_Briar    (0x02) /* Big Briar Inc.           */
#define Octave       (0x03) /* Octave/Plateau           */
#define Moog         (0x04) /* Moog Music               */
#define Passport     (0x05) /* Passport Designs         */
#define Lexicon      (0x06) /* Lexicon 			*/
#define Tempi        (0x20) /* Bon Tempi                */
#define Siel         (0x21) /* S.I.E.L.                 */
#define Kawai        (0x41) 
#define Roland       (0x42)
#define Korg         (0x42)
#define Yamaha       (0x43)

/* miscellaneous definitions */
#define MThd 0x4d546864
#define MTrk 0x4d54726b
#define lowerbyte(x) ((unsigned char)(x & 0xff))
#define upperbyte(x) ((unsigned char)((x & 0xff00)>>8))

typedef struct _fts_midifile_ fts_midifile_t;

typedef struct _fts_midifile_read_functions_
{
  int (*header) (struct _fts_midifile_ *file);
  int (*trackstart)(struct _fts_midifile_ *file);
  int (*trackend)(struct _fts_midifile_ *file);
  int (*noteon)(struct _fts_midifile_ *file, int chan, int pitch, int vol);
  int (*noteoff)(struct _fts_midifile_ *file, int chan, int pitch, int vol);
  int (*pressure)(struct _fts_midifile_ *file, int chan, int pitch, int press);
  int (*controller)(struct _fts_midifile_ *file, int chan, int control, int value);
  int (*pitchbend)(struct _fts_midifile_ *file, int chan, int msb, int lsb);
  int (*program)(struct _fts_midifile_ *file, int chan, int program);
  int (*chanpressure)(struct _fts_midifile_ *file, int chan, int  press);
  int (*sysex)(struct _fts_midifile_ *file, int leng, char *mess);
  int (*arbitrary)(struct _fts_midifile_ *file, int leng, char *mess);
  int (*metamisc)(struct _fts_midifile_ *file, int type, int leng, char *mess);
  int (*seqnum)(struct _fts_midifile_ *file, int num);
  int (*eot)(struct _fts_midifile_ *file);
  int (*smpte)(struct _fts_midifile_ *file, int hr, int mn, int se, int fr, int ff);
  int (*tempo)(struct _fts_midifile_ *file);
  int (*timesig)(struct _fts_midifile_ *file, int nn, int dd, int cc, int bb);
  int (*keysig)(struct _fts_midifile_ *file, int sf, int mi);
  int (*seqspecific)(struct _fts_midifile_ *file, int type, int leng, char *mess);
  int (*text)(struct _fts_midifile_ *file, int type, int leng, char *mess);
}fts_midifile_read_functions_t;

typedef struct _fts_midifile_write_functions_
{
  int (*track)(struct _fts_midifile_ *file, int track);
  int (*tempotrack)(struct _fts_midifile_ *file);
} fts_midifile_write_functions_t;

struct _fts_midifile_
{
  FILE *fp;
  fts_symbol_t name;
  
  int format;
  int n_tracks;
  int division;
  int tempo;

  fts_midifile_read_functions_t *read;
  fts_midifile_write_functions_t *write;

  long currtime; /* current time in delta-time units */
  long toberead;
  long numbyteswritten;

  char *Msgbuff; /* message buffer */
  int Msgsize; /* Size of currently allocated Msg */
  int Msgindex; /* index of next available location in Msg */

  char *error; /* error message */

  void *user; /* user data */
};

#define fts_midifile_get_current_time(f) ((f)->currtime)
extern double fts_midifile_get_current_time_in_seconds(fts_midifile_t *file);
#define fts_midifile_set_read_functions(f, r) ((f)->read = (r))

#define fts_midifile_set_user_data(f, p) ((f)->user = (void *)(p))
#define fts_midifile_get_user_data(f) ((f)->user)

extern void fts_midifile_read_functions_init(fts_midifile_read_functions_t *read);
extern void fts_midifile_write_functions_init(fts_midifile_write_functions_t *write);
extern void fts_midifile_init(fts_midifile_t *file, FILE *fp, fts_symbol_t name);

extern fts_midifile_t *fts_midifile_open_read(fts_symbol_t name);
extern fts_midifile_t *fts_midifile_open_write(fts_symbol_t name);
extern void fts_midifile_close(fts_midifile_t *file);

extern int fts_midifile_read(fts_midifile_t *file);
extern int fts_midifile_write(fts_midifile_t *file, int format, int n_tracks, int division);

extern int fts_midifile_write_midi_event(fts_midifile_t *file, 
					 unsigned long delta_time, unsigned int type, 
					 unsigned int chan, unsigned char *data, unsigned long size);

extern int fts_midifile_write_meta_event(fts_midifile_t *file, 
					 unsigned long delta_time, unsigned int type, 
					 unsigned char *data, unsigned long size);
