/*************************************************************************
**************************************************************************
    serial.h        - Serial Routines
                      for Unix Machines
Author: W. Ritsch 16.01.97


Desc.:  used the flock of birds source as base (serknr.c)
*/


#ifndef SERIAL_H
#define SERIAL_H

#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <termio.h>               /* for TERMIO ioctl calls */
#include <termios.h>

#ifndef TRUE
#define TRUE    1
#define FALSE   0
#endif

#ifndef ON
#define ON      1
#define OFF     0
#endif

/*
    RX and TX buffers
*/
#define RXBUFSIZE 0x100             /* size of RX circular buffer */
#define TXBUFSIZE 50                /* size of TX buffer */

/*
    Labels for the Com Ports
*/
#define COM1 0                      /* com port 1 */
#define COM2 1                      /* com port 2 */


/*
    Serial Port Return Values
*/
#define NODATAAVAIL     -1
#define RXERRORS        -2
#define RXBUFOVERRUN    -4
#define TXBUFOVERRUN    -5




/*
    Serial Port Configuration Constants
*/
#define STOP_WORDLEN_PARITY 0x3     /* 1 start, 8 data, 1 stop, no parity */

/* Type def */

typedef struct {
  fts_object_t o;   /* MUST BE FIRST STRUCTURE MEMBER */
  long n;           /* the state of a last input */

  struct termios oldcom_termio;    /* save the old com config */
  struct termios com_termio;       /* for the new com config */
  short serialconfigsaved;        /* flag indicates serial port saved */

  short comport;            /* holds the comport # */
  long baud;                /* holds the current baud rate */

  int comhandle;              /* holds the comport handle */
  short rxerrors;             /* holds the rx line errors */

  long cycle;

  fts_alarm_t *alarm; /* for reading */
} serial_t;

/*
    Prototypes
*/





#endif /* SERIAL */



