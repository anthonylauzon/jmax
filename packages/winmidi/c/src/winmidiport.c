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

#include <windows.h>
#include <fts/fts.h>
#include <mmreg.h>

/* 
The code to handle sysex in is still being debugged. The current
code seems to work alright. When the midiport is closed no
errors are returned and the driver sends the the MIM_CLOSE message
to the calback function. However, when FTS restarts, the MIDI in
port cannot be opened (Device busy). This happens even when the
application did not receive any sysex messages at all. It's still a
mystery to me (blame it on MicroSoft). [PH,20020311]

 The following macro defines whether sysex in will be compiled in or
 not.
*/

#define WINMIDI_SYSEXIN 0

/*
 The following macro defines whether critical sections are used or not.
*/
#define USE_CRITICAL_SECTIONS 1


#define BUFFER_SIZE 1024
#define SYSEX_HEADER_SIZE 1024
#define SYSEX_BUFFER_SIZE 32768

#define NOTEOFF 0x80
#define NOTEON 0x90
#define KEYPRESSURE 0xa0
#define CONTROLCHANGE 0xb0
#define PROGRAMCHANGE 0xc0
#define CHANNELPRESSURE 0xd0
#define PITCHBEND 0xe0
#define SYSEX 0xf0
#define SYSEX_END 0xf7

#define WINMIDI_READ 1
#define WINMIDI_WRITE 2
#define WINMIDI_SCHEDULED 4


static char winmidiport_error_buffer[256];

#define msg_pack(_s,_p1,_p2) ((DWORD) ((_s) | (((_p1) & 0x7f) << 8) | (((_p2) & 0x7f) << 16)))

#define msg_type(_m) ((_m) & 0xf0)
#define msg_type_enum(_m) ((((_m) & 0xf0) - 144) >> 4)
#define msg_chan(_m) ((_m) & 0x0f)
#define msg_p1(_m) (((_m) >> 8) & 0x7f)
#define msg_p2(_m) (((_m) >> 16) & 0x7f)


/*************************************************
*
*  Win midi port
*
*/

typedef struct _winmidiport_t
{
    fts_midiport_t port;
    HMIDIOUT hmidiout;
    HMIDIIN hmidiin;

#if USE_CRITICAL_SECTIONS
    CRITICAL_SECTION critical_section;
#endif
    
#if WINMIDI_SYSEXIN
    MIDIHDR inhdr[2];
#endif
    
    DWORD incoming[BUFFER_SIZE];
    int head;
    int tail;
    unsigned int flags;
    
    int midiin_state;
    
    MIDIHDR sysex_hdr;
    unsigned char sysex_buffer[SYSEX_BUFFER_SIZE];
    unsigned int sysex_count;

    int bPriorityChanged, oldPriorityLevel;
    DWORD oldPriorityClass;
    
} winmidiport_t;


#define winmidiport_buffer_full(_this)  ((_this->head == _this->tail - 1) || ((_this->head == BUFFER_SIZE - 1) && (_this->tail == 0)))
#define winmidiport_available(_this)  (_this->head != _this->tail)

