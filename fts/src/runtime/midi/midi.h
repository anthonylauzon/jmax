#ifndef _FTSMIDI_H
#define _FTSMIDI_H

extern fts_module_t fts_midi_module;

struct _fts_midi_port;
typedef struct _fts_midi_port fts_midi_port_t;

extern fts_midi_port_t *fts_midi_get_port(int idx);

extern double *fts_get_midi_time_code_p(int idx); /*return a pointer to a long keeping
						  the midi time code of the port */

typedef  void (* midi_action_t) (fts_midi_port_t *p, int midi_ev, void *user_data, int argc, fts_atom_t *at);

extern void fts_midi_install_fun(fts_midi_port_t *port, int midi_ev, midi_action_t fun, void *user_data);
extern void fts_midi_deinstall_fun(fts_midi_port_t *port, int midi_ev, midi_action_t fun, void *user_data);
/* Macros identifing the MIDI_EV  in the above call.
   Note that the n argument to the _CH macros can be between 1 and 16.
   Should add at least the modulation wheel ...
*/


#define MIDI_NOTE                       0
#define MIDI_NOTE_CH(n)                 (MIDI_NOTE + (n))
#define MIDI_POLY_AFTERTOUCH            (MIDI_NOTE + 16 + 1)
#define MIDI_POLY_AFTERTOUCH_CH(n)      (MIDI_POLY_AFTERTOUCH + (n))
#define MIDI_CONTROLLER                 (MIDI_POLY_AFTERTOUCH + 16 + 1)
#define MIDI_CONTROLLER_CH(n)           (MIDI_CONTROLLER + (n))
#define MIDI_PROGRAM_CHANGE             (MIDI_CONTROLLER + 16 + 1)
#define MIDI_PROGRAM_CHANGE_CH(n)       (MIDI_PROGRAM_CHANGE + (n))
#define MIDI_CHANNEL_AFTERTOUCH         (MIDI_PROGRAM_CHANGE + 16 + 1)
#define MIDI_CHANNEL_AFTERTOUCH_CH(n)   (MIDI_CHANNEL_AFTERTOUCH + (n))
#define MIDI_PITCH_BEND                 (MIDI_CHANNEL_AFTERTOUCH + 16 + 1)
#define MIDI_PITCH_BEND_CH(n)           (MIDI_PITCH_BEND + (n))
#define MIDI_BYTE                       (MIDI_PITCH_BEND + 16 + 1)
#define MIDI_SYSEX                      (MIDI_BYTE + 1)
#define MIDI_REALTIME                   (MIDI_SYSEX + 1)
#define MIDI_MTC                        (MIDI_REALTIME + 1)

#define MAX_MIDI_EV                     MIDI_MTC

extern void fts_midi_send(fts_midi_port_t *p, long val);
#endif
