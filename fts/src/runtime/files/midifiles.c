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

static void mferror(fts_midifile_t *file, char *s);
static int to16bit(int c1, int c2);
static long to32bit(int c1, int c2, int c3, int c4);

static int egetc(fts_midifile_t *file);
static int readheader(fts_midifile_t *file);
static int readtrack(fts_midifile_t *file);
static void readmetaevent(fts_midifile_t *file, int type);
static void readsysex(fts_midifile_t *file);
static void readchanmessage(fts_midifile_t *file, int status, int c1, int c2);
static long readvarinum(fts_midifile_t *file);
static int read16bit(fts_midifile_t *file);
static long read32bit(fts_midifile_t *file);

static int eputc(fts_midifile_t *file, unsigned char c);
static void writevarlen(fts_midifile_t *file, unsigned long value);
static void write16bit(fts_midifile_t *file, int data);
static void write32bit(fts_midifile_t *file, unsigned long data);

static void msginit(fts_midifile_t *file);
static char *msg(fts_midifile_t *file);
static int msgleng(fts_midifile_t *file);
static void msgadd(fts_midifile_t *file, int c);
static void biggermsg(fts_midifile_t *file);

/* 
 * This routine converts delta times in ticks into seconds. The
 * else statement is needed because the formula is different for tracks
 * based on notes and tracks based on SMPTE times.
 *
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

static unsigned long 
sec2ticks(float secs, int division, unsigned int tempo)
{    
  return (long)((secs * 1000.0 / 4.0 * division) / tempo);
}

double
fts_midifile_get_current_time_in_seconds(fts_midifile_t *file)
{
  return (ticks2sec(file->currtime, file->division, file->tempo));
}

void
fts_midifile_read_functions_init(fts_midifile_read_functions_t *read)
{
  read->header = 0;
  read->trackstart = 0;
  read->trackend = 0;
  read->noteon = 0;
  read->noteoff = 0;
  read->pressure = 0;
  read->controller = 0;
  read->pitchbend = 0;
  read->program = 0;
  read->chanpressure = 0;
  read->sysex = 0;
  read->arbitrary = 0;
  read->metamisc = 0;
  read->seqnum = 0;
  read->eot = 0;
  read->smpte = 0;
  read->tempo = 0;
  read->timesig = 0;
  read->keysig = 0;
  read->seqspecific = 0;
  read->text = 0;
}

void
fts_midifile_write_functions_init(fts_midifile_write_functions_t *write)
{
  write->track = 0;
  write->tempotrack = 0;
}

void
fts_midifile_init(fts_midifile_t *file, FILE *fp, fts_symbol_t name)
{
  file->fp = fp;
  file->name = name;

  file->format = 0;
  file->n_tracks = 0;
  file->division = 0;
  file->tempo = 50000;

  file->read = 0;
  file->write = 0;

  file->currtime = 0;

  file->toberead = 0;
  file->numbyteswritten = 0;

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


/* The only non-static function in this file. */
int
fts_midifile_read(fts_midifile_t *file)
{
  int n_tracks = 0;

  if(readheader(file) == 0)
    return 0;

  while ( readtrack(file) > 0)
    n_tracks++;

  return n_tracks;
}

/* read through the "MThd" or "MTrk" header string */
static int 
readmt(fts_midifile_t *file, char *s)
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
      file->toberead--; 
      return c;
    }
}

static int
readheader(fts_midifile_t *file)		/* read a header chunk */
{
  int format, n_tracks, division;
  FILE *fp = file->fp;
  char c;

  if ( readmt(file, "MThd") == 0 )
    {
      mferror(file, "exspected beginning of header (didn't find 'MThd' tag)");
      return 0;
    }

  file->toberead = read32bit(file);

  file->format = read16bit(file);
  file->n_tracks = read16bit(file);
  file->division = read16bit(file);

  if( file->read->header )
    (*file->read->header)(file);

  /* flush any extra stuff */
  c = 0;
  while ( c != EOF && file->toberead > 0 )
    c = egetc(file);

  if(c == EOF)
    return 0;
  else
    return 1;
}

