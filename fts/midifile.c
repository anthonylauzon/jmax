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
 */
#include <stdlib.h>
#include <stdio.h>
#include <fts/fts.h>

/* MIDI status bytes */
#define NOTE_OFF 0x80
#define NOTE_ON 0x90
#define POLY_PRESSURE 0xa0
#define CONTROL_CHANGE 0xb0
#define PROGRAM_CHANGE 0xc0
#define CHANNEL_PRESSURE 0xd0
#define PITCH_BEND 0xe0
#define SYSTEM_EXCLUSIVE 0xf0
#define SYSTEM_EXCLUSIVE_CONTINUE 0xf7
#define SYSTEM_EXCLUSIVE_END 0xf7
#define	META_EVENT 0xFF

/* meta event definitions */
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
#define	SEQUENCER_SPECIFIC 0x7f

/* magic strings */
#define MThd 0x4d546864
#define MTrk 0x4d54726b

#define FTS_MIDIFILE_DEFAULT_TEMPO 500000

/*************************************************************
 *
 *  error handling
 *
 */
static void
mferror(fts_midifile_t *file, char *s)
{
  file->error = s;
}

/*************************************************************
 *
 *  read utilities
 *
 */

/* read a single character and abort on EOF */
static int
readbyte(fts_midifile_t *file)
{
  int c = getc(file->fp);

  if (c == EOF)
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

/* read a varying-length number, and return the number of characters it took */
static int
readvarinum(fts_midifile_t *file)
{
  int c = readbyte(file);
  int value;
  
  if(c == EOF)
    return 0;

  value = c & 0x7f;

  while(c & 0x80)
    {
      c = readbyte(file);

      if(c == EOF)
	return 0;

      value = (value << 7) + (c & 0x7f);
    }

  return value;
}

static int
read32bit(fts_midifile_t *file)
{
  int c1 = readbyte(file);
  int c2 = readbyte(file);
  int c3 = readbyte(file);
  int c4 = readbyte(file);

  return (c1 << 24) + (c2 << 16) + (c3 << 8) + c4;
}

static int
read16bit(fts_midifile_t *file)
{
  int msb = readbyte(file);
  int lsb = readbyte(file);

  return (msb << 8) + lsb;
}

static void
midifile_channel_message_call(fts_midifile_t *file, enum midi_type type, int channel, int byte1, int byte2)
{
  if (file->read->midi_event)
    {
      fts_midievent_t *event = fts_midievent_channel_message_new(type, channel, byte1, byte2);
      
      fts_object_refer(event);
      file->read->midi_event(file, event); 
      fts_object_release(event);
   }
}

static void
midifile_system_exclusive_start(fts_midifile_t *file)
{
  fts_midievent_t *sysex_event = file->system_exclusive;

  if(sysex_event)
    fts_object_release(sysex_event);

  sysex_event = fts_midievent_system_exclusive_new();
  fts_object_refer(sysex_event);
}

static void
midifile_system_exclusive_byte(fts_midifile_t *file, int byte)
{
  fts_midievent_system_exclusive_append(file->system_exclusive, byte);
}

static void
midifile_system_exclusive_call(fts_midifile_t *file)
{
  fts_midievent_t *sysex_event = file->system_exclusive;

  if(sysex_event)
    {
      if(file->read->midi_event && fts_midievent_system_exclusive_get_size(sysex_event) > 0)
	file->read->midi_event(file, sysex_event);
      
      fts_object_release((fts_object_t *)sysex_event);
      file->system_exclusive = 0;
    }
}

#define MSGINCREMENT 128

static void
midifile_string_clear(fts_midifile_t *file)
{
  file->string_size = 0;
}

static void
midifile_string_add_char(fts_midifile_t *file, int c)
{
  /* If necessary, allocate larger message buffer */
  if (file->string_size >= file->string_alloc)
    {
      file->string_alloc += MSGINCREMENT;
      file->string = (char *)fts_realloc(file->string, sizeof(char) * file->string_alloc);
    }

  file->string[file->string_size++] = c;
}

/* read through the "MThd" or "MTrk" header string */
static int 
midifile_read_mt(fts_midifile_t *file, char *s)
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

/* read a header chunk */
static int
midifile_read_header(fts_midifile_t *file)
{
  FILE *fp = file->fp;
  char c;

  if (midifile_read_mt(file, "MThd") == 0)
    {
      mferror(file, "exspected beginning of header (didn't find 'MThd' tag)");
      return 0;
    }

  file->bytes = read32bit(file);

  file->format = read16bit(file);
  file->n_tracks = read16bit(file);
  file->division = read16bit(file);

  if(file->division > 0)
    file->time_conv = 0.001 * FTS_MIDIFILE_DEFAULT_TEMPO / (double)file->division;
  else
    {
      double smpte_format = (file->division & 0xff00) >> 8;
      double smpte_resolution = file->division & 0xff;

      file->time_conv = 1000.0 / (smpte_format * smpte_resolution);
    }

  if(!file->error && file->read->header)
    (*file->read->header)(file);

  /* flush any extra stuff */
  c = 0;
  while (c != EOF && file->bytes > 0)
    c = readbyte(file);

  if(c == EOF)
    return 0;
  else
    return file->n_tracks;
}

/* read a track chunk */
static void
midifile_read_track(fts_midifile_t *file)
{
  FILE *fp = file->fp;
  int sysex_continue = 0;
  int status = 0;
  int channel = 0;

  if (midifile_read_mt(file, "MTrk") == 0)
    {
      mferror(file, "exspected beginning of track (didn't find 'MTrk' tag)");
      return;
    }

  file->bytes = read32bit(file);

  /* time starts at zero */
  file->ticks = 0;
  file->time = 0.0;

  if (!file->error && file->read->track_start)
    (*file->read->track_start)(file);

  while(!file->error && file->bytes > 0)
    {
      int data1 = 0;
      int data2 = 0;
      int ticks = readvarinum(file);
      int byte;

      file->ticks += ticks; /* delta time in ticks */
      file->time += ticks * file->time_conv; /* delta time in msec */

      byte = readbyte(file);
      
      if(byte == EOF)
	break;

      if (sysex_continue && byte != SYSTEM_EXCLUSIVE_CONTINUE)
	{
	  mferror(file, "didn't find expected continuation of a sysex");
	  return;
	}
      
      if(byte < 128)
	{
	  /* running status */
	  if (status < 128)
	    {
	      mferror(file, "unexpected running status");
	      return;
	    }

	  data1 = byte;
	}
      else if(byte < SYSTEM_EXCLUSIVE)
	{
	  /* channel message status byte */
	  status = byte & 0xf0;
	  channel = byte & 0x0f;
	  
	  /* read first data byte */
	  data1 = readbyte(file);
	}
      else
	/* sysex or meta event */
	status = byte & 0xff;

      if(file->error != 0)
	break;
      
      switch (status) 
	{
	case NOTE_OFF:
	  data2 = readbyte(file); /* read but ignore */
	  midifile_channel_message_call(file, midi_note, channel, data1, 0);
	  break;

	case NOTE_ON:
	  data2 = readbyte(file);
	  midifile_channel_message_call(file, midi_note, channel, data1, data2);
	  break;
	  
	case POLY_PRESSURE:
	  data2 = readbyte(file);
	  midifile_channel_message_call(file, midi_poly_pressure, channel, data1, data2);
	  break;
	  
	case CONTROL_CHANGE:
	  data2 = readbyte(file);
	  midifile_channel_message_call(file, midi_control_change, channel, data1, data2);
	  break;
	  
	case PROGRAM_CHANGE:
	  midifile_channel_message_call(file, midi_program_change, channel, data1, MIDI_EMPTY_BYTE);
	  break;
	  
	case CHANNEL_PRESSURE:
	  midifile_channel_message_call(file, midi_channel_pressure, channel, data1, MIDI_EMPTY_BYTE);
	  break;
	  
	case PITCH_BEND:
	  data2 = readbyte(file);
	  midifile_channel_message_call(file, midi_pitch_bend, channel, data1, data2);
	  break;

	case SYSTEM_EXCLUSIVE: /* start of system exclusive */
	  {
	    int lookfor = file->bytes - readvarinum(file);
	    
	    midifile_system_exclusive_start(file);
	    
	    while (file->bytes > lookfor)
	      {
		byte = readbyte(file);
		midifile_system_exclusive_byte(file, byte);
	      }
	    
	    if (byte == SYSTEM_EXCLUSIVE_END)
	      midifile_system_exclusive_call(file);
	    else
	      sysex_continue = 1; /* merge into next message */
	    
	    break;
	    
	  case SYSTEM_EXCLUSIVE_CONTINUE: /* sysex continuation */
	    
	    lookfor = file->bytes - readvarinum(file);
	    
	    if (!sysex_continue)
	      midifile_system_exclusive_start(file);
	    
	    while (file->bytes > lookfor)
	      {
		byte = readbyte(file);
		midifile_system_exclusive_byte(file, byte);
	      }
	    
	    if (!sysex_continue)
	      midifile_system_exclusive_call(file); /* arbitrary message */
	    else if(byte == SYSTEM_EXCLUSIVE_END)
	      {
		midifile_system_exclusive_call(file);
		sysex_continue = 0;
	      }
	  }
	  break;
	  
	case META_EVENT: /* meta event */
	  {
	    int meta = readbyte(file);
	    int lookfor = file->bytes - readvarinum(file);
	    
	    switch(meta) 
	      {
	      case SEQUENCE_NUMBER:
		{
		  int msb = readbyte(file);
		  int lsb = readbyte(file);
		  
		  if (file->read->sequence_number)
		    (*file->read->sequence_number)(file, (msb << 8) + lsb);
		}
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
		/* text events */
		midifile_string_clear(file);
	      
		while (file->bytes > lookfor)
		  midifile_string_add_char(file, readbyte(file));
	      
		if (file->read->text)
		  (*file->read->text)(file, meta, file->string_size, file->string);

		break;

	      case END_OF_TRACK:
		if (file->read->end_of_track)
		  (*file->read->end_of_track)(file);

		break;

	      case SET_TEMPO:
		{
		  int b0 = readbyte(file);
		  int b1 = readbyte(file);
		  int b2 = readbyte(file);
		
		  file->tempo = (b0 << 16) + (b1 << 8) + b2;

		  if(file->division > 0)
		    file->time_conv = 0.001 * (double)file->tempo / (double)file->division;
		
		  if (file->read->tempo)
		    (*file->read->tempo)(file);
		}
		break;

	      case SMPTE_OFFSET:
		{
		  int byte = readbyte(file);
		  int type = (byte & 0x60) >> 5;
		  int hour = (byte & 0x1f);
		  int minute = readbyte(file);
		  int second = readbyte(file);
		  int frame = readbyte(file);
		  int frac = readbyte(file);
		
		  if (file->read->smpte)
		    (*file->read->smpte)(file, type, hour, minute, second, frame, frac);
		}
		break;

	      case TIME_SIGNATURE:
		{
		  int numerator = readbyte(file);
		  int denominator = readbyte(file);
		  int clocks_per_metronome_click = readbyte(file);
		  int heals_per_quarter_note = readbyte(file); /* heals of 32nd notes per quarter note */
		  
		  if (file->read->time_signature)
		    (*file->read->time_signature)(file, numerator, denominator, clocks_per_metronome_click, heals_per_quarter_note);
		}
		break;

	      case KEY_SIGNATURE:
		{
		  int n_sharps_or_flats = readbyte(file);
		  int major_or_minor = readbyte(file);
		  
		  if (file->read->key_signature)
		    (*file->read->key_signature)(file, n_sharps_or_flats, major_or_minor);
		  break;
		}

	      case SEQUENCER_SPECIFIC:
		{
		  /* sequencer specific data */
		  midifile_string_clear(file);
		  
		  while (file->bytes > lookfor)
		    midifile_string_add_char(file, readbyte(file));
		  
		  /* ignore data */

		  break;
		}

	      default:
		{
		  /* unknown meta event */
		  midifile_string_clear(file);
		  
		  while (file->bytes > lookfor)
		    midifile_string_add_char(file, readbyte(file));
		  
		  /* ignore data */
		  
		  break;
		}
	      }
	  }
	  break;
	  
	default:
	  mferror(file, "found unexpected byte");
	  return;

	  break;
	}
    }
  
  if (!file->error && file->read->track_end)
    (*file->read->track_end)(file);
}

/*************************************************************
 *
 *  write utilities
 *
 */

/*
 * writevarlen()
 * writebyte()
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
writebyte(fts_midifile_t *file, unsigned char c)			
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

/* write multi-length bytes to MIDI format files */
static void 
writevarlen(fts_midifile_t *file, unsigned int value)
{
  unsigned int buffer;

  buffer = value & 0x7f;

  while((value >>= 7) > 0)
    {
      buffer <<= 8;
      buffer |= 0x80;
      buffer += (value & 0x7f);
    }

  while(1)
    {
      writebyte(file, (unsigned)(buffer & 0xff));
      
      if(buffer & 0x80)
	buffer >>= 8;
      else
	return;
    }
}

static void 
write32bit(fts_midifile_t *file, unsigned int data)
{
  writebyte(file, (unsigned)((data >> 24) & 0xff));
  writebyte(file, (unsigned)((data >> 16) & 0xff));
  writebyte(file, (unsigned)((data >> 8) & 0xff));
  writebyte(file, (unsigned)(data & 0xff));
}

static void 
write16bit(fts_midifile_t *file, int data)
{
  writebyte(file, (unsigned)((data & 0xff00) >> 8));
  writebyte(file, (unsigned)(data & 0xff));
}

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
  unsigned int ident = MThd; /* head chunk identifier */
  unsigned int length = 6; /* chunk length */
    
  /* write header */
  write32bit(file, ident);
  write32bit(file, length);
  write16bit(file, format);
  write16bit(file, n_tracks);
  write16bit(file, division);

  file->division = division;

  return 1;
}

