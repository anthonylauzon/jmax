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
#include "sys.h"
#include "lang.h"
#include "files.h"
#include "midifiles.h"
#include <stdlib.h>
#include <stdio.h>

/* MIDI status bytes */
#define NOTE_OFF 0x80
#define NOTE_ON 0x90
#define POLY_PRESSURE 0xa0
#define CONTROL_CHANGE 0xb0
#define PROGRAM_CHANGE 0xc0
#define CHANNEL_PRESSURE 0xd0
#define PITCH_BEND 0xe0
#define SYSTEM_EXCLUSIVE 0xf0

/* meta event definitions */
#define	META_EVENT 0xFF
#define	SEQUENCE_NUMBER 0x00
#define	TEXT_EVENT 0x01
#define COPYRIGHT_NOTICE 0x02
#define SEQUENCE_NAME 0x03
#define INSTRUMENT_NAME 0x04
#define LYRIC 0x05
#define MARKER 0x06
#define	CUE_POINT 0x07
#define CHANNEL_PREFIX 0x20
#define	END_OF_TRACK 0x2f
#define	SET_TEMPO 0x51
#define	SMPTE_OFFSET 0x54
#define	TIME_SIGNATURE 0x58
#define	KEY_SIGNATURE 0x59
#define	SEQUENCER_SPECIFIC 0x74

/* miscellaneous definitions */
#define MThd 0x4d546864
#define MTrk 0x4d54726b
#define lowerbyte(x) ((unsigned char)(x & 0xff))
#define upperbyte(x) ((unsigned char)((x & 0xff00)>>8))

static void mferror(fts_midifile_t *file, char *s);

/*************************************************************
 *
 *  reading MIDI files
 *
 */

static int to16bit(int c1, int c2);
static long to32bit(int c1, int c2, int c3, int c4);

static int read_header(fts_midifile_t *file);
static int read_track(fts_midifile_t *file);
static void read_meta_event(fts_midifile_t *file, int type);
static void read_system_exclusive(fts_midifile_t *file);
static void read_channel_message(fts_midifile_t *file, int status, int c1, int c2);

static int egetc(fts_midifile_t *file);
static long readvarinum(fts_midifile_t *file);
static int read16bit(fts_midifile_t *file);
static long read32bit(fts_midifile_t *file);

static void msginit(fts_midifile_t *file);
static char *msg(fts_midifile_t *file);
static int msgleng(fts_midifile_t *file);
static void msgadd(fts_midifile_t *file, int c);
static void biggermsg(fts_midifile_t *file);

/* 
 * This routine converts delta times in ticks into seconds. The
 * else statement is needed because the formula is different for tracks
 * based on notes and tracks based on SMPTE times.
 */
static double
ticks2sec(unsigned long ticks, int division, unsigned int tempo)
{
  if(division > 0)
    {
      return (double)0.000001 * (double)ticks * (double)tempo / (double)division;
    }
  else
    {
      double smpte_format = upperbyte(division);
      double smpte_resolution = lowerbyte(division);
      return (double)0.000001 * (double)ticks / (smpte_format * smpte_resolution);
    }
}

double
fts_midifile_get_current_time_in_seconds(fts_midifile_t *file)
{
  return (ticks2sec(file->currtime, file->division, file->tempo));
}

long
fts_midifile_seconds_to_ticks(fts_midifile_t *file, double seconds)
{
  return (long)((seconds * 1000.0 / 4.0 * file->division) / file->tempo);
}

/*************************************************************
 *
 *  reading MIDI files
 *
 */

void
fts_midifile_read_functions_init(fts_midifile_read_functions_t *read)
{
  read->header = 0;
  read->track_start = 0;
  read->track_end = 0;
  read->note_on = 0;
  read->note_off = 0;
  read->poly_pressure = 0;
  read->control_change = 0;
  read->program_change = 0;
  read->channel_pressure = 0;
  read->pitch_bend = 0;
  read->system_exclusive = 0;
  read->arbitrary = 0;
  read->meta_misc = 0;
  read->sequence_number = 0;
  read->end_of_track = 0;
  read->smpte = 0;
  read->tempo = 0;
  read->time_signature = 0;
  read->key_signature = 0;;
  read->sequencer_specific = 0;
  read->text = 0;
}

