#include "sampfilt.h"

sampfilt_t *sampfilt_tab = 0;

int 
make_sampfilt_tab(void)
{
  int i;

  if(sampfilt_tab) 
    return(1);
  
  sampfilt_tab = (sampfilt_t *) fts_malloc(SAMP_NPOINTS * sizeof(sampfilt_t));

  if ( !sampfilt_tab)
    return 0;

  for(i = 0; i < SAMP_NPOINTS; i++)
    {
      float f = i * (1.0f/SAMP_NPOINTS);

      sampfilt_tab[i].f1 = -.1666667f * f * (1-f) * (2-f);
      sampfilt_tab[i].f2 = .5f * (1+f) * (1-f) * (2-f);
      sampfilt_tab[i].f3 = .5f * (1+f) * f * (2-f);
      sampfilt_tab[i].f4 = -.1666667f * (1+f) * f * (1-f);
    }
  
  return(1);
}

