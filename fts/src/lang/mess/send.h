#ifndef _SEND_H_
#define _SEND_H_

extern int fts_send_message_to_receives(fts_symbol_t name, fts_symbol_t selector, int ac, const fts_atom_t *at);
extern int fts_receive_exists(fts_symbol_t name);

#endif