static void
fts_midifile_init(fts_midifile_t *file, FILE *fp, fts_symbol_t name)
{
  file->fp = fp;
  file->name = name;

  file->format = 0;
  file->n_tracks = 0;
  file->division = 0;
  file->tempo = 50000;

  file->read = 0;

  file->currtime = 0;
  file->bytes = 0;
  file->size = 0;

  file->Msgbuff = 0;
  file->Msgsize = 0;
  file->Msgindex = 0;

  file->error = 0;

  file->user = 0;
}

fts_midifile_t *
fts_midifile_open_read(fts_symbol_t name)
{
  char full_path[1024];
  const char *path = fts_symbol_name(name);
  FILE *fp;

  if (!fts_file_get_read_path(path, full_path))
    return 0;

  fp = fopen(full_path, "r");

  if(fp != 0)
    {
      fts_midifile_t *file = fts_malloc(sizeof(fts_midifile_t));
      
      fts_midifile_init(file, fp, name);

      return file;
    }
  else
    return 0;
}

fts_midifile_t *
fts_midifile_open_write(fts_symbol_t name)
{
  char full_path[1024];
  const char *path = fts_symbol_name(name);
  FILE *fp;

  /* get full path of file location */
  fts_file_get_write_path(path, full_path);

  fp = fopen(full_path, "w");

  if(fp != 0)
    {
      fts_midifile_t *file = fts_malloc(sizeof(fts_midifile_t));
      
      fts_midifile_init(file, fp, name);

      return file;
    }
  else
    return 0;
}

void 
fts_midifile_close(fts_midifile_t *file)
{
  fclose(file->fp);
}


int
fts_midifile_read(fts_midifile_t *file)
{
  int n_tracks = 0;

  if(read_header(file) == 0)
    return 0;

  while ( read_track(file) > 0)
    n_tracks++;

  return n_tracks;
}


/*************************************************************
 *
 *  read utilities
 *
 */

/* read through the "MThd" or "MTrk" header string */
static int 
read_mt(fts_midifile_t *file, char *s)
{
  FILE *fp = file->fp;
  int i;

  for(i=0; i<4; i++)
    {
      int c = getc(fp);

      if (c == EOF || c != s[i])
	return 0;
    }
  
  return 1;
}

/* read a single character and abort on EOF */
static int
egetc(fts_midifile_t *file)
{
  int c = getc(file->fp);

  if ( c == EOF )
    {
      mferror(file, "unexspected end of file");
      return EOF;
    }
  else
    {
      file->bytes--; 
      return c;
    }
}

static int
read_header(fts_midifile_t *file)		/* read a header chunk */
{
  int format, n_tracks, division;
  FILE *fp = file->fp;
  char c;

  if ( read_mt(file, "MThd") == 0 )
    {
      mferror(file, "exspected beginning of header (didn't find 'MThd' tag)");
      return 0;
    }

  file->bytes = read32bit(file);

  file->format = read16bit(file);
  file->n_tracks = read16bit(file);
  file->division = read16bit(file);

  if( file->read->header )
    (*file->read->header)(file);

  /* flush any extra stuff */
  c = 0;
  while ( c != EOF && file->bytes > 0 )
    c = egetc(file);

  if(c == EOF)
    return 0;
  else
    return 1;
}

