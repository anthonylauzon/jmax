#ifndef _FTSAUDIO_H_
#define _FTSAUDIO_H_

/* types */

struct fts_audio_input_logical_device;
typedef struct fts_audio_input_logical_device fts_audio_input_logical_device_t;

struct fts_audio_output_logical_device;
typedef struct fts_audio_output_logical_device fts_audio_output_logical_device_t;


/*Include file for the audio I/O module */

extern fts_module_t fts_audio_module;

/*  Audio logical device access */

extern fts_audio_input_logical_device_t *fts_audio_get_input_logical_device(fts_symbol_t name);
extern fts_audio_output_logical_device_t *fts_audio_get_output_logical_device(fts_symbol_t name);

/* Audio logical device buffers access */

extern void fts_audio_set_pending_close(void);

extern fts_dev_t *fts_audio_get_input_device(fts_audio_input_logical_device_t *ldev);
extern fts_dev_t *fts_audio_get_output_device(fts_audio_output_logical_device_t *ldev);

extern fts_symbol_t fts_audio_get_input_ftl_function(fts_audio_input_logical_device_t *ldev);
extern fts_symbol_t fts_audio_get_output_ftl_function(fts_audio_output_logical_device_t *ldev);

extern fts_dev_t *fts_audio_get_output_device(fts_audio_output_logical_device_t *ldev);

extern int fts_audio_get_input_channels(fts_audio_input_logical_device_t *ldev);
extern int fts_audio_get_output_channels(fts_audio_output_logical_device_t *ldev);

extern int fts_audio_input_device_is_active(fts_audio_input_logical_device_t *ldev);
extern int fts_audio_output_device_is_active(fts_audio_output_logical_device_t *ldev);

extern int fts_audio_input_stream_is_installed(fts_audio_input_logical_device_t *ldev, int index);
extern int fts_audio_output_stream_is_installed(fts_audio_output_logical_device_t *ldev, int index);

extern float *fts_audio_get_input_buffer(fts_audio_input_logical_device_t *ldev, int index);
extern float *fts_audio_get_output_buffer(fts_audio_output_logical_device_t *ldev, int index);

extern void fts_audio_set_output_buffer_used(fts_audio_output_logical_device_t *ldev, int index);
extern int  fts_audio_is_output_buffer_used(fts_audio_output_logical_device_t *ldev, int index);

extern void fts_audio_activate_devices(void);
extern void fts_audio_deactivate_devices(void);


/* housekeeping */

extern void fts_audio_add_output_object(fts_audio_output_logical_device_t *ldev);
extern void fts_audio_remove_output_object(fts_audio_output_logical_device_t *ldev);
extern int  fts_audio_get_output_object_count(fts_audio_output_logical_device_t *ldev);
extern void fts_audio_add_scheduled_output_object(fts_audio_output_logical_device_t *ldev);
extern int  fts_audio_get_scheduled_output_object_count(fts_audio_output_logical_device_t *ldev);

extern void fts_audio_add_input_object(fts_audio_input_logical_device_t *ldev);
extern void fts_audio_remove_input_object(fts_audio_input_logical_device_t *ldev);
extern int  fts_audio_get_input_object_count(fts_audio_input_logical_device_t *ldev);
extern void fts_audio_add_scheduled_input_object(fts_audio_input_logical_device_t *ldev);
extern int  fts_audio_get_scheduled_input_object_count(fts_audio_input_logical_device_t *ldev);




/* handling the zeroing of unused out devices */

extern void fts_audio_add_all_zero_fun(void);
extern void fts_audio_add_unused_zero_fun(void);

/* default in and out  */

extern void fts_audio_set_default_in(fts_symbol_t in);
extern void fts_audio_set_default_out(fts_symbol_t out);

#endif


