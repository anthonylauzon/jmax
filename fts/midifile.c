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

#include <stdlib.h>
#include <stdio.h>
#include <fts/fts.h>
#include <ftsconfig.h>
#if HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

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
mferror(fts_midifile_t *midifile, char *s)
{
  midifile->error = s;
}

/*************************************************************
*
*  read utilities
*
*/

/* read a single character and abort on EOF */
static int
readbyte(fts_midifile_t *midifile)
{
  int c = getc(midifile->fp);
  
  if (c == EOF)
  {
    mferror(midifile, "unexspected end of file");
    return EOF;
  }
  else
  {
    midifile->bytes--; 
    return c;
  }
}

/* read a varying-length number, and return the number of characters it took */
static int
readvarinum(fts_midifile_t *midifile)
{
  int c = readbyte(midifile);
  int value;
  
  if(c == EOF)
    return 0;
  
  value = c & 0x7f;
  
  while(c & 0x80)
  {
    c = readbyte(midifile);
    
    if(c == EOF)
      return 0;
    
    value = (value << 7) + (c & 0x7f);
  }
  
  return value;
}

static int
read32bit(fts_midifile_t *midifile)
{
  int c1 = readbyte(midifile);
  int c2 = readbyte(midifile);
  int c3 = readbyte(midifile);
  int c4 = readbyte(midifile);
  
  return (c1 << 24) + (c2 << 16) + (c3 << 8) + c4;
}

static int
read16bit(fts_midifile_t *midifile)
{
  int msb = readbyte(midifile);
  int lsb = readbyte(midifile);
  
  return (msb << 8) + lsb;
}

static fts_heap_t *tempo_map_entry_heap = NULL;

/* format 1 tempo map handling */
static void
midifile_tempo_map_add_entry(fts_midifile_t *midifile, int tick, int tempo)
{
  fts_midifile_tempo_map_entry_t *entry;
  
  if(tempo_map_entry_heap == NULL)
    tempo_map_entry_heap = fts_heap_new(sizeof(fts_midifile_tempo_map_entry_t));
      
  /* create new tempo map entry */
  entry = (fts_midifile_tempo_map_entry_t *)fts_heap_alloc(tempo_map_entry_heap);
  
  if(midifile->tempo_map == NULL)
  {    
    if(tick == 0)
    {
      entry->tick = 0;
      entry->time = 0.0;
      entry->conv = 0.001 * tempo / (double)midifile->division;
      entry->next = NULL;
    }
    else
    {
      midifile_tempo_map_add_entry(midifile, 0, FTS_MIDIFILE_DEFAULT_TEMPO);
      
      entry->tick = tick;
      entry->time = tick * midifile->tempo_map->conv;
      entry->conv = 0.001 * tempo / (double)midifile->division;
      entry->next = NULL;
    }
    
    midifile->tempo_map = entry;
    midifile->tempo_map_end = entry;
  }
  else
  {    
    entry->tick = tick;
    entry->time = midifile->tempo_map_end->time + (tick - midifile->tempo_map_end->tick) * midifile->tempo_map_end->conv;
    entry->conv = 0.001 * tempo / (double)midifile->division;
    entry->next = 0;
    
    midifile->tempo_map_end->next = entry;
    midifile->tempo_map_end = entry;
  }
}

static void
midifile_tempo_map_destroy(fts_midifile_t *midifile)
{
  fts_midifile_tempo_map_entry_t *e = midifile->tempo_map;
  
  while(e != NULL)
  {
    fts_midifile_tempo_map_entry_t *next = e->next;
    
    fts_heap_free(e, tempo_map_entry_heap);
    
    e = next;
  }
  
  midifile->tempo_map = NULL;
  midifile->tempo_map_end = NULL;
}

/* ordinary channel message */
static void
midifile_channel_message_call(fts_midifile_t *midifile, enum midi_type type, int channel, int byte1, int byte2)
{
  if (midifile->read->midi_event)
  {
    fts_midievent_t *event = fts_midievent_channel_message_new(type, channel, byte1, byte2);
    
    fts_object_refer(event);
    midifile->read->midi_event(midifile, event); 
    fts_object_release(event);
  }
}