/* read a track chunk */
static int
read_track(fts_midifile_t *file)
{
  /* This array is indexed by the high half of a status byte.  It's */
  /* value is either the number of bytes needed (1 or 2) for a channel */
  /* message, or 0 (meaning it's not  a channel message). */
  static int chantype[] = {
    0, 0, 0, 0, 0, 0, 0, 0, /* 0x00 through 0x70 */
    2, 2, 2, 2, 1, 1, 2, 0 /* 0x80 through 0xf0 */
  };
  long lookfor;
  int c, c1, type;
  int sysexcontinue = 0;	/* 1 if last message was an unfinished sysex */
  int running = 0;	/* 1 when running status used */
  int status = 0;		/* status value (e.g. 0x90==note-on) */
  int needed;
  FILE *fp = file->fp;

  if (read_mt(file, "MTrk") == 0)
    {
      mferror(file, "exspected beginning of track (didn't find 'MTrk' tag)");
      return 0;
    }

  file->bytes = read32bit(file);
  file->currtime = 0;

  if ( file->read->track_start )
    (*file->read->track_start)(file);

  while ( file->bytes > 0 ) 
    {      
      file->currtime += readvarinum(file);	/* delta time */
      
      c = egetc(file);
      
      if ( sysexcontinue && c != 0xf7 )
	{
	  mferror(file, "didn't find expected continuation of a sysex");
	  return 0;
	}
      
      if ( (c & 0x80) == 0 ) 
	{	 
	  /* running status? */
	  if ( status == 0 )
	    {
	      mferror(file, "unexpected running status");
	      return 0;
	    }
	  
	  running = 1;
	}
      else 
	{
	  status = c;
	  running = 0;
	}
      
      needed = chantype[ (status>>4) & 0xf ];
      
      if ( needed ) 
	{		
	  /* ie. is it a channel message? */
	  if ( running )
	    c1 = c;
	  else
	    c1 = egetc(file);
	  
	  read_channel_message(file, status, c1, (needed > 1) ? egetc(file) : 0 );
	  
	  continue;;
	}
      
      switch ( c ) 
	{
	  
	case META_EVENT: /* meta event */
	  
	  type = egetc(file);
	  lookfor = file->bytes - readvarinum(file);
	  
	  msginit(file);
	  
	  while ( file->bytes > lookfor )
	    msgadd(file, egetc(file));
	  
	  read_meta_event(file, type);
	  
	  break;
	  
	case SYSTEM_EXCLUSIVE: /* start of system exclusive */
	  
	  lookfor = file->bytes - readvarinum(file);
	  
	  msginit(file);
	  msgadd(file, SYSTEM_EXCLUSIVE);
	  
	  while ( file->bytes > lookfor )
	    msgadd(file, c = egetc(file));
	  
	  if (c == 0xf7)
	    read_system_exclusive(file);
	  else
	    /* merge into next msg */
	    sysexcontinue = 1;
	  
	  break;
	  
	case 0xf7: /* sysex continuation or arbitrary stuff */
	  
	  lookfor = file->bytes - readvarinum(file);
	  
	  if ( ! sysexcontinue )
	    msginit(file);
	  
	  while ( file->bytes > lookfor )
	    {
	      c = egetc(file);
	      msgadd(file, c);
	    }
	  
	  if ( ! sysexcontinue ) 
	    {
	      if ( file->read->arbitrary )
		(*file->read->arbitrary)(file, msgleng(file), msg(file));
	    }
	  else if ( c == 0xf7 ) 
	    {
	      read_system_exclusive(file);
	      sysexcontinue = 0;
	    }
	  
	  break;
	  
	default:
	  mferror(file, "found unexpected byte");
	  return 0;

	  break;
	}
    }
  
  if ( file->read->track_end )
    (*file->read->track_end)(file);
  
  return 1;
}

static void
read_meta_event(fts_midifile_t *file, int type)
{
  fts_midifile_read_functions_t *read = file->read;
  int leng = msgleng(file);
  char *m = msg(file);

  switch  ( type ) 
    {
    case SEQUENCE_NUMBER:
      if ( read->sequence_number )
	(*read->sequence_number)(file, to16bit(m[0], m[1]));
      break;
    case TEXT_EVENT:
    case COPYRIGHT_NOTICE:
    case SEQUENCE_NAME:
    case INSTRUMENT_NAME:
    case LYRIC:
    case MARKER:
    case CUE_POINT:
    case 0x08:
    case 0x09:
    case 0x0a:
    case 0x0b:
    case 0x0c:
    case 0x0d:
    case 0x0e:
    case 0x0f:
      /* These are all text events */
      if ( read->text )
	(*read->text)(file, type, leng, m);
      break;
    case END_OF_TRACK:
      if ( read->end_of_track )
	(*read->end_of_track)(file);
      break;
    case SET_TEMPO:	/* Set tempo */
      file->tempo = to32bit(0, m[0], m[1], m[2]);
      if ( read->tempo )
	(*read->tempo)(file);
      break;
    case SMPTE_OFFSET:
      if ( read->smpte )
	(*read->smpte)(file, m[0], m[1], m[2], m[3], m[4]);
      break;
    case TIME_SIGNATURE:
      if ( read->time_signature )
	(*read->time_signature)(file, m[0], m[1], m[2], m[3]);
      break;
    case KEY_SIGNATURE:
      if ( read->key_signature )
	(*read->key_signature)(file, m[0], m[1]);
      break;
    case SEQUENCER_SPECIFIC:
      if ( read->sequencer_specific )
	(*read->sequencer_specific)(file, type, leng, m);
      break;
    default:
      if ( read->meta_misc )
	(*read->meta_misc)(file, type, leng, m);
    }
}

static void
read_system_exclusive(fts_midifile_t *file)
{
  if ( file->read->system_exclusive )
    (*file->read->system_exclusive)(file, msgleng(file), msg(file));
}