static int winmidiport_open(fts_object_t *o, int ac, const fts_atom_t *at);
static void winmidiport_close(fts_object_t *o);
static void winmidiport_output(fts_object_t *o, fts_midievent_t *event, double time);
static char* winmidiport_output_error(int no);
static char* winmidiport_input_error(int no);
void CALLBACK winmidiport_callback_in(HMIDIIN hmi, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
static unsigned char* winmidiport_realloc_sysex_buffer(MIDIHDR* hdr, int size);


void CALLBACK 
winmidiport_callback_in(HMIDIIN hmi, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
    winmidiport_t *this = (winmidiport_t *) dwInstance;
    
    switch (wMsg) {
    case MIM_OPEN: 
        this->midiin_state = MIM_OPEN;
        break;
        
    case MIM_CLOSE:
        this->midiin_state = MIM_CLOSE;
        break;
        
    case MIM_DATA:
        this->midiin_state = MIM_DATA;
        
#if USE_CRITICAL_SECTIONS
        EnterCriticalSection(&this->critical_section);
#endif
        
        if ((this->hmidiin != NULL) && !winmidiport_buffer_full(this)) {
            this->incoming[this->head++] = dwParam1;
            if (this->head == BUFFER_SIZE) {
                this->head = 0;
            }
        }
        
#if USE_CRITICAL_SECTIONS
        LeaveCriticalSection(&this->critical_section);
#endif
        
        break;
        
    case MIM_LONGDATA:
    /* I don't handle the sysex message here. Instead, the
    winmidiport_dispatch() function checks if the buffer is done
        and then handles the sysex data. */
        this->midiin_state = MIM_LONGDATA;
        break;
        
    case MIM_ERROR:
        this->midiin_state = MIM_ERROR;
        break;
        
    case MIM_LONGERROR:
        this->midiin_state = MIM_LONGERROR;
        break;
        
    case MIM_MOREDATA:
        this->midiin_state = MIM_MOREDATA;
        break;
    }
}

void
winmidiport_dispatch(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
    winmidiport_t *this = (winmidiport_t *)o;
    int cur_fill, cur_send;
    int i;
    UINT err;
    MMRESULT res;
    char msg[256];
    
#if USE_CRITICAL_SECTIONS
    EnterCriticalSection(&this->critical_section);
#endif
    
    if (this->hmidiin != NULL) {
        
        /* handle the short midi messages */
        
        while (winmidiport_available(this)) {
            
            fts_midievent_t *event = NULL;
            
            DWORD msg = this->incoming[this->tail++];
            if (this->tail == BUFFER_SIZE) {
                this->tail = 0;
            }
            
            /* we have to make a distinction between the events that have two
            parameter values and those that take only one. */
            switch (msg_type(msg)) {
                
            case NOTEOFF:
                event = fts_midievent_channel_message_new(midi_note, msg_chan(msg), msg_p1(msg), msg_p2(msg));
                break;
                
            case NOTEON:
            case KEYPRESSURE:
            case CONTROLCHANGE:
            case PITCHBEND:
                event = fts_midievent_channel_message_new(msg_type_enum(msg), msg_chan(msg), msg_p1(msg), msg_p2(msg));
                break;
                
            case PROGRAMCHANGE:
            case CHANNELPRESSURE:
                event = fts_midievent_channel_message_new(msg_type_enum(msg), msg_chan(msg), msg_p1(msg), MIDI_EMPTY_BYTE);
                break;
                
            case SYSEX:
                break;
            }
            
            if (event != NULL) {
                fts_midiport_input((fts_midiport_t *) this, event, 0.0);
            }
        }
        
#if WINMIDI_SYSEXIN
        /* check for incoming sysex messages */
        for (i = 0; i < 2; i++) {
            
            if (this->inhdr[i].dwFlags & MHDR_DONE) {
                int size = this->inhdr[i].dwBytesRecorded;
                int j; 
                
                fts_log("[winmidiport]: Received sysex message\n");
                
                /* create a new midi event and send the sysex message to the
                midiport. start at position 1 to skip the start-of-sysex
                byte, and skip the end-of-sysex byte */
                if (size > 2) {
                    
                    fts_midievent_t *sysex = fts_midievent_system_exclusive_new();
                    
                    for (j = 1; j < size - 1; j++) {
                        fts_midievent_system_exclusive_append(sysex, this->inhdr[i].lpData[j]);
                    }
                    
                    fts_midiport_input((fts_midiport_t *) this, sysex, 0.0);
                }
                
                /* unprepare the buffer and flag it as available */
                fts_log("[winmidiport]: Unprepare sysex buffer\n");
                midiInUnprepareHeader(this->hmidiin, &this->inhdr[i], sizeof(MIDIHDR));
                this->inhdr[i].dwFlags = 0;
                
                /* prepare and add the buffer to the driver */
                fts_log("[winmidiport]: Prepare sysex buffer\n");
                err = midiInPrepareHeader(this->hmidiin, &this->inhdr[i], sizeof(MIDIHDR));
                
                if (err == MMSYSERR_NOERROR ) {
                    
                    fts_log("[winmidiport]: Add sysex buffer\n");
                    err = midiInAddBuffer(this->hmidiin, &this->inhdr[i], sizeof(MIDIHDR));
                    
                    if (err != MMSYSERR_NOERROR ) {
                        midiInUnprepareHeader(this->hmidiin, &this->inhdr[i], sizeof(MIDIHDR));
                        midiInGetErrorText(err, &msg[0], 256);
                        post("Warning: winmidiport: Couldn't add sysex buffer: %s\n", msg);
                        fts_log("Warning: winmidiport: Couldn't add sysex buffer: %s\n", msg);
                    }      
                    
                } else {
                    midiInGetErrorText(err, &msg[0], 256);
                    post("Error: winmidiport: Couldn't prepare sysex buffer: %s\n", msg);
                    fts_log("Error: winmidiport: Couldn't prepare sysex buffer: %s\n", msg);
                }
            }
        }
#endif 
        
    }
    
    if (this->hmidiout != NULL) {
        
        /* check if the outgoing sysex message is finished. */
        if (this->sysex_hdr.dwFlags & MHDR_DONE) {
            midiOutUnprepareHeader(this->hmidiout, &this->sysex_hdr, sizeof(MIDIHDR));
            this->sysex_hdr.dwFlags = 0;
            this->sysex_hdr.dwBytesRecorded = 0;
        }
        
        /* Check if there's new data to send */
        if ((this->sysex_count > 0) && (this->sysex_hdr.dwFlags == 0)) {
            
            this->sysex_hdr.dwFlags = 0;
            this->sysex_hdr.dwBufferLength = this->sysex_count;      
            this->sysex_hdr.dwBytesRecorded = this->sysex_count;
            
            memcpy(this->sysex_hdr.lpData, &this->sysex_buffer[0], this->sysex_count);
            
            res = midiOutPrepareHeader(this->hmidiout, &this->sysex_hdr, sizeof(MIDIHDR));
            
            if (res == MMSYSERR_NOERROR) {
                res = midiOutLongMsg(this->hmidiout, &this->sysex_hdr, sizeof(MIDIHDR));
                this->sysex_count = 0;
            }	  
            
            if (res != MMSYSERR_NOERROR) {
                char msg[256];
                midiOutGetErrorText(res, &msg[0], 256);
                post("Error: winmidiport: Couldn't send MIDI message: %s\n", msg);
                fts_log("Error: winmidiport: Couldn't send MIDI message: %s\n", msg);
            }
        }
    }
    
#if USE_CRITICAL_SECTIONS
    LeaveCriticalSection(&this->critical_section);
#endif

}

static char* 
winmidiport_output_error(int no)
{
    midiOutGetErrorText(no, winmidiport_error_buffer, 256);
    return winmidiport_error_buffer;
}

static char* 
winmidiport_input_error(int no)
{
    midiInGetErrorText(no, winmidiport_error_buffer, 256);
    return winmidiport_error_buffer;
}

/************************************************************
*
*  MIDI port interface methods
*
*/


static void
winmidiport_output(fts_object_t *o, fts_midievent_t *event, double time)
{
    winmidiport_t *this = (winmidiport_t *)o;
    MMRESULT res = MMSYSERR_NOERROR;

#if USE_CRITICAL_SECTIONS    
    EnterCriticalSection(&this->critical_section);
#endif
    
    if (this->hmidiout != NULL) {
        
        if (fts_midievent_is_channel_message(event)) {
            if (fts_midievent_channel_message_has_second_byte(event)) {
                
                res = midiOutShortMsg(this->hmidiout, 
                    msg_pack(fts_midievent_channel_message_get_status_byte(event),
                    fts_midievent_channel_message_get_first(event), 
                    fts_midievent_channel_message_get_second(event)));
                
            } else {
                
                res = midiOutShortMsg(this->hmidiout, 
                    msg_pack(fts_midievent_channel_message_get_status_byte(event),
                    fts_midievent_channel_message_get_first(event), 
                    0));
            }
            
        } else {
            
            switch(fts_midievent_get_type(event)) {
                
            case midi_system_exclusive:
                {
                    int i;
                    int size = fts_midievent_system_exclusive_get_size(event);
                    fts_atom_t *atoms = fts_midievent_system_exclusive_get_atoms(event);
                    
                    /* FIXME: If we can't store the message, drop it for
                    now. (FIXME means that this has to be fixed ;) */
                    if (this->sysex_count + size + 2 > SYSEX_BUFFER_SIZE) {
                        return;
                    }
                    
                    this->sysex_buffer[this->sysex_count++] = SYSEX;
                    
                    for (i = 0; i < size; i++) {
                        this->sysex_buffer[this->sysex_count++] = fts_get_int(atoms + i) & 0x7f;
                    }
                    
                    this->sysex_buffer[this->sysex_count++] = SYSEX_END;	  
                }
                break;
                
            case midi_real_time:
                res = midiOutShortMsg(this->hmidiout, msg_pack(fts_midievent_real_time_get_status_byte(event), 0, 0));
                break;
                
            default:
                break;
            }
        }
        
        if (res != MMSYSERR_NOERROR) {
            char msg[256];
            midiOutGetErrorText(res, msg, 256);
            post("Error: winmidiport: Couldn't send MIDI message: %s\n", msg);
            fts_log("Error: winmidiport: Couldn't send MIDI message: %s\n", msg);
        }
    }
    
#if USE_CRITICAL_SECTIONS
    LeaveCriticalSection(&this->critical_section);
#endif

}

static unsigned char* 
winmidiport_realloc_sysex_buffer(MIDIHDR* hdr, int newlen)
{
    unsigned char* newbuf;
    
    newbuf = fts_malloc(newlen);
    memcpy(newbuf, hdr->lpData, newlen);
    fts_free(hdr->lpData);
    
    hdr->lpData = newbuf;
    hdr->dwBufferLength = newlen;
    
    return newbuf;
}

/************************************************************
*
*  get midiport variable
*
*/
static void
winmidiport_get_state(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
    winmidiport_t *this = (winmidiport_t *)o;
    
    fts_set_object(value, o);
}

/************************************************************
*
*  class
*
*/


static int
winmidiport_open(fts_object_t *o, int ac, const fts_atom_t *at)
{ 
    winmidiport_t *this = (winmidiport_t *)o;
    MMRESULT res;
    UINT i, err, num;
    MIDIOUTCAPS out_caps;
    MIDIINCAPS in_caps;
    fts_symbol_t devname;
    char msg[256];
    int in_num = -1;
    int out_num = -1;
    int input = 1;
    fts_symbol_t mode;
    
    fts_midiport_init(&this->port);
    fts_midiport_set_input(&this->port);
    fts_midiport_set_output(&this->port, winmidiport_output);
    
#if USE_CRITICAL_SECTIONS
    InitializeCriticalSection(&this->critical_section);    
    EnterCriticalSection(&this->critical_section);
#endif
    
    this->head = 0;
    this->tail = 0;
    this->hmidiout = NULL;
    this->hmidiin = NULL;
    this->midiin_state = 0;
    
    this->sysex_count = 0;

    this->bPriorityChanged = 0;
    
    /* check for the device name */
    devname = fts_get_symbol_arg(ac, at, 0, fts_s_default);
    
    /* Check for read or write access. I am doing a pretty pedantic
    error checking. In my experience, it does not help to be easy on
    this settings since it might provoke a system deadlock later if
    two devices are opened with the wrong mode. */
    
    if (ac >= 2) {
        if (fts_is_symbol(at + 1)) {
            mode = fts_get_symbol( at + 1);
        } else {
            post("Warning: winmidiport: Invalid argument (mode)\n");
            fts_log("[winmidiport]: Invalid argument (mode)\n");
            goto error_recovery;
        }
    } else {
        post("Warning: winmidiport: Not enough arguments (mode)\n");
        fts_log("[winmidiport]: Not enough arguments (mode)\n");
        goto error_recovery;
    }
    
    if ((mode != fts_s_read) && (mode != fts_s_write) && (mode != fts_s_read_write)) {
        post("Warning: winmidiport: Invalid argument (mode=\"%s\")\n", fts_symbol_name(mode));
        fts_log("[winmidiport]: Invalid argument (mode=\"%s\")\n", fts_symbol_name(mode));
        goto error_recovery;
    }
    
    this->flags = (mode == fts_s_read)? WINMIDI_READ : (mode == fts_s_write)? WINMIDI_WRITE : WINMIDI_READ | WINMIDI_WRITE;
    
    /* open midi output device */
    if (this->flags & WINMIDI_WRITE) {
        
        /* check if there any midi devices installed */
        num = midiOutGetNumDevs(); 
        
        if (num == 0) {
            post("Warning: winmidiport: no MIDI out devices found\n");
            fts_log("[winmidiport]: No MIDI out devices found\n");
            
        } else {
            
        /* search the id of the default midi out (print out the list to
            the log for debugging) */
            
            for (i = 0; i < num; i++) {
                
                res = midiOutGetDevCaps(i, &out_caps, sizeof(MIDIOUTCAPS));
                
                if (res == MMSYSERR_NOERROR) {
                    fts_log("[winmidiport]: midi out port %d: \"%s\" (Mid=%d, Pid=%d)\n", i, out_caps.szPname, out_caps.wMid, out_caps.wPid); 
                    
                    if (strcmp(out_caps.szPname, fts_symbol_name(devname)) == 0) {
                        out_num = i;
                        fts_log("[winmidiport]: Found default midi out port in device list\n");
                    }
                    
                    /* if the user specified the "default" midi device, take the
                    first non-microsoft synth in the list */
                    if ((devname == fts_s_default) && (out_num < 0) && (strstr(out_caps.szPname, "Microsoft") == NULL)) {
                        out_num = i;
                        fts_log("[winmidiport]: Using midi out port \"%s\" as default device\n", out_caps.szPname);
                    }
                }
            }
            
            /* I am being tough here. But I think it's for the best. */
            if (out_num == -1) {
                post("Warning: winmidiport: Invalid MIDI out device (%s)\n", fts_symbol_name(devname));
                fts_log("[winmidiport]: Invalid MIDI out device (%s)\n", fts_symbol_name(devname));
                goto open_midiin;
            }
            
            /* try opening the default port */
            res = midiOutGetDevCaps(out_num, &out_caps, sizeof(MIDIOUTCAPS));
            
            if (res == MMSYSERR_NOERROR) {
                fts_log("[winmidiport]: Trying to open midi out port \"%s\"\n", out_caps.szPname);
                
                /*
                    [RS]
                    As the Microsoft Synth is not GM compatible, we inform the user 
                    that some stuff may not work correctly, and ask him/her to confirm
                    the use of the synth.
                
                    The message box may be localized using the following REG_SZ (string) registry key:
                
                    HKEY_LOCAL_MACHINE\SOFTWARE\ircam\jMax\<fts-version>\language
                
                    where <fts-version> will typically be "3.0.1-plugin" (in MusiqueLab).
                
                    If the string starts with 'f', this means "french",
                    if it starts with 'e', it means "english". If the key is not found french
                    is assumed.
                */
                 
                if ((strstr(out_caps.szPname, "Microsoft") != NULL) && 
                    (strstr(out_caps.szPname, "Map") == NULL)) {  /* No message for "Mappeur MIDI Microsoft" */

                    enum { eLangFR='f', eLangEN='e' };
                    int result;
                    int language = eLangFR;
                    const char *msg, *title;
                    char buffer[64];
                    int autoPrioritySwitch = 0;
                    
                    fts_log("[winmidiport]: showing microsoft warning message box\n");

                    if(!fts_get_regvalue_string("language", buffer, 63)) {

                        if(strlen(buffer) > 0) {
                            language = buffer[0];
                        }
                    }

                    switch(language)
                    {
                    case eLangFR:
                        msg = "Le synthétiseur logiciel Microsoft n'est pas compatible Général MIDI.\n"
                        "Des fonctionalités comme la réverbération ne pourront pas fonctionner correctement.\n"
                        "Souhaitez-vous tout de même l'utiliser ?";

                        title = "Avertissement FTS";
                        break;

                    default:
                    case eLangEN:
                        msg = "The Microsoft Synth is not compatible with General MIDI.\n"
                        "Some functionalities such as Reverb won't work correctly.\n"
                        "Do you want to use it anyway ?";

                        title = "FTS Warning";
                        break;
                    }

                    result = (MessageBox(NULL, msg, title,
                        MB_YESNO | MB_SYSTEMMODAL | MB_SETFOREGROUND | MB_ICONSTOP) == IDNO);
                    
                    if (result) {
                        post("Open MIDI out cancelled\n");
                        fts_log("[winmidiport]: Open MIDI out cancelled\n");
                        goto open_midiin;
                    }

                    /*
                        The following integer key:
                        HKEY_LOCAL_MACHINE\SOFTWARE\ircam\jMax\<fts-version>\useAutoPrioritySwitch
                    
                        when set to 1, will cause winmidiport to change the priority of the current
                        thread so that the base priority of the thread is inferior to 11.
                        With a base priority inferior to 11, the current thread won't interfere
                        with Microsoft Windows (tm) system operation.

                        See MDSN:
                        http://msdn.microsoft.com/library/default.asp?url=/library/en-us/dllproc/base/setthreadpriority.asp

                    */

                    fts_get_regvalue_int("autoPrioritySwitch", &autoPrioritySwitch);

                    if(autoPrioritySwitch) {

                        fts_log("[winmidi] Changing thread & process priority to avoid freezing with Microsoft Synth.\n");

                        this->oldPriorityClass = fts_get_win32_priority_class();
                        this->oldPriorityLevel = fts_get_win32_thread_priority();

                        // The following calls will set the base priority of the current
                        // thread to 11 (and then, theorically, the thread won't interfere
                        // with the Microsoft Synth).
                        fts_set_win32_priority_class(NORMAL_PRIORITY_CLASS);
                        fts_set_win32_thread_priority(THREAD_PRIORITY_HIGHEST);
                        this->bPriorityChanged = 1;
                    } else {
                        fts_log("[winmidi] Warning: Microsoft Synth is known to interfere with FTS if FTS's thread's priority is too high.\n");
                    }

                }
                
                // Open the port
                fts_log("[winmidiport]: Calling midiOutOpen() to open midi out port \"%s\"\n", out_caps.szPname);
                err = midiOutOpen(&this->hmidiout, out_num, 0, 0, CALLBACK_NULL);
                fts_log("[winmidiport]: Returned from midiOutOpen()\n");
                
                // Check errors
                if (err != MMSYSERR_NOERROR) {
                    post("Warning: winmidiport: couldn't open default MIDI out device: %s (error %d)\n", winmidiport_output_error(err), err);
                    fts_log("[winmidiport]: Couldn't open default MIDI out device: %s (error %d)\n", winmidiport_output_error(err), err);
                    this->hmidiout = NULL;
                    goto open_midiin;
                    
                } else {
                    fts_log("[winmidiport]: Opened\n");
                }
            }
            
            this->sysex_hdr.lpData = fts_malloc(SYSEX_BUFFER_SIZE);
            this->sysex_hdr.dwBufferLength = SYSEX_BUFFER_SIZE;
            this->sysex_hdr.dwBytesRecorded = 0;
            this->sysex_hdr.dwUser = 0;
            this->sysex_hdr.dwFlags = 0;
            
        }
    }
    
    /* open midi input device */
    
open_midiin:
    
    if (this->flags & WINMIDI_READ) 
    {
        /* check if there any midi devices installed */
        num = midiInGetNumDevs(); 
        
        if (num == 0) 
        {
            post("Warning: winmidiport: no MIDI in devices found\n");
            fts_log("[winmidiport]: No MIDI in devices found\n");
            
        } 
        else 
        {
            
            for (i = 0; i < num; i++) {
                res = midiInGetDevCaps(i, &in_caps, sizeof(MIDIINCAPS));
                if (res == MMSYSERR_NOERROR) {
                    fts_log("[winmidiport]: MIDI in port %d: \"%s\"\n", i, in_caps.szPname);
                    if (strcmp(in_caps.szPname, fts_symbol_name(devname)) == 0) {
                        in_num = i;
                        fts_log("[winmidiport]: Found default midi in port in device list\n");
                    }
                }
            }
            
            if (in_num == -1) {
                
                /* I am being tough here. But I think it's for the best. */
                if (devname != fts_s_default) {
                    post("Warning: winmidiport: Invalid MIDI in device (%s)\n", fts_symbol_name(devname));
                    fts_log("[winmidiport]: Invalid MIDI in device (%s)\n", fts_symbol_name(devname));
                    goto graceful_exit;
                } else {
                    in_num = 0;
                }
            }
            
            /* try opening the default device  */
            res = midiInGetDevCaps(in_num, &in_caps, sizeof(MIDIINCAPS));
            if (res == MMSYSERR_NOERROR) {  
                fts_log("[winmidiport]: Trying to open midi in port \"%s\"\n", in_caps.szPname);
            }
            
            err = midiInOpen(&this->hmidiin, in_num, (DWORD) winmidiport_callback_in, (DWORD) this, CALLBACK_FUNCTION);
            
            if (err != MMSYSERR_NOERROR) {
                post("Warning: winmidiport: couldn't open default MIDI in device: %s (error %d)\n", winmidiport_input_error(err), err);
                fts_log("[winmidiport]: Couldn't open default MIDI in device: %s (error %d)\n", winmidiport_input_error(err), err);
                this->hmidiin = NULL;  
                goto graceful_exit;
                
            } else {
                fts_log("[winmidiport]: Opened\n");
            }
            
            
#if WINMIDI_SYSEXIN
            /* setup the buffers for incoming sysex messages */
            
            fts_log("[winmidiport]: Setting up buffers for sysex message\n");
            
            for (i = 0; i < 2; i++) {
                this->inhdr[i].lpData = fts_malloc(SYSEX_HEADER_SIZE);
                this->inhdr[i].dwBufferLength = SYSEX_HEADER_SIZE;
                this->inhdr[i].dwBytesRecorded = 0;
                this->inhdr[i].dwUser = i;
                this->inhdr[i].dwFlags = 0;
                
                /* prepare and add the buffer for incoming sysex messages */
                fts_log("[winmidiport]: Prepare sysex buffer\n");
                err = midiInPrepareHeader(this->hmidiin, &this->inhdr[i], sizeof(MIDIHDR));
                
                if (err == MMSYSERR_NOERROR ) {
                    
                    fts_log("[winmidiport]: Add sysex buffer\n");
                    err = midiInAddBuffer(this->hmidiin, &this->inhdr[i], sizeof(MIDIHDR));
                    if (err != MMSYSERR_NOERROR ) {
                        midiInGetErrorText(err, &msg[0], 256);
                        post("Warning: winmidiport: Couldn't add sysex buffer: %s\n", msg);
                        fts_log("Warning: winmidiport: Couldn't add sysex buffer: %s\n", msg);
                    }      
                    
                } else {
                    midiInGetErrorText(err, &msg[0], 256);
                    post("Error: winmidiport: Couldn't prepare sysex buffer: %s\n", msg);
                    fts_log("Error: winmidiport: Couldn't prepare sysex buffer: %s\n", msg);
                }
            }
#endif 
            
            /* start the midi input */
            err = midiInStart(this->hmidiin);
            if (err != MMSYSERR_NOERROR) {
                midiInGetErrorText(err, &msg[0], 256);
                post("Warning: winmidiport: failed to start the input device; midi input not available (%s)\n", msg);
                fts_log("[winmidiport]: Failed to start the input device; midi input not available (%s)\n", msg);
                goto error_recovery;
            } 
        }    
    }
    
graceful_exit:

#if USE_CRITICAL_SECTIONS    
    LeaveCriticalSection(&this->critical_section);
#endif
    
    return 0;
    
error_recovery:
    
    winmidiport_close(o);

    // [RS]: critical_section is already deleted in winmidiport_close...
    //LeaveCriticalSection(&this->critical_section);
    return -1;
}

static void 
winmidiport_close(fts_object_t *o)
{ 
    winmidiport_t *this = (winmidiport_t *)o;
    int i;
    HMIDIOUT hmidiout;
    HMIDIIN hmidiin;
    MMRESULT err;
    
    fts_log("[winmidiport]: Closing MIDI port\n");

#if USE_CRITICAL_SECTIONS    
    EnterCriticalSection(&this->critical_section);
#endif
    
    /* Get a local copy of the MIDI handles and set the handles of the
    port to NULL to avoid that midi callbacks access them */
    
    hmidiout = this->hmidiout;
    hmidiin = this->hmidiin;
    this->hmidiout = NULL;
    this->hmidiin = NULL;
    
#if USE_CRITICAL_SECTIONS
    LeaveCriticalSection(&this->critical_section);
#endif
    
    
    if (hmidiout != NULL) {
        
        /* mark the output buffers as done */
        if (midiOutReset(hmidiout) != MMSYSERR_NOERROR) {
            fts_log("[winmidiport]: midiOutReset returned an error\n");      
        }
        
        if (this->sysex_hdr.dwFlags != 0) {
            midiOutUnprepareHeader(hmidiout, &this->sysex_hdr, sizeof(MIDIHDR));
        }
        
        if ((err = midiOutClose(hmidiout)) != MMSYSERR_NOERROR) {
            if (err == MIDIERR_STILLPLAYING) {
                fts_log("[winmidiport]: midiOutClose returned MIDIERR_STILLPLAYING\n");      
            } else {
                fts_log("[winmidiport]: midiOutClose returned an error\n");      
            }
        }
        
        if (this->sysex_hdr.lpData != NULL) {
            fts_free(this->sysex_hdr.lpData);
            this->sysex_hdr.lpData = NULL;
        }
    }
    
    if (hmidiin != NULL) {
        
#if WINMIDI_SYSEXIN
        fts_log("[winmidiport]: Reset midi in\n");      
        
        if (midiInReset(hmidiin) != MMSYSERR_NOERROR) {
            fts_log("[winmidiport]: midiInReset returned an error\n");      
        }
        
        fts_log("[winmidiport]: Unpreparing midi in headers\n");      
        
        for (i = 0; i < 2; i++) {
            if ((err = midiInUnprepareHeader(hmidiin, &this->inhdr[i], sizeof(MIDIHDR))) != MMSYSERR_NOERROR) {
                if (err == MIDIERR_STILLPLAYING) {
                    fts_log("[winmidiport]: midiInUnprepareHeader returned MIDIERR_STILLPLAYING\n");      
                } else {
                    fts_log("[winmidiport]: midiInUnprepareHeader returned an error\n");      
                }
            }
        }
#endif
        
        fts_log("[winmidiport]: Closing midi in\n");      
        
        if ((err = midiInClose(hmidiin)) != MMSYSERR_NOERROR) {
            if (err == MIDIERR_STILLPLAYING) {
                fts_log("[winmidiport]: midiInClose returned MIDIERR_STILLPLAYING\n");      
            } else {
                fts_log("[winmidiport]: midiInClose returned an error\n");      
            }
        }
        
#if WINMIDI_SYSEXIN
        
        fts_log("[winmidiport]: Deleting midi headers\n");      
        
        for (i = 0; i < 2; i++) {
            if (this->inhdr[i].lpData != NULL) {
                fts_free(this->inhdr[i].lpData);
                this->inhdr[i].lpData = NULL;
            }
        }
#endif
        
    }
    
    fts_log("[winmidiport]: Done\n");

    // [RS]
    // If we changed the priority of the FTS thread, we restore it (see winmidiport_open).
    if(this->bPriorityChanged) {
        fts_log("[winmidiport]: Restoring previous process/thread priority class/level... \n");

        fts_set_win32_priority_class(this->oldPriorityClass);
        fts_set_win32_thread_priority(this->oldPriorityLevel);
    }
    fts_log("[winmidiport]: Done.\n");


#if USE_CRITICAL_SECTIONS    
    DeleteCriticalSection(&this->critical_section);
#endif
}

static void
winmidiport_reopen(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
    ac--;
    at++;
    
    winmidiport_close(o);
    winmidiport_open(o, ac, at);
}

static void
winmidiport_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
    winmidiport_t *this = (winmidiport_t *) o;
    
    ac--;
    at++;
    
    winmidiport_open(o, ac, at);
    fts_sched_add(o, FTS_SCHED_ALWAYS);
}