/* system exclusive message buffer */
static void
midifile_system_exclusive_start(fts_midifile_t *midifile)
{
  fts_midievent_t *sysex_event = midifile->system_exclusive;
  
  if(sysex_event)
    fts_object_release(sysex_event);
  
  sysex_event = fts_midievent_system_exclusive_new(0, 0);
  fts_object_refer(sysex_event);
  
  midifile->system_exclusive = sysex_event;
}

static void
midifile_system_exclusive_byte(fts_midifile_t *midifile, int byte)
{
  fts_midievent_system_exclusive_append(midifile->system_exclusive, byte);
}

static void
midifile_system_exclusive_call(fts_midifile_t *midifile)
{
  fts_midievent_t *sysex_event = midifile->system_exclusive;
  
  if(sysex_event)
  {
    if(midifile->read->midi_event && fts_midievent_system_exclusive_get_size(sysex_event) > 0)
      midifile->read->midi_event(midifile, sysex_event);
    
    fts_object_release((fts_object_t *)sysex_event);
    midifile->system_exclusive = 0;
  }
}

/* divers meta data buffer */
static void
midifile_string_clear(fts_midifile_t *midifile)
{
  midifile->string_size = 0;
}

static void
midifile_string_add_char(fts_midifile_t *midifile, int c)
{
  /* If necessary, allocate larger message buffer */
  if (midifile->string_size >= midifile->string_alloc)
  {
    midifile->string_alloc += 128;
    midifile->string = (char *)fts_realloc(midifile->string, sizeof(char) * midifile->string_alloc);
  }
  
  midifile->string[midifile->string_size++] = c;
}