static void
read_channel_message(fts_midifile_t *file, int status, int c1, int c2)
{
  fts_midifile_read_functions_t *read = file->read;
  int chan = status & 0xf;

  switch (status & 0xf0) 
    {
    case NOTE_OFF:
      if ( read->note_off )
	(*read->note_off)(file, chan, c1, c2);
      break;
    case NOTE_ON:
      if ( read->note_on )
	(*read->note_on)(file, chan, c1, c2);
      break;
    case POLY_PRESSURE:
      if ( read->poly_pressure )
	(*read->poly_pressure)(file, chan, c1, c2);
      break;
    case CONTROL_CHANGE:
      if ( read->control_change )
	(*read->control_change)(file, chan, c1, c2);
      break;
    case PROGRAM_CHANGE:
      if ( read->program_change )
	(*read->program_change)(file, chan, c1);
      break;
    case CHANNEL_PRESSURE:
      if ( read->channel_pressure )
	(*read->channel_pressure)(file, chan, c1);
      break;
    case PITCH_BEND:
      if ( read->pitch_bend )
	(*read->pitch_bend)(file, chan, c1, c2);
      break;
    }
}

/* readvarinum - read a varying-length number, and return the */
/* number of characters it took. */

static long
readvarinum(fts_midifile_t *file)
{
  long value;
  int c;

  c = egetc(file);
  value = c;

  if ( c & 0x80 ) 
    {
      value &= 0x7f;

      do {
	  c = egetc(file);
	  value = (value << 7) + (c & 0x7f);
	} 
      while (c & 0x80);
    }

  return (value);
}

static long
to32bit(int c1, int c2, int c3, int c4)
{
  long value = 0L;

  value = (c1 & 0xff);
  value = (value<<8) + (c2 & 0xff);
  value = (value<<8) + (c3 & 0xff);
  value = (value<<8) + (c4 & 0xff);

  return (value);
}

static int
to16bit(int c1, int c2)
{
  return ((c1 & 0xff ) << 8) + (c2 & 0xff);
}

static long
read32bit(fts_midifile_t *file)
{
  int c1, c2, c3, c4;

  c1 = egetc(file);
  c2 = egetc(file);
  c3 = egetc(file);
  c4 = egetc(file);

  return to32bit(c1, c2, c3, c4);
}

static int
read16bit(fts_midifile_t *file)
{
  int c1, c2;
  c1 = egetc(file);
  c2 = egetc(file);

  return to16bit(c1, c2);
}

/* static */
static void
mferror(fts_midifile_t *file, char *s)
{
  file->error = s;
}

/* The code below allows collection of a system exclusive message of */
/* arbitrary length.  The Msgbuff is expanded as necessary.  The only */
/* visible data/routines are msginit(), msgadd(), msg(), msgleng(). */

#define MSGINCREMENT 128
static char *Msgbuff = 0; /* message buffer */
static int Msgsize = 0; /* size of currently allocated message */
static int Msgindex = 0; /* index of next available location in message */

static void
msginit(fts_midifile_t *file)
{
  file->Msgindex = 0;
}

static char *
msg(fts_midifile_t *file)
{
  return(file->Msgbuff);
}

static int
msgleng(fts_midifile_t *file)
{
  return(file->Msgindex);
}

static void
msgadd(fts_midifile_t *file, int c)
{
  /* If necessary, allocate larger message buffer. */
  if ( file->Msgindex >= file->Msgsize )
    biggermsg(file);

  file->Msgbuff[file->Msgindex++] = c;
}

static void
biggermsg(fts_midifile_t *file)
{
  char *oldmess = file->Msgbuff;
  int oldleng = file->Msgsize;
  char *newmess;

  file->Msgsize += MSGINCREMENT;
  newmess = (char *)fts_malloc(sizeof(char) * file->Msgsize);

  if(newmess == 0)
    mferror(file, "malloc error!");
		
  /* copy old message into larger new one */
  if(oldmess != 0) 
    {
      int i;
      
      for(i=0; i<oldleng; i++)
	newmess[i] = oldmess[i];

      fts_free(oldmess);
    }

  file->Msgbuff = newmess;
}

/*************************************************************
 *
 *  writing MIDI files
 *
 */

static int eputc(fts_midifile_t *file, unsigned char c);
static void writevarlen(fts_midifile_t *file, unsigned long value);
static void write16bit(fts_midifile_t *file, int data);
static void write32bit(fts_midifile_t *file, unsigned long data);

