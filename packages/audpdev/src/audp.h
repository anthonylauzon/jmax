/*---------------audplib.h--------------------------------

Description:
-----------
  Methods prototypes and declarations for the audp library

Summary:
--------
  - The AUDP formalism (data structs)
  - The AUDP Encoding/Decoding methods
  - The AUDP setting methods
  - The AUDP transport (UDP) methods
  - The AUDP utility methods

----------------------------------------------------------*/

#include "udp.h"
#include "audp_types.h"
#include "audp_format.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

/* Constants:
-------------*/
/* Number of significative bytes to define the sound format */
#define FORMAT_LEN 3
/* Header length in bytes */
/* format + packet size + packet id */ 
#define HEADER_LEN 11 
/* Maximum size of the audio sample buffer */
#define MAX_AUDIO_SIZE (MAX_UDP_SIZE - HEADER_LEN)
   
/*****************************************************/
/*                                                   */
/* The AUDP formalism (data structs)                 */
/*                                                   */
/*****************************************************/

/* Type definitions for the audp header
---------------------------------------

unsigned char format:
--------------------

Masks for endianess: (unsigned char)
        audp_LE,  (Little Endian)
	audp_BE,  (Big Endian)

Masks for sample_format: (unsigned char)
	
	audp_undef,  (undefined format)
	audp_int16,  (16 bits signed integer)
	audp_uint16, (16 bits unsigned integer)
	audp_int32,  (32 bits signed integer)
	audp_uint32, (32 bits unsigned integer)
	audp_float32.(32 bits float)


unsigned char format = endianess | sample_format


unsigned char sampling_rate:
---------------------------

Masks for sampling_rate:
        audp_sr22, (Sampling rate = 22050Hz)
	audp_sr32, (Sampling rate = 32000Hz)
	audp_sr44, (Sampling rate = 44100Hz)
      	audp_sr48. (Sampling rate = 48000Hz)


unsigned char channels:
----------------------

       unsigned char channels = (unsigned char) nb_channels;
       (nb_channels must be an integer < 255)

audp_int16_t packet_size:
-------------------------

      The audio buffer size in bytes

audp_int32_t packet_id:
-----------------------

      The packet number

*/
typedef struct
{       
  /* Header information */
  unsigned char format;     
  unsigned char sampling_rate;
  unsigned char channels;
  audp_uint32_t packet_size;
  audp_int32_t packet_id;		   

}audp_header_t;


/* Type definition for the audp struct
--------------------------------------

- audp_header_t* audp_header: pointer to the associated audp_header struct
- struct sockaddr_in: socket address struct
- int sockd: socket file descriptor
- int swap: 0 don't swap / 1 swap, indicator to know if the datas need to be swapped
- void (* swapper): pointer to the swap method to use
       void audp_no_swap: for 8 bits data type
       void audp_swap2: for 16 bits data type
       void audp_swap4: for 32 bits data type
- unsigned char current_format: memory image of the last received format(sampling rate, bitrate, channels), use to know if there is any modification. If don't, we do not touch the header*/
typedef struct
{
  audp_header_t* audp_header;
  struct sockaddr_in sockaddr;
  int sockd;
  int swap;
  void (* swapper)(unsigned char* tab, int len);
  unsigned char current_format[FORMAT_LEN];
  int head; /* 0 = no header; 1 = header */
  int ttl; /* time to live (multicast)*/
  
}audp_t;


/*****************************/
/* audp functions prototypes */
/*****************************/

/*****************************************************/
/*                                                   */
/* The AUDP encoding/decoding methods                */
/*                                                   */
/*****************************************************/

/* AUDP init 
------------
audp_t struct inititialisation. It does the same thing than the audp_open function, without taking care of the transport. It allocates the audp_t struct with the default parameters. This function is used as well before encoding as before decoding operations.
<-- (audp_t) return a pointer to a new audp struct*/
audp_t* audp_init(void);


/* AUDP encoding
---------------
This function returns a pointer to a memory zone(audp packet) containing the audio datas with their respective audp header, and the lenght of the packet.
<-- (int) audp packet size in bytes
<--> (audp_t* audp) pointer to the audp struct
--> (void* buffer) pointer to the audio sample buffer to encode
--> (int len) audio sample buffer length in byte
<-- (void** packet) pointer to the pointer to the audp packet
*/
int audp_encode(audp_t* audp, unsigned char* buffer, int len, unsigned char** packet);


/* AUDP decoding
----------------
This function returns an pointer to the audio data buffer with its lenght, and fills the the local audp_t strcut with the header informations of the received packet.
<-- (int) return the audio sample buffer length in byte
<--> (audp_t* audp) pointer to the audp struct
<-- (void** buffer) pointer to the pointer to the audio sample buffer
--> (void* packet) pointer to the audp packet to decode*/
int audp_decode(audp_t* audp, unsigned char* buffer,unsigned char* packet);

/* AUDP delete 
--------------
This method free the memory allocated to the audp struct.
--> (audp_t* audp) pointer to the audp struct to delete*/
void audp_delete(audp_t* audp);



/*****************************************************/
/*                                                   */
/* The AUDP setting methods                          */
/*                                                   */
/*****************************************************/