static void 
winmidiport_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
    winmidiport_close(o);
    fts_sched_remove(o);
}

static fts_status_t
winmidiport_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
    fts_class_init(cl, sizeof(winmidiport_t), 1, 0, 0);
    
    fts_midiport_class_init(cl);
    
    fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, winmidiport_init);
    fts_method_define_varargs(cl, fts_SystemInlet, fts_s_reopen, winmidiport_reopen);
    fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, winmidiport_delete);
    fts_method_define_varargs(cl, fts_SystemInlet, fts_s_sched_ready, winmidiport_dispatch);
    
    /* define variable */
    fts_class_add_daemon(cl, obj_property_get, fts_s_state, winmidiport_get_state);
    
    return fts_Success;
}




/*************************************************
*
*  Win midi 
*
*/

typedef struct _winmidi_t
{
    fts_object_t object;
} winmidi_t;

static void 
winmidi_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
}

static void 
winmidi_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
}

static void 
winmidi_ins(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
    fts_atom_t a[64];
    fts_symbol_t name;
    MIDIINCAPS caps;
    int i, num;
    MMRESULT errCode;
    
    num = midiInGetNumDevs(); 
    
    if (num == 0) {
        return;
    }
    
    for (i = 0; i < num && i < 64; i++) {

        errCode = midiInGetDevCaps(i, &caps, sizeof(MIDIINCAPS));       

        if (errCode == MMSYSERR_NOERROR) {
            name = fts_new_symbol_copy(caps.szPname);
            fts_set_symbol(&a[i], name);
        } else {
            name = fts_new_symbol_copy("<invalid device>");
            fts_set_symbol(&a[i], name);
        }
    }
    
    fts_outlet_send(o, 0, fts_s_list, num, a);
}