/*
 * fts_midifile_write_header()
 *
 * write a standard MIDI file header
 *
 * format      0 - Single multi-channel track
 *             1 - Multiple simultaneous tracks
 *             2 - One or more sequentially independent
 *                 single track patterns                
 * ntracks     The number of tracks in the file.
 * division    This is kind of tricky, it can represent two
 *             things, depending on whether it is positive or negative
 *             (bit 15 set or not).  If  bit  15  of division  is zero,
 *             bits 14 through 0 represent the number of delta-time
 *             "ticks" which make up a quarter note.  If bit  15 of
 *             division  is  a one, delta-times in a file correspond to
 *             subdivisions of a second similiar to  SMPTE  and  MIDI
 *             time code.  In  this format bits 14 through 8 contain
 *             one of four values - 24, -25, -29, or -30,
 *             corresponding  to  the  four standard  SMPTE and MIDI
 *             time code frame per second formats, where  -29
 *             represents  30  drop  frame.   The  second  byte
 *             consisting  of  bits 7 through 0 corresponds the the
 *             resolution within a frame.  Refer the Standard MIDI
 *             Files 1.0 spec for more details.
 */ 
int
fts_midifile_write_header(fts_midifile_t *file, int format, int n_tracks, int division) 
{
  int i;
  unsigned long ident = MThd; /* head chunk identifier */
  unsigned long length = 6; /* chunk length */
    
  /* write header */
  write32bit(file, ident);
  write32bit(file, length);
  write16bit(file, format);
  write16bit(file, n_tracks);
  write16bit(file, division);

  return 1;
}

/* call for each track before writing the events */
int
fts_midifile_write_track_begin(fts_midifile_t *file)
{
  unsigned long trkhdr = MTrk;
	
  /* remember where the length was written, because we don't know how long it will be until we've finished writing */
  file->bytes = ftell(file->fp);

  /* write the track chunk header */
  write32bit(file, trkhdr);
  write32bit(file, 0); /* write length as 0 and correct later */

  file->currtime = 0; /* time starts from zero */
  file->size = 0; /* the header's length doesn't count */

  return 1;
}

/* call for each track after having written the events */
int
fts_midifile_write_track_end(fts_midifile_t *file)
{
  unsigned long trkhdr = MTrk;
  long place_marker;

  /* write end of track meta event */
  eputc(file, 0);
  eputc(file, META_EVENT);
  eputc(file, END_OF_TRACK);

  eputc(file, 0);
	 
  /* It's impossible to know how long the track chunk will be beforehand,
     so the position of the track length data is kept so that it can
     be written after the chunk has been generated */
  place_marker = ftell(file->fp);
	
  if(fseek(file->fp, file->bytes, 0) < 0)
    {
      mferror(file, "error seeking during final stage of write");
      return EOF;
    }

  /* Re-write the track chunk header with right length */
  write32bit(file, trkhdr);
  write32bit(file, file->size);

  fseek(file->fp, place_marker, 0);

  return 1;
}

/*
 * write channel messages to standard MIDI file
 *
 *   fts_midifile_write_note_off()
 *   fts_midifile_write_note_on()
 *   fts_midifile_write_poly_pressure()
 *   fts_midifile_write_control_change()
 *   fts_midifile_write_program_change()
 *   fts_midifile_write_channel_pressure()
 *   fts_midifile_write_pitch_bend()
 *
 */ 

#define clip_channel(ch) ((ch > 15)? 15: ((ch < 0)? 0: ch))

void 
fts_midifile_write_note_off(fts_midifile_t *file, long time, int channel, int number)
{
  writevarlen(file, time - file->currtime);
  file->currtime = time;

  eputc(file, NOTE_OFF | clip_channel(channel));  
  eputc(file, number & 127);  
}

void 
fts_midifile_write_note_on(fts_midifile_t *file, long time, int channel, int number, int velocity)
{
  writevarlen(file, time - file->currtime);
  file->currtime = time;

  eputc(file, NOTE_ON | clip_channel(channel));  
  eputc(file, number & 127);  
  eputc(file, velocity & 127);  
}

void 
fts_midifile_write_poly_pressure(fts_midifile_t *file, long time, int channel, int number, int value)
{
  writevarlen(file, time - file->currtime);
  file->currtime = time;

  eputc(file, POLY_PRESSURE | clip_channel(channel));  
  eputc(file, number & 127);  
  eputc(file, value & 127);  
}

