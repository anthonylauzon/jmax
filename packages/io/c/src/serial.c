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
 *
 *
 * Authors: W. Ritsch, François Déchelle
 */

/*
Desc.:  used the flock of birds source as base (serknr.c)
*/

#include <stdio.h>          /* general I/O */
#include <string.h>         /* for string commands */
#include "mess.h"
#include "serial.h"
/*
    Descriptors for the COM ports
    =============================

    sys_com_config gets built at run time in configserial via strcpy and
    strcat C library calls... the user MUST modify this code when compiling
    on a UNIX platform using the proper /dev/tty driver
*/


/*
  SGI IRIX - For the Silicon Graphics Platforms (using
  --------   the TERMIO interface)
 */
#define COMPORT_MAX 2

static char *sys_com_port[COMPORT_MAX] ={"/dev/ttyd1","/dev/ttyd2"};
static int serial_virgin[COMPORT_MAX] = { TRUE, TRUE};

#define OPENPARAMS O_RDWR

/*
  Baud Rates for 6DFOBs
  */
#define BAUDRATETABLE_LEN 7

static
long baudratetable[] = {115200L,
			57600L,
			38400L,
			19200L,
			9600L,
			4800L,
			2400L};   


/* --- PROTOS -- */
static void serial_read( serial_t *this);

static int saveserialconfig(serial_t* s);    /* open device comport & save state */
static int change_baud(serial_t* s,long b);  /* change baudrate */
static void restoreserialconfig(serial_t* s);/* restore old state and close */

static int send_serial_char(serial_t* s,unsigned char chr);
static int get_serial_char(serial_t* s);

/* -------------------- the serial object methods -------------------- */
static void
serial_init( fts_object_t *o, int winlet, fts_symbol_t *s, 
	      int ac, const fts_atom_t *at)
{
  serial_t *this = (serial_t *)o;

  /* i think arg 0 is the name of the object */
  this->comport = fts_get_long_arg(at, ac, 1, COM2); /* COM2=1,COM1=0 */
  this->baud = fts_get_long_arg(at, ac, 2, 9600l);   /* default 9600 */

  this->comhandle = -1;           /* holds the comport handle */
  this->serialconfigsaved = FALSE;/* flag indicates serial port saved */
  this->rxerrors = 0;             /* holds the rx line errors */

  saveserialconfig(this); /* open and remember old state */
  change_baud(this, this->baud); /* setup params */
}

static void
serial_exit( fts_object_t *o, int winlet, fts_symbol_t *s, 
	      int ac, const fts_atom_t *at)
{
  serial_t *this = (serial_t *)o;

  restoreserialconfig(this);

}


/* with bang zou trigger a reads until buffer empty,
    on SGI if there is a lot of chars in the inputbuffer
    it does not read them all at the same time, since the buffer
    is not updated immidiatly ??? dont know why :-| */

static void
serial_bang( fts_object_t *o, int winlet, fts_symbol_t *s,
	      int ac, const fts_atom_t *at)
{
  serial_t *this = (serial_t *)o;
  serial_read(this);

}

static void serial_read( serial_t *this)
  {
  fts_atom_t a;
  int in;

  /* while there are bytes, read them and send them out, ignore errors */
  while( (in = get_serial_char(this)) >= 0){
	 fts_set_long( &a, (long) in );
	 fts_outlet_send( (fts_object_t*) this, 0, fts_s_int, 1, &a);
  }

  if(in == RXERRORS){                  /* if an readerror detected */
	 if(this->rxerrors == 0)            /* post it once */
		post("serial:RXERRORS on serial line\n");
	 this->rxerrors = 1;                /* remember */
  }
/* automatic error clear disabled
  else
	 this->rxerrors = 0;                
*/
}

/* send a integer to the serial */
static void
serial_send( fts_object_t *o, int winlet, fts_symbol_t *s,
	     int ac, const fts_atom_t *at)
{
  serial_t *this = (serial_t *)o;
  fts_atom_t a;

  this->n = fts_get_long_arg( at, ac, 0, 0);
  send_serial_char(this,(char) (this->n & 0xFF));
}