/* read a track chunk */
static int
readtrack(fts_midifile_t *file)
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

  if (readmt(file, "MTrk") == 0)
    {
      mferror(file, "exspected beginning of track (didn't find 'MTrk' tag)");
      return 0;
    }

  file->toberead = read32bit(file);
  file->currtime = 0;

  if ( file->read->trackstart )
    (*file->read->trackstart)(file);

  while ( file->toberead > 0 ) 
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
	  
	  readchanmessage(file, status, c1, (needed > 1) ? egetc(file) : 0 );
	  
	  continue;;
	}
      
      switch ( c ) 
	{
	  
	case 0xff: /* meta event */
	  
	  type = egetc(file);
	  lookfor = file->toberead - readvarinum(file);
	  
	  msginit(file);
	  
	  while ( file->toberead > lookfor )
	    msgadd(file, egetc(file));
	  
	  readmetaevent(file, type);
	  
	  break;
	  
	case 0xf0: /* start of system exclusive */
	  
	  lookfor = file->toberead - readvarinum(file);
	  
	  msginit(file);
	  msgadd(file, 0xf0);
	  
	  while ( file->toberead > lookfor )
	    msgadd(file, c = egetc(file));
	  
	  if (c == 0xf7)
	    readsysex(file);
	  else
	    /* merge into next msg */
	    sysexcontinue = 1;
	  
	  break;
	  
	case 0xf7: /* sysex continuation or arbitrary stuff */
	  
	  lookfor = file->toberead - readvarinum(file);
	  
	  if ( ! sysexcontinue )
	    msginit(file);
	  
	  while ( file->toberead > lookfor )
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
	      readsysex(file);
	      sysexcontinue = 0;
	    }
	  
	  break;
	  
	default:
	  mferror(file, "found unexpected byte");
	  return 0;

	  break;
	}
    }
  
  if ( file->read->trackend )
    (*file->read->trackend)(file);
  
  return 1;
}

static void
readmetaevent(fts_midifile_t *file, int type)
{
  fts_midifile_read_functions_t *read = file->read;
  int leng = msgleng(file);
  char *m = msg(file);

  switch  ( type ) 
    {
    case 0x00:
      if ( read->seqnum )
	(*read->seqnum)(file, to16bit(m[0], m[1]));
      break;
    case 0x01:	/* Text event */
    case 0x02:	/* Copyright notice */
    case 0x03:	/* Sequence/Track name */
    case 0x04:	/* Instrument name */
    case 0x05:	/* Lyric */
    case 0x06:	/* Marker */
    case 0x07:	/* Cue point */
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
    case 0x2f:	/* End of Track */
      if ( read->eot )
	(*read->eot)(file);
      break;
    case 0x51:	/* Set tempo */
      file->tempo = to32bit(0, m[0], m[1], m[2]);
      if ( read->tempo )
	(*read->tempo)(file);
      break;
    case 0x54:
      if ( read->smpte )
	(*read->smpte)(file, m[0], m[1], m[2], m[3], m[4]);
      break;
    case 0x58:
      if ( read->timesig )
	(*read->timesig)(file, m[0], m[1], m[2], m[3]);
      break;
    case 0x59:
      if ( read->keysig )
	(*read->keysig)(file, m[0], m[1]);
      break;
    case 0x7f:
      if ( read->seqspecific )
	(*read->seqspecific)(file, type, leng, m);
      break;
    default:
      if ( read->metamisc )
	(*read->metamisc)(file, type, leng, m);
    }
}

static void
readsysex(fts_midifile_t *file)
{
  if ( file->read->sysex )
    (*file->read->sysex)(file, msgleng(file), msg(file));
}