static void 
winmidi_outs(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
    fts_atom_t a[64];
    fts_symbol_t name;
    MIDIOUTCAPS caps;
    int i, num;
    MMRESULT errCode;
    
    num = midiOutGetNumDevs(); 
    
    if (num == 0) {
        return;
    }
    
    for (i = 0; i < num && i < 64; i++) {

        errCode = midiOutGetDevCaps(i, &caps, sizeof(MIDIOUTCAPS));

        if (errCode == MMSYSERR_NOERROR) {
            name = fts_new_symbol_copy(caps.szPname);
            fts_set_symbol(&a[i], name);
        } else {
            name = fts_new_symbol_copy("<invalid device>");
            fts_set_symbol(&a[i], name);
        }
    }
    
    fts_outlet_send(o, 0, fts_s_list, num, a);
}

static fts_status_t
winmidi_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
    fts_class_init(cl, sizeof(winmidi_t), 1, 1, 0);
    
    fts_midiport_class_init(cl);
    
    fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, winmidi_init);
    fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, winmidi_delete);
    fts_method_define_varargs(cl, 0, fts_new_symbol("ins"), winmidi_ins);
    fts_method_define_varargs(cl, 0, fts_new_symbol("outs"), winmidi_outs);
    
    return fts_Success;
}



/*************************************************
*
*  Module config 
*
*/

void
winmidiport_config(void)
{
    fts_metaclass_install( fts_new_symbol("winmidiport"), winmidiport_instantiate, fts_always_equiv);
    fts_metaclass_install( fts_new_symbol("winmidi"), winmidi_instantiate, fts_always_equiv);
}