/* call for each track before writing the events */
int
fts_midifile_write_track_begin(fts_midifile_t *file)
{
  unsigned int trkhdr = MTrk;
	
  /* remember where the length was written, because we don't know how long it will be until we've finished writing */
  file->bytes = ftell(file->fp);

  /* write the track chunk header */
  write32bit(file, trkhdr);
  write32bit(file, 0); /* write length as 0 and correct later */

  file->ticks = 0; /* time starts from zero */
  file->time = 0.0;
  file->size = 0; /* the header's length doesn't count */

  return 1;
}

/* call for each track after having written the events */
int
fts_midifile_write_track_end(fts_midifile_t *file)
{
  unsigned int trkhdr = MTrk;
  int place_marker;
  int size;

  /* write end of track meta event */
  writebyte(file, 0);
  writebyte(file, META_EVENT);
  writebyte(file, END_OF_TRACK);

  writebyte(file, 0);
	 
  /* It's impossible to know how long the track chunk will be beforehand,
     so the position of the track length data is kept so that it can
     be written after the chunk has been generated */
  place_marker = ftell(file->fp);
	
  if(fseek(file->fp, file->bytes, 0) < 0)
    {
      mferror(file, "error seeking during final stage of write");
      return EOF;
    }

  size = file->size; /* get number of written bytes until HERE! */

  /* Re-write the track chunk header with right length */
  write32bit(file, trkhdr);
  write32bit(file, size);

  fseek(file->fp, place_marker, 0);

  return 1;
}