static void
readchanmessage(fts_midifile_t *file, int status, int c1, int c2)
{
  fts_midifile_read_functions_t *read = file->read;
  int chan = status & 0xf;

  switch ( status & 0xf0 ) 
    {
    case 0x80:
      if ( read->noteoff )
	(*read->noteoff)(file, chan, c1, c2);
      break;
    case 0x90:
      if ( read->noteon )
	(*read->noteon)(file, chan, c1, c2);
      break;
    case 0xa0:
      if ( read->pressure )
	(*read->pressure)(file, chan, c1, c2);
      break;
    case 0xb0:
      if ( read->controller )
	(*read->controller)(file, chan, c1, c2);
      break;
    case 0xe0:
      if ( read->pitchbend )
	(*read->pitchbend)(file, chan, c1, c2);
      break;
    case 0xc0:
      if ( read->program )
	(*read->program)(file, chan, c1);
      break;
    case 0xd0:
      if ( read->chanpressure )
	(*read->chanpressure)(file, chan, c1);
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
static char *Msgbuff = 0;	/* message buffer */
static int Msgsize = 0;		/* Size of currently allocated Msg */
static int Msgindex = 0;	/* index of next available location in Msg */

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

/*
 * fts_midifile_write() - The only fuction you'll need to call to write out
 *             a midi file.
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
static int write_track_chunk(fts_midifile_t *file, int which_track);
static void write_header_chunk(fts_midifile_t *file, int format, int ntracks, int division);

int
fts_midifile_write(fts_midifile_t *file, int format, int n_tracks, int division) 
{
  int i;

  if ( file->write->track == 0 )
    {
      mferror(file, "no write function defined");
      return 0;
    }

  /* every MIDI file starts with a header */
  write_header_chunk(file, format, n_tracks, division);

  /* In format 1 files, the first track is a tempo map */
  if(format == 1 && ( file->write->tempotrack ))
    (*file->write->tempotrack)(file);

  /* The rest of the file is a series of tracks */
  for(i = 0; i < n_tracks; i++)
    write_track_chunk(file, i);

  return 1;
}

static int
write_track_chunk(fts_midifile_t *file, int which_track)
{
  unsigned long trkhdr,trklength;
  long offset, place_marker;
	
  trkhdr = MTrk;
  trklength = 0;

  /* Remember where the length was written, because we don't
     know how long it will be until we've finished writing */
  offset = ftell(file->fp); 

  /* Write the track chunk header */
  write32bit(file, trkhdr);
  write32bit(file, trklength);

  file->numbyteswritten = 0L; /* the header's length doesn't count */

  if( file->write->track )
    (*file->write->track)(file, which_track);

  /* write End of track meta event */
  eputc(file, 0);
  eputc(file, meta_event);
  eputc(file, end_of_track);

  eputc(file, 0);
	 
  /* It's impossible to know how long the track chunk will be beforehand,
     so the position of the track length data is kept so that it can
     be written after the chunk has been generated */
  place_marker = ftell(file->fp);
	
  /* This method turned out not to be portable because the
     parameter returned from ftell is not guaranteed to be
     in bytes on every machine */
  /* track.length = place_marker - offset - (long) sizeof(track); */

  if(fseek(file->fp, offset, 0) < 0)
    {
      mferror(file, "error seeking during final stage of write");
      return EOF;
    }

  trklength = file->numbyteswritten;

  /* Re-write the track chunk header with right length */
  write32bit(file, trkhdr);
  write32bit(file, trklength);

  fseek(file->fp, place_marker, 0);

  return 1;
}

static void 
write_header_chunk(fts_midifile_t *file, int format, int ntracks, int division)
{
  unsigned long ident, length;
    
  ident = MThd; /* Head chunk identifier */
  length = 6; /* Chunk length */

  /* individual bytes of the header must be written separately
     to preserve byte order across cpu types :-( */
  write32bit(file, ident);
  write32bit(file, length);
  write16bit(file, format);
  write16bit(file, ntracks);
  write16bit(file, division);
}

/*
 * fts_midifile_write_midi_event()
 * 
 * Library routine to write a single MIDI track event in the standard MIDI
 * file format. The format is:
 *
 *                    <delta-time><event>
 *
 * In this case, event can be any multi-byte midi message, such as
 * "note on", "note off", etc.      
 *
 * delta_time - the time in ticks since the last event.
 * type - the type of meta event.
 * chan - The midi channel.
 * data - A pointer to a block of chars containing the META EVENT,
 *        data.
 * size - The length of the meta-event data.
 */
int 
fts_midifile_write_midi_event(fts_midifile_t *file, unsigned long delta_time, unsigned int type, unsigned int chan, unsigned char *data, unsigned long size)
{
  unsigned long i;
  unsigned char c;

  writevarlen(file, delta_time);

  /* all MIDI events start with the type in the first four bits,
     and the channel in the lower four bits */

  if(chan > 15)
    chan = 15;

  c = type | chan;

  eputc(file, c);

  /* write out the data bytes */
  for(i = 0; i < size; i++)
    eputc(file, data[i]);

  return size;
} /* end write MIDI event */

/*
 * fts_midifile_write_meta_event()
 *
 * Library routine to write a single meta event in the standard MIDI
 * file format. The format of a meta event is:
 *
 *          <delta-time><FF><type><length><bytes>
 *
 * delta_time - the time in ticks since the last event.
 * type - the type of meta event.
 * data - A pointer to a block of chars containing the META EVENT,
 *        data.
 * size - The length of the meta-event data.
 */
int
fts_midifile_write_meta_event(fts_midifile_t *file, unsigned long delta_time, unsigned int type, unsigned char *data, unsigned long size)
{
  unsigned long i;

  writevarlen(file, delta_time);
    
  /* This marks the fact we're writing a meta-event */
  eputc(file, meta_event);

  /* The type of meta event */
  eputc(file, type);

  /* The length of the data bytes to follow */
  writevarlen(file, size); 

  for(i=0; i<size; i++)
    {
      if(eputc(file, data[i]) != data[i])
	return -1; 
    }

  return size;
}

static void 
write_tempo(fts_midifile_t *file, unsigned long tempo)
{
  /* Write tempo */
  /* all tempos are written as 120 beats/minute, */
  /* expressed in microseconds/quarter note     */
  eputc(file, 0);
  eputc(file, meta_event);
  eputc(file, set_tempo);

  eputc(file, 3);
  eputc(file, (unsigned)(0xff & (tempo >> 16)));
  eputc(file, (unsigned)(0xff & (tempo >> 8)));
  eputc(file, (unsigned)(0xff & tempo));
}

/*
 * Write multi-length bytes to MIDI format files
 */
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

/*
 * 
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
      file->numbyteswritten++;
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
