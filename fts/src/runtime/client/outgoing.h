#ifndef _OUTGOING_H_
#define _OUTGOING_H_

/* Sending a message */


extern void fts_client_mess_start_msg(int type);
extern void fts_client_mess_add_int(int value);
#define fts_client_mess_add_long fts_client_mess_add_int
extern void fts_client_mess_add_float(float value);
extern void fts_client_mess_add_sym(fts_symbol_t s);
extern void fts_client_mess_add_object(fts_object_t *obj);
extern void fts_client_mess_add_connection(fts_connection_t *c);
extern void fts_client_mess_add_void(void);
extern void fts_client_mess_add_data( fts_data_t *data);
extern void fts_client_mess_add_string(const char *sp);
extern void fts_client_mess_add_atoms(int ac, const fts_atom_t *args);
extern void fts_client_mess_send_msg(void);
extern void fts_object_send_mess(fts_object_t *obj, fts_symbol_t selector, int argc, const fts_atom_t *args);

extern void fts_client_upload_object(fts_object_t *obj);
extern void fts_client_upload_connection(fts_connection_t *c);
extern void fts_client_upload_patcher_content(fts_patcher_t *patcher);

extern void fts_client_release_connection(fts_connection_t *c);
extern void fts_client_redefine_connection(fts_connection_t *c);
extern void fts_client_release_object(fts_object_t *c);
extern void fts_client_release_object_data(fts_object_t *obj);
#endif
