#ifndef _LIBSNDMMAP_H_
#define _LIBSNDMMAP_H_

typedef struct {
  snd_pcm_t *capture_handle;
  snd_pcm_t *playback_handle;
  char ***capture_addr;
  char ***playback_addr;
  int n_fragments;
  int n_channels;
  int frag_size;
  int fragment;
  int fd;
  int xrun;
} mmapdev_t;

extern int snd_open( mmapdev_t *d, int card, int dev, int subdev, int format, int sampling_rate, int fragment_size);
extern int snd_start( mmapdev_t *d);
extern int snd_close( mmapdev_t *d);

extern int snd_get_n_fragments( mmapdev_t *d);
extern int snd_get_n_channels( mmapdev_t *d);
extern int snd_get_frag_size( mmapdev_t *d);

extern int snd_get_xrun( mmapdev_t *d);
extern int snd_clear_xrun( mmapdev_t *d);

extern int snd_do_poll( mmapdev_t *d);

extern void *snd_get_capture_fragment( mmapdev_t *d, int channel);
extern void *snd_get_playback_fragment( mmapdev_t *d, int channel);

extern void snd_done_fragment( mmapdev_t *d);

#endif