/* read through the "MThd" or "MTrk" header string */
static int 
midifile_read_mt(fts_midifile_t *midifile, char *s)
{
  FILE *fp = midifile->fp;
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
midifile_read_header(fts_midifile_t *midifile)
{
  char c;
  
  if (midifile_read_mt(midifile, "MThd") == 0)
  {
    mferror(midifile, "exspected beginning of header (didn't find 'MThd' tag)");
    return 0;
  }
  
  midifile->bytes = read32bit(midifile);
  
  midifile->format = read16bit(midifile);
  midifile->n_tracks = read16bit(midifile);
  midifile->division = read16bit(midifile);
  
  if(midifile->division > 0)
    midifile->time_conv = 0.001 * FTS_MIDIFILE_DEFAULT_TEMPO / (double)midifile->division;
  else
  {
    double smpte_format = (midifile->division & 0xff00) >> 8;
    double smpte_resolution = midifile->division & 0xff;
    
    midifile->time_conv = 1000.0 / (smpte_format * smpte_resolution);
  }
  
  if(!midifile->error && midifile->read->header)
    (*midifile->read->header)(midifile);
  
  /* flush any extra stuff */
  c = 0;
  while (c != EOF && midifile->bytes > 0)
    c = readbyte(midifile);
  
  if(c == EOF)
    return 0;
  else
    return midifile->n_tracks;
}

/* read a track chunk */
static void
midifile_read_track(fts_midifile_t *midifile)
{
  int sysex_continue = 0;
  int status = 0;
  int channel = 0;
  
  if (midifile_read_mt(midifile, "MTrk") == 0)
  {
    mferror(midifile, "exspected beginning of track (didn't find 'MTrk' tag)");
    return;
  }
  
  midifile->bytes = read32bit(midifile);
  
  /* time starts at zero */
  midifile->ticks = 0;
  midifile->time = 0.0;
  midifile->tempo_map_pointer = midifile->tempo_map;
  
  if (!midifile->error && midifile->read->track_start)
    (*midifile->read->track_start)(midifile);
  
  while(!midifile->error && midifile->bytes > 0)
  {
    int delta = readvarinum(midifile);
    int byte = readbyte(midifile);
    int data1 = 0;
    int data2 = 0;
    
    if(byte == EOF)
      break;
    
    midifile->ticks += delta; /* delta time in ticks */
    
    if(midifile->tempo_map_pointer != NULL)
    {
      fts_midifile_tempo_map_entry_t *p = midifile->tempo_map_pointer;
      
      /* adjust tempo map pointer */
      while(p->next && p->next->tick <= midifile->ticks)
        p = p->next;
      
      midifile->time = p->time + (midifile->ticks - p->tick) * p->conv;
      
      midifile->tempo_map_pointer = p;
    }
    else
      midifile->time += delta * midifile->time_conv; /* delta time in msec */
    
    if(sysex_continue && byte != SYSTEM_EXCLUSIVE_CONTINUE)
    {
      mferror(midifile, "didn't find expected continuation of a sysex");
      return;
    }
    
    if(byte < 128)
    {
      /* running status */
      if(status < 128)
	    {
	      mferror(midifile, "unexpected running status");
	      return;
	    }
      
      data1 = byte;
    }
    else if(byte < SYSTEM_EXCLUSIVE)
    {
      /* channel message status byte */
      status = byte & 0xf0;
      channel = (byte & 0x0f) + 1;
      
      /* read first data byte */
      data1 = readbyte(midifile);
    }
    else
      /* sysex or meta event */
      status = byte & 0xff;
    
    if(midifile->error != 0)
      break;
    
    switch (status) 
    {
      case NOTE_OFF:
        data2 = readbyte(midifile); /* read but ignore */
        midifile_channel_message_call(midifile, midi_note, channel, data1, 0);
        break;
        
      case NOTE_ON:
        data2 = readbyte(midifile);
        midifile_channel_message_call(midifile, midi_note, channel, data1, data2);
        break;
        
      case POLY_PRESSURE:
        data2 = readbyte(midifile);
        midifile_channel_message_call(midifile, midi_poly_pressure, channel, data1, data2);
        break;
        
      case CONTROL_CHANGE:
        data2 = readbyte(midifile);
        midifile_channel_message_call(midifile, midi_control_change, channel, data1, data2);
        break;
        
      case PROGRAM_CHANGE:
        midifile_channel_message_call(midifile, midi_program_change, channel, data1, MIDI_EMPTY_BYTE);
        break;
        
      case CHANNEL_PRESSURE:
        midifile_channel_message_call(midifile, midi_channel_pressure, channel, data1, MIDI_EMPTY_BYTE);
        break;
        
      case PITCH_BEND:
        data2 = readbyte(midifile);
        midifile_channel_message_call(midifile, midi_pitch_bend, channel, data1, data2);
        break;
        
      case SYSTEM_EXCLUSIVE: /* start of system exclusive */
      {
        int lookfor = midifile->bytes - readvarinum(midifile);
        
        midifile_system_exclusive_start(midifile);
        
        while (midifile->bytes > lookfor)
	      {
          byte = readbyte(midifile);
          midifile_system_exclusive_byte(midifile, byte);
	      }
        
        if (byte == SYSTEM_EXCLUSIVE_END)
          midifile_system_exclusive_call(midifile);
        else
          sysex_continue = 1; /* merge into next message */
        
        break;
        
        case SYSTEM_EXCLUSIVE_CONTINUE: /* sysex continuation */
          
          lookfor = midifile->bytes - readvarinum(midifile);
          
          if (!sysex_continue)
            midifile_system_exclusive_start(midifile);
            
          while (midifile->bytes > lookfor)
            {
              byte = readbyte(midifile);
              midifile_system_exclusive_byte(midifile, byte);
            }
              
              if (!sysex_continue)
                midifile_system_exclusive_call(midifile); /* arbitrary message */
          else if(byte == SYSTEM_EXCLUSIVE_END)
          {
            midifile_system_exclusive_call(midifile);
            sysex_continue = 0;
          }
      }
        break;
          
        case META_EVENT: /* meta event */
        {
          int meta = readbyte(midifile);
          int lookfor = midifile->bytes - readvarinum(midifile);
          
          switch(meta) 
          {
            case SEQUENCE_NUMBER:
            {
              int msb = readbyte(midifile);
              int lsb = readbyte(midifile);
              
              if (midifile->read->sequence_number)
                (*midifile->read->sequence_number)(midifile, (msb << 8) + lsb);
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
              midifile_string_clear(midifile);
              
              while (midifile->bytes > lookfor)
                midifile_string_add_char(midifile, readbyte(midifile));
                
                if (midifile->read->text)
                  (*midifile->read->text)(midifile, meta, midifile->string_size, midifile->string);
                  
                  break;
              
            case END_OF_TRACK:
              if (midifile->read->end_of_track)
                (*midifile->read->end_of_track)(midifile);
              
              break;
              
            case SET_TEMPO:
            {
              int b0 = readbyte(midifile);
              int b1 = readbyte(midifile);
              int b2 = readbyte(midifile);
              int tempo = (b0 << 16) + (b1 << 8) + b2;
              
              if(midifile->format == 1)
                midifile_tempo_map_add_entry(midifile, midifile->ticks, tempo);

              if(midifile->division > 0)
                midifile->time_conv = 0.001 * (double)tempo / (double)midifile->division;
              
              midifile->tempo = tempo;
              
              if (midifile->read->tempo)
                (*midifile->read->tempo)(midifile, tempo);
            }
              break;
              
            case SMPTE_OFFSET:
            {
              int byte = readbyte(midifile);
              int type = (byte & 0x60) >> 5;
              int hour = (byte & 0x1f);
              int minute = readbyte(midifile);
              int second = readbyte(midifile);
              int frame = readbyte(midifile);
              int frac = readbyte(midifile);
              
              if (midifile->read->smpte)
                (*midifile->read->smpte)(midifile, type, hour, minute, second, frame, frac);
            }
              break;
              
            case TIME_SIGNATURE:
            {
              int numerator = readbyte(midifile);
              int denominator = 1 << readbyte(midifile);
              int clocks_per_metronome_click = readbyte(midifile);
              int heals_per_quarter_note = readbyte(midifile); /* heals of 32nd notes per quarter note */
              
              if (midifile->read->time_signature)
                (*midifile->read->time_signature)(midifile, numerator, denominator, clocks_per_metronome_click, heals_per_quarter_note);
            }
              break;
              
            case KEY_SIGNATURE:
            {
              int n_sharps_or_flats = readbyte(midifile);
              int major_or_minor = readbyte(midifile);
              
              if (midifile->read->key_signature)
                (*midifile->read->key_signature)(midifile, n_sharps_or_flats, major_or_minor);
              break;
            }
              
            case SEQUENCER_SPECIFIC:
            {
              /* sequencer specific data */
              midifile_string_clear(midifile);
              
              while (midifile->bytes > lookfor)
                midifile_string_add_char(midifile, readbyte(midifile));
              
              /* ignore data */
              
              break;
            }
              
            default:
            {
              /* unknown meta event */
              midifile_string_clear(midifile);
              
              while (midifile->bytes > lookfor)
                midifile_string_add_char(midifile, readbyte(midifile));
              
              /* ignore data */
              
              break;
            }
          }
        }
          break;
            
          default:
            mferror(midifile, "found unexpected byte");
            return;
            
            break;
    }
  }
  
  if (!midifile->error && midifile->read->track_end)
    (*midifile->read->track_end)(midifile);
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
writebyte(fts_midifile_t *midifile, unsigned char c)			
{
  FILE *fp = midifile->fp;
  int i;
	
  i = putc(c, fp);
  
  if (i == EOF)
  {
    mferror(midifile, "write error");
    return EOF;
  }
  else
  {
    midifile->size++;
    return i;
  }
}

/* write multi-length bytes to MIDI format files */
static void 
writevarlen(fts_midifile_t *midifile, unsigned int value)
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
    writebyte(midifile, (unsigned)(buffer & 0xff));
    
    if(buffer & 0x80)
      buffer >>= 8;
    else
      return;
  }
}

static void 
write32bit(fts_midifile_t *midifile, unsigned int data)
{
  writebyte(midifile, (unsigned)((data >> 24) & 0xff));
  writebyte(midifile, (unsigned)((data >> 16) & 0xff));
  writebyte(midifile, (unsigned)((data >> 8) & 0xff));
  writebyte(midifile, (unsigned)(data & 0xff));
}

static void 
write16bit(fts_midifile_t *midifile, int data)
{
  writebyte(midifile, (unsigned)((data & 0xff00) >> 8));
  writebyte(midifile, (unsigned)(data & 0xff));
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
fts_midifile_write_header(fts_midifile_t *midifile, int format, int n_tracks, int division) 
{
  unsigned int ident = MThd; /* head chunk identifier */
  unsigned int length = 6; /* chunk length */
  
  /* write header */
  write32bit(midifile, ident);
  write32bit(midifile, length);
  write16bit(midifile, format);
  write16bit(midifile, n_tracks);
  write16bit(midifile, division);
  
  midifile->division = division;
  
  return 1;
}

/* call for each track before writing the events */
int
fts_midifile_write_track_begin(fts_midifile_t *midifile)
{
  unsigned int trkhdr = MTrk;
	
  /* remember where the length was written, because we don't know how long it will be until we've finished writing */
  midifile->bytes = ftell(midifile->fp);
  
  /* write the track chunk header */
  write32bit(midifile, trkhdr);
  write32bit(midifile, 0); /* write length as 0 and correct later */
  
  midifile->ticks = 0; /* time starts from zero */
  midifile->time = 0.0;
  midifile->size = 0; /* the header's length doesn't count */
  
  return 1;
}

/* call for each track after having written the events */
int
fts_midifile_write_track_end(fts_midifile_t *midifile)
{
  unsigned int trkhdr = MTrk;
  int place_marker;
  int size;
  
  /* write end of track meta event */
  writebyte(midifile, 0);
  writebyte(midifile, META_EVENT);
  writebyte(midifile, END_OF_TRACK);
  
  writebyte(midifile, 0);
	 
  /* It's impossible to know how long the track chunk will be beforehand,
    so the position of the track length data is kept so that it can
    be written after the chunk has been generated */
  place_marker = ftell(midifile->fp);
	
  if(fseek(midifile->fp, midifile->bytes, 0) < 0)
  {
    mferror(midifile, "error seeking during final stage of write");
    return EOF;
  }
  
  size = midifile->size; /* get number of written bytes until HERE! */
  
  /* Re-write the track chunk header with right length */
  write32bit(midifile, trkhdr);
  write32bit(midifile, size);
  
  fseek(midifile->fp, place_marker, 0);
  
  return 1;
}

void 
fts_midifile_write_channel_message(fts_midifile_t *midifile, int ticks, enum midi_type type, int channel, int byte1, int byte2)
{
  int status = 144 + ((type & 0x0f) << 4) + ((channel - 1) & 0x0f);
  int delta = ticks - midifile->ticks;
  
  if(delta < 0)
    delta = 0;
  
  /* write delt time */
  writevarlen(midifile, delta);
  
  /* set current file ticks */
  midifile->ticks = ticks;
  
  /* write message */
  writebyte(midifile, status);
  writebyte(midifile, byte1 & 0x7f);
  
  if(byte2 != MIDI_EMPTY_BYTE)
    writebyte(midifile, byte2 & 0x7f);
}

void 
fts_midifile_write_midievent(fts_midifile_t *midifile, int ticks, fts_midievent_t *event)
{
  int delta = ticks - midifile->ticks;
  
  if(delta < 0)
    delta = 0;
  
  /* write delt time */
  writevarlen(midifile, delta);
  
  /* set current file ticks */
  midifile->ticks = ticks;
  
  if(fts_midievent_is_channel_message(event))
  {
    writebyte(midifile, fts_midievent_channel_message_get_status_byte(event));
    writebyte(midifile, (fts_midievent_channel_message_get_first(event) & 0x7f));
    
    if(fts_midievent_channel_message_has_second_byte(event))
      writebyte(midifile, (fts_midievent_channel_message_get_second(event) & 0x7f));
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
        
        writebyte(midifile, SYSTEM_EXCLUSIVE);
        
        for(i=0, n=1; i<size; i++)
          writebyte(midifile, fts_get_int(atoms + i) & 0x7f);
        
        writebyte(midifile, SYSTEM_EXCLUSIVE_END);
      }
        break;
        
      case midi_real_time:
        writebyte(midifile, fts_midievent_real_time_get_status_byte(event));
        break;
        
      default:
        break;
    }
  }
}