void 
fts_midifile_write_channel_message(fts_midifile_t *file, int ticks, enum midi_type type, int channel, int byte1, int byte2)
{
  int status = 144 + ((type & 0x0f) << 4) + (channel & 0x0f);
  int delta = ticks - file->ticks;

  if(delta < 0)
    delta = 0;

  /* write delt time */
  writevarlen(file, delta);

  /* set current file ticks */
  file->ticks = ticks;

  /* write message */
  writebyte(file, status);
  writebyte(file, byte1 & 0x7f);

  if(byte2 != MIDI_EMPTY_BYTE)
    writebyte(file, byte2 & 0x7f);
}

void 
fts_midifile_write_midievent(fts_midifile_t *file, int ticks, fts_midievent_t *event)
{
  int delta = ticks - file->ticks;

  if(delta < 0)
    delta = 0;

  /* write delt time */
  writevarlen(file, delta);

  /* set current file ticks */
  file->ticks = ticks;
  
  if(fts_midievent_is_channel_message(event))
    {
      writebyte(file, fts_midievent_channel_message_get_status_byte(event));
      writebyte(file, (fts_midievent_channel_message_get_first(event) & 0x7f));

      if(fts_midievent_channel_message_has_second_byte(event))
	writebyte(file, (fts_midievent_channel_message_get_second(event) & 0x7f));
    }
  else 
    {
      switch(fts_midievent_get_type(event))
	{
	case midi_system_exclusive:
	  {
	    int size = fts_midievent_system_exclusive_get_size(event);
	    fts_atom_t *atoms = fts_midievent_system_exclusive_get_atoms(event);
	    int i, n;
	    
	    writebyte(file, SYSTEM_EXCLUSIVE);
	    
	    for(i=0, n=1; i<size; i++)
	      writebyte(file, fts_get_int(atoms + i) & 0x7f);
	    
	    writebyte(file, SYSTEM_EXCLUSIVE_END);
	  }
	  break;
	  
	case midi_real_time:
	  writebyte(file, fts_midievent_real_time_get_status_byte(event));
	  break;
	  
	default:
	  break;
	}
    }
}

