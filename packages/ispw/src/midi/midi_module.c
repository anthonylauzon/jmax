#include "fts.h"

extern void Rreceive_config(void);
extern void Rsend_config(void);
extern void makenote_config(void);
extern void midi_config(void);
extern void midiformat_config(void);
extern void midiparse_config(void);
extern void stripnote_config(void);
extern void sustain_config(void);

static void
midi_module_init(void)
{
  Rreceive_config();
  Rsend_config();
  makenote_config();
  midi_config();
  midiformat_config();
  midiparse_config();
  stripnote_config();
  sustain_config();
}

fts_module_t midi_module = {"midi", "ISPW midi classes", midi_module_init};