/* set the params for the serial */
static void
serial_set( fts_object_t *o, int winlet, fts_symbol_t *s,
	     int ac, const fts_atom_t *at)
{
  serial_t *this = (serial_t *)o;
  fts_atom_t a;
  char *str;
  long val;
  fts_symbol_t *dumy,*sym;

  /*dumy = fts_new_symbol("dummy");*/

  if(fts_is_string(at) ) 
	 post("serial: String %s\n",(str = fts_get_string_arg( at, ac, 0, "DUMMY")));
  else  if(fts_is_symbol(at) ){

	 sym =  fts_get_symbol_arg( at, ac, 0, 0l);
	 val = fts_get_long_arg( at, ac, 1, 0);

	 str = (char *) fts_symbol_name(sym);

	 post("serial: Try to set %s = %d\n",str,val);

	 if(strcmp("baud",str) == 0 && val > 0){
		change_baud(this,val);
	 }
  }
  else
	 post("serial: first argument is no string or symbol");

  post("serial: settings Comport=%d, Baudrate=%ld\n",
		 this->comport,this->baud);
}

/* ------ the integer object instantiation function --------------- */
static fts_status_t
serial_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_atom_type_t a[3];

/*
  int port = fts_get_long_arg(at, ac, 0, COM2); 
  if(serial_virgin[port] == FALSE){
 	 post("serial: Comport %d already used",port);
	 return fts_fail; dont know what the fail argument 
  } 

  serial_virgin[port]= TRUE;
*/ 



  /* initialize the class */
  fts_class_init( cl, sizeof(serial_t), 1, 1, 0);

  /* define the methods */

  /* ser COMPORT(0 oder 1) Baudrate(long)  init */  
  a[0] = fts_Symbol;           /* ser */
  a[1] = fts_Long|fts_OptArg;  /* Comport 0,1 */
  a[2] = fts_Long|fts_OptArg;  /* Baudrate    */
  fts_method_define( cl, fts_SystemInlet, fts_s_init, serial_init, 3, a);

  /* ser exit */
  fts_method_define( cl, fts_SystemInlet, fts_s_delete, serial_exit, 0, 0l);

  /* send a byte cutted to 8 Bit */
  a[0] = fts_Long;
  fts_method_define( cl, 0, fts_s_int, serial_send, 1, a);

  /* get the inputs (later automatic with polling) */
  fts_method_define( cl, 0, fts_s_bang, serial_bang, 0, 0l );

  /* set the params, for debug */
  a[0] = fts_Symbol;                      /* variable name  */
  a[1] = fts_Long|fts_OptArg;             /* variable value */
  fts_method_define( cl, 0, fts_s_set, serial_set, 2, a );


  /* define outlet type */
  a[0] = fts_Long;
  fts_outlet_type_define( cl, 0, fts_s_int, 1, a );

  return fts_Success;
}

/* -------------------- the class installation function --------------- */
void
serial_config(void)
{
  /* create the class 'integer' */
  fts_metaclass_create(fts_new_symbol("serial"),serial_instantiate, 
							  fts_always_equiv);

/* ... and register  aliases for the "ser" name: "serial" */
  fts_metaclass_alias(fts_new_symbol("ser"), fts_new_symbol("serial"));
}




/* Help Functions ====================================================== */
/*
    set baudrate - Configure the Serial Port connected to the BIRD

    Prototype in:       serial.h

    Parameters Passed:  void

    Return Value:       TRUE if successfull, else FALSE

    Remarks:
*/

static int change_baud(serial_t *s,long baud)
{
  struct termios tempcom_termio;
  int i=0;

  /* verify Bauds take next low Bauds */

  while(i < BAUDRATETABLE_LEN && baudratetable[i] > baud)
	 i++;

       /* Baudrate not found ?? */
  if(i==BAUDRATETABLE_LEN){
	 post("serial: *Error* The baud rate %d is not suported or out of range\n",baud);
	 return(FALSE);
  }

  /* Get the Current Com Port Configuration */
  if (tcgetattr(s->comhandle, &tempcom_termio) >= 0)
    {
      /* Set the New Baud Rate */
      cfsetispeed(&tempcom_termio, baud);
      cfsetospeed(&tempcom_termio, baud);
      if (tcsetattr(s->comhandle, TCSANOW, &tempcom_termio)>= 0) {
	s->baud = baudratetable[i];    /* remember the choosen one */
	post ("serial: baudrate set to %d\n", baud);
	return(TRUE);
      }
    }

post("serial: ** ERROR ** could not set the COM port Baud Rate %ld\n",s->baud );
  return(FALSE);
}