void 
fts_midifile_write_control_change(fts_midifile_t *file, long time, int channel, int number, int value)
{
  writevarlen(file, time - file->currtime);
  file->currtime = time;

  eputc(file, CONTROL_CHANGE | clip_channel(channel));  
  eputc(file, number & 127);  
  eputc(file, value & 127);  
}

void 
fts_midifile_write_program_change(fts_midifile_t *file, long time, int channel, int number)
{
  writevarlen(file, time - file->currtime);
  file->currtime = time;

  eputc(file, PROGRAM_CHANGE | clip_channel(channel));  
  eputc(file, number & 127);  
}

void 
fts_midifile_write_channel_pressure(fts_midifile_t *file, long time, int channel, int value)
{
  writevarlen(file, time - file->currtime);
  file->currtime = time;

  eputc(file, CHANNEL_PRESSURE | clip_channel(channel));  
  eputc(file, value & 127);  
}

void
fts_midifile_write_pitch_bend(fts_midifile_t *file, long time, int channel, int value)
{
  writevarlen(file, time - file->currtime);
  file->currtime = time;

  eputc(file, PITCH_BEND | clip_channel(channel));  
  eputc(file, value & 127);
  eputc(file, (value >> 7) & 127);
}

/*
 * fts_midifile_write_meta_event()
 *
 * write a single meta event in the standard MIDI file format
 *
 * delta_time - the time in ticks since the last event.
 * type - the type of meta event.
 * data - A pointer to a block of chars containing the META EVENT,
 *        data.
 * size - The length of the meta-event data.
 */
int
fts_midifile_write_meta_event(fts_midifile_t *file, long time, int type, unsigned char *data, int size)
{
  int i;

  writevarlen(file, time - file->currtime);
  file->currtime = time;
    
  /* this marks the fact we're writing a meta-event */
  eputc(file, META_EVENT);

  /* the type of meta event */
  eputc(file, type);

  /* the length of the data bytes to follow */
  writevarlen(file, size); 

  for(i=0; i<size; i++)
    {
      if(eputc(file, data[i]) != data[i])
	return -1; 
    }

  return size;
}

/* write tempo in microseconds/quarter note */
void 
fts_midifile_write_tempo(fts_midifile_t *file, int tempo)
{
  eputc(file, 0);
  eputc(file, META_EVENT);
  eputc(file, SET_TEMPO);

  eputc(file, 3);
  eputc(file, (unsigned)(0xff & (tempo >> 16)));
  eputc(file, (unsigned)(0xff & (tempo >> 8)));
  eputc(file, (unsigned)(0xff & tempo));
}

/*************************************************************
 *
 *  write utilities
 *
 */

/*
 * writevarlen()
 * eputc()
 * write32bit()
 * write16bit()
 *
 * These routines are used to make sure that the byte order of
 * the various data types remains constant between machines. This
 * helps make sure that the code will be portable from one system
 * to the next.  It is slightly dangerous that it assumes that longs
 * have at least 32 bits and ints have at least 16 bits, but this
 * has been true at least on PCs, UNIX machines, and Macintosh's.
 *
 */

/* write multi-length bytes to MIDI format files */
static void 
writevarlen(fts_midifile_t *file, unsigned long value)
{
  unsigned long buffer;

  buffer = value & 0x7f;

  while((value >>= 7) > 0)
    {
      buffer <<= 8;
      buffer |= 0x80;
      buffer += (value & 0x7f);
    }

  while(1)
    {
      eputc(file, (unsigned)(buffer & 0xff));
      
      if(buffer & 0x80)
	buffer >>= 8;
      else
	return;
    }
}

/* write a single character and abort on error */
static int
eputc(fts_midifile_t *file, unsigned char c)			
{
  FILE *fp = file->fp;
  int i;
	
  i = putc(c, fp);

  if (i == EOF)
    {
      mferror(file, "write error");
      return EOF;
    }
  else
    {
      file->size++;
      return i;
    }
}

static void 
write32bit(fts_midifile_t *file, unsigned long data)
{
  eputc(file, (unsigned)((data >> 24) & 0xff));
  eputc(file, (unsigned)((data >> 16) & 0xff));
  eputc(file, (unsigned)((data >> 8 ) & 0xff));
  eputc(file, (unsigned)(data & 0xff));
}

static void 
write16bit(fts_midifile_t *file, int data)
{
  eputc(file, (unsigned)((data & 0xff00) >> 8));
  eputc(file, (unsigned)(data & 0xff));
}