int
fts_midifile_write_meta_event(fts_midifile_t *file, int ticks, int type, unsigned char *data, int size)
{
  int i;

  writevarlen(file, ticks - file->ticks);
  file->ticks = ticks;
    
  /* this marks the fact we're writing a meta-event */
  writebyte(file, META_EVENT);

  /* the type of meta event */
  writebyte(file, type);

  /* the length of the data bytes to follow */
  writevarlen(file, size); 

  for(i=0; i<size; i++)
    {
      if(writebyte(file, data[i]) != data[i])
	return -1; 
    }

  return size;
}

/* write tempo in microseconds/quarter note */
void 
fts_midifile_write_tempo(fts_midifile_t *file, int tempo)
{
  writebyte(file, 0); /* at time 0 */
  writebyte(file, META_EVENT);
  writebyte(file, SET_TEMPO);

  writebyte(file, 3);
  writebyte(file, (unsigned)(0xff & (tempo >> 16)));
  writebyte(file, (unsigned)(0xff & (tempo >> 8)));
  writebyte(file, (unsigned)(0xff & tempo));

  file->tempo = tempo;
}

/*************************************************************
 *
 *  API
 *
 */

void
fts_midifile_read_functions_init(fts_midifile_read_functions_t *read)
{
  read->header = 0;
  read->track_start = 0;
  read->track_end = 0;
  read->midi_event = 0;
  read->sequence_number = 0;
  read->end_of_track = 0;
  read->smpte = 0;
  read->tempo = 0;
  read->time_signature = 0;
  read->key_signature = 0;;
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
  file->tempo = FTS_MIDIFILE_DEFAULT_TEMPO;

  file->read = 0;

  file->ticks = 0;
  file->bytes = 0;
  file->size = 0;

  file->string = 0;
  file->string_size = 0;
  file->string_alloc = 0;

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

  fp = fopen(full_path, "rb");

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

  fp = fopen(full_path, "wb");

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
  int size;
  int i;

  size = midifile_read_header(file);
  
  for(i=0; i<size; i++)
    {
      midifile_read_track(file);
      
      if(file->error != 0)
	break;
    }

return i;
}

/*************************************************************
 *
 *  timing
 *
 */

int
fts_midifile_time_to_ticks(fts_midifile_t *file, double time)
{
  return (int)((time * (double)file->division) / (0.001 * (double)file->tempo) + 0.5);
}