/*
    saveserialconfig    -   save serial port configuration

    Prototype in:       serial.h

    Parameters Passed:  void

    Return Value:       void

    Remarks:            saves the current configuration of the serial port

*/

static int saveserialconfig(serial_t *s)
{
  /* if already opened, return FALSE */
  if (s->comhandle != -1)
	 return(FALSE);

  /* Open the Comport for RD and WR and get a handle */
  if ((s->comhandle = open(sys_com_port[s->comport], O_RDWR)) == -1)
    {
		post("serial: ** ERROR ** could not open device %s, failure %d (see man open)\n",
			  sys_com_port[s->comport],s->comhandle);
		perror( "serial\n");
		return(FALSE);
    }

  /* Save the Current Com Port Configuration  */
  if (tcgetattr(s->comhandle, &(s->oldcom_termio)) >= 0)
    { 
		s->serialconfigsaved = TRUE;

		/*  Setupt the new port configuration
		    NON-CANONICAL INPUT MODE
		    as defined in termio.h */
		s->com_termio.c_iflag = IXOFF;
		s->com_termio.c_oflag = 0;
		s->com_termio.c_cflag = CS8 | CLOCAL | CREAD; /* Default */
		s->com_termio.c_lflag = 0;
		s->com_termio.c_cc[VMIN] = 0;     /* setup to return after 0 seconds */
		s->com_termio.c_cc[VTIME] = 0;   /* ..if no characters are received */

		if (tcsetattr(s->comhandle, TCSANOW, &(s->com_termio)) >= 0) {
		  post("serial: Opened device %s OK\n", sys_com_port[s->comport]);
		  return(TRUE);
		} else {
		  post("serial: ** ERROR ** could not set params device %s\n",
		       sys_com_port[s->comport]);
		  return(FALSE);
		}
    }

  /* Error */
  post("serial: ** ERROR ** could not get params of device %s",
					sys_com_port[s->comport]);
  return(FALSE);
}

/*
    restoreserialconfig -   Restore the original serial port configuration

    Prototype in:       serial.h

    Parameters Passed:  void

    Return Value:       void

    Remarks:            restores the configuration of the serial port
*/
static void restoreserialconfig(serial_t *s)
{
  /*
	 Restore the Com Port Configuration.. if already opened
	 */
  if (s->comhandle != -1)
    {
		if( s->serialconfigsaved){
		  tcsetattr(s->comhandle, TCSANOW, &(s->oldcom_termio)); /* restore config */
		  s->serialconfigsaved = FALSE;
		}

		close(s->comhandle);    /* close the handle */
		
post("serial: Close serial line device  %s\n",sys_com_port[s->comport]);
		s->comhandle = -1;        /* make the comhandle 'look' closed */
    }
}

/*
  get_serial_char -   Get 1 Character from the serial port if one is available

  Prototype in:       serial.h

  Parameters Passed:  void

  Return Value:       returns the

  Remarks:            returns the receive character if successful,
                        RXERRORS if receive errors, NODATAAVAIL if no data
*/
static int get_serial_char(serial_t *s)
{
  unsigned char chr;
  int ret;

  if ((ret=read(s->comhandle, &chr,1)) > 0)
    {		
		return(chr);
    }
  else 
	 if(ret == 0)
		return(NODATAAVAIL);

  return(RXERRORS);
}

/*
    send_serial_char    -   Send one serial char to the serial port

    Prototype in:       serial.h

    Parameters Passed:  chr     -   character to send to the serial port

    Return Value:       returns TRUE if successful, or  TXNOTEMPTY if
                        cannot send because the holding register is not
                        empty

    Remarks:
*/
static int send_serial_char( serial_t *s, unsigned char chr)
{

  if ((write(s->comhandle,(char *) &chr,1)) == 1)
    {
		ioctl(s->comhandle,TCFLSH,TIOCFLUSH);  /* flush pending I/O chars */
        return(TRUE);
    }
    else
        return(TXBUFOVERRUN);
}