/* audp header setting functions
-------------------------- 

Sampling Rate setting:
  - audp_get_sampling_rate:
    Return the audp sampling rate integer value
  - audp_set_sampling_rate:
    Set the audp sampling rate (integer), and return the new value in case of success, or the previous value if audp can't affoard the new one.

Format setting:
  - audp_get_format:
    Return the audp format mask (unsigned char)
  - audp_set_format:
     Set the audp format mask (unsigned char), and return the new value in case of success, or the previous value if audp can't affoard the new one.

Channels setting:
  - audp_get_channels:
    Return the audp number of channels (integer)
  - audp_set_channels:
    Set the audp number of channels (integer), and return the new value in case of success, or the previous value if audp can't affoard the new one.
*/
int audp_get_sampling_rate(audp_t* audp);
int audp_set_sampling_rate(audp_t* audp, int sampling_rate);

unsigned char audp_get_format(audp_t* audp);
unsigned char audp_set_format(audp_t* audp, unsigned char fm_mask); 

int audp_get_channels(audp_t* audp);
int audp_set_channels(audp_t* audp, int channels);


/* Endianess setup
--------------------
The audp header endianess setup is only needed by the audp sender. If the local endianess is different from the setting one the audp struct swap value is set to 1. The receiver get the endianess of the packet and swap the data when it's necessary */
 
void audp_set_LE(audp_t* audp); /* Set to Little Endian */
void audp_set_BE(audp_t* audp); /* Set to Big Endian */

/* Get the audp endianess */
unsigned char audp_get_endian(audp_t* audp);

/*int audp_get_size(audp_packet_t* ap);
  int audp_get_id(audp_packet_t* ap);*/



/*****************************************************/
/*                                                   */
/* The AUDP transport methods                        */
/*                                                   */
/*****************************************************/

/* audp communication openning functions:
-----------------------------------------
Description:
    Fill the structs needed to assure the UDP communication and prepare the audp header with the default parameters.

<-- audp_t*: udp communication and audp_header description
--> char* addr: destination/reception address
--> char* port: port number

Default audp parameters:
  format = 16bits integer & machine endianess
  sampling_rate = 44,1KHz
  channels = 2 (stereo)
  packet_size = 0 (empty)
  packet_id = 0 (the first sent will be 1) 
-------------------------------------------*/
audp_t* audp_send_open(char* addr, int port, int ttl); /* open transmiter */
audp_t* audp_recv_open(char* addr, int port); /* open receiver */

/* 2 possible architectures:
   - Direct communication:
   The transmiter and the receiver communicate directly. Then, the receiver is the server who's listenning on the correct port what the client transmiter are sending onto. This solution is certainly the most efficient in term of latency.

   - Communication via a server:
   The transmiter and the receiver are clients of a server. The server has two ports: a listenning port and a sending port. The transmiter send the audio data on the server's listenning port and the receiver listen to the server sending port. The server just make the link. This solution permit to easily introduce things like multicasting.  
*/


/* audp communication ending
----------------------------
Descrition:
    Free memory allocations

--> audp_t*: pointer to the audp to close
------------------------------*/ 
void audp_close(audp_t* audp);





/* Size of Format
-----------------
  Return the size(in bytes) of a data with the format passed in argument */
int audp_sizeof_format(unsigned char format);

/* audp sending function
-------------------------
Description:
  Sends the audio datas pointed by pbuff with the corresponding audp header in an udp packet to the destination port.

  <-- size of the sent packet or -1 if error
  --> pointer to the appropriate audp struct
  --> pointer to the audio datas to transmit
  --> audio sample buffer length
----------------------------------------*/
int audp_send(audp_t* audp, unsigned char* pbuff, int lenbuff);

/* audp receiving 
------------------
Description:
  Waits for receiving an audp packets from the udp communication port defined in the audp struct, fills the audp header and makes the pbuff point on the received audio datas. If there is no change on the audio format it return 0, else 1, and -1 if error. 
  audp_recv swap the datas if their endianess 

  <-- change indicator or -1 if error
  <--> pointer to the appropriate audp struct
  <--> pointer to the buffer ready to receive the audio samples
-----------------------------------------*/
int audp_recv(audp_t* audp, unsigned char* pbuff);


/*****************************************************/
/*                                                   */
/* The AUDP utility methods                          */
/*                                                   */
/*****************************************************/


/* Get local endianess:
-----------------------
  unsigned char audp_local_endianess(): Test the local endianess
    return audp_BE: Big Endian
    returm audp_LE: Little ENdian
*/
unsigned char audp_local_endianess(void);


/* Set swapper function:
------------------------
Descrition:
   Set the swapping function to use for the audp session depending of the audio data format*/
void audp_set_swapper(audp_t* audp);


/* Swapping functions:
----------------------*/
/* Swapper for buffer of 2 bytes long data */
void audp_swap2(unsigned char *buffer, int bufflen);
/* Swapper for buffer of 4 bytes long data */
void audp_swap4(unsigned char *buffer, int bufflen);
/* 1 byte data doesn't need to swapped */
void audp_noswap(unsigned char *buffer, int bufflen);


/****************************************************************/
/*                                                              */
/* Memory <--> audp_header                                      */
/*                                                              */
/****************************************************************/

/* audp_header --> memory
-------------------------
This method puts the header content into a memory zone.
<-- (int) return the audp header lenght in byte
<--> (audp_t* audp) pointer to the audp struct containing the audp header to put in memory
<-- (unsigned char* ptr) pointer to the memory that will contain the header*/
int audp_header_to_mem(audp_t* audp, unsigned char* ptr);

/* memory --> audp_header
-------------------------
This method fills the audp header with the datas contained in the memory pointed by ptr.
<-- (int) return 1 if the header is different from the last received header, else return 0
--> (unsigned char* ptr) pointer to the memory containing the header
<--> (audp_t* audp) pointer to the audp struct to fill*/
int audp_mem_to_header(unsigned char* ptr, audp_t* audp);

  