int
fts_midifile_write_meta_event(fts_midifile_t *midifile, int ticks, int type, unsigned char *data, int size)
{
  int i;
  
  writevarlen(midifile, ticks - midifile->ticks);
  midifile->ticks = ticks;
  
  /* this marks the fact we're writing a meta-event */
  writebyte(midifile, META_EVENT);
  
  /* the type of meta event */
  writebyte(midifile, type);
  
  /* the length of the data bytes to follow */
  writevarlen(midifile, size); 
  
  for(i=0; i<size; i++)
  {
    if(writebyte(midifile, data[i]) != data[i])
      return -1; 
  }
  
  return size;
}

/* write tempo in microseconds/quarter note */
void 
fts_midifile_write_tempo(fts_midifile_t *midifile, int tempo)
{
  writebyte(midifile, 0); /* at time 0 */
  writebyte(midifile, META_EVENT);
  writebyte(midifile, SET_TEMPO);
  
  writebyte(midifile, 3);
  writebyte(midifile, (unsigned)(0xff & (tempo >> 16)));
  writebyte(midifile, (unsigned)(0xff & (tempo >> 8)));
  writebyte(midifile, (unsigned)(0xff & tempo));
  
  midifile->tempo = tempo;
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
fts_midifile_init(fts_midifile_t *midifile, FILE *fp, fts_symbol_t name)
{
  midifile->fp = fp;
  midifile->name = name;
  
  midifile->format = 0;
  midifile->n_tracks = 0;
  midifile->division = 0;
  midifile->tempo = FTS_MIDIFILE_DEFAULT_TEMPO;
  
  midifile->tempo_map = NULL;
  midifile->tempo_map_end = NULL;
  midifile->tempo_map_pointer = NULL;
  
  midifile->read = 0;
  
  midifile->ticks = 0;
  midifile->bytes = 0;
  midifile->size = 0;
  
  midifile->system_exclusive = 0;
  
  midifile->string = 0;
  midifile->string_size = 0;
  midifile->string_alloc = 0;
  
  midifile->error = 0;
  
  midifile->user = 0;
}

fts_midifile_t *
fts_midifile_open_read(fts_symbol_t name)
{
  char full_path[MAXPATHLEN];
  FILE *fp;
  
  if (fts_file_find( name, full_path, MAXPATHLEN) == NULL)
    return 0;
  
  fp = fopen(full_path, "rb");
  
  if(fp != 0)
  {
    fts_midifile_t *midifile = fts_malloc(sizeof(fts_midifile_t));
    
    fts_midifile_init(midifile, fp, name);
    
    return midifile;
  }
  else
    return 0;
}

fts_midifile_t *
fts_midifile_open_write(fts_symbol_t name)
{
  char full_path[1024];
  FILE *fp;
  
  fts_make_absolute_path( NULL, name, full_path, MAXPATHLEN);
  
  fp = fopen(full_path, "wb");
  
  if(fp != 0)
  {
    fts_midifile_t *midifile = fts_malloc(sizeof(fts_midifile_t));
    
    fts_midifile_init(midifile, fp, name);
    
    return midifile;
  }
  else
    return 0;
}

void 
fts_midifile_close(fts_midifile_t *midifile)
{
  midifile_tempo_map_destroy(midifile);
  fclose(midifile->fp);
}

int
fts_midifile_read(fts_midifile_t *midifile)
{
  int size;
  int i;
  
  size = midifile_read_header(midifile);
  
  for(i=0; i<size; i++)
  {
    midifile_read_track(midifile);
    
    if(midifile->error != 0)
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
fts_midifile_time_to_ticks(fts_midifile_t *midifile, double time)
{
  return (int)((time * (double)midifile->division) / (0.001 * (double)midifile->tempo) + 0.5);
}
