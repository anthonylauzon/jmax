/*
  function to install the correct device set for the given configuration
  */


#include "sys.h"
#include "runtime/devices/profdev.h"

#ifdef HAS_UNIX
#include "runtime/devices/unixdev.h"
#endif

#ifdef SGI
#include "runtime/devices/sgidev.h"
#endif

#ifdef HAS_OSS
#include "runtime/devices/ossdev.h"
#endif




void fts_dev_configure(void)
{
  profdev_init();

#ifdef HAS_UNIX
  unixdev_init();
#endif

#ifdef SGI
  sgidev_init();
#endif

#ifdef HAS_OSS
  ossdev_init();
#endif
}






