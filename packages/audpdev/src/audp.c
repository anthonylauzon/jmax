#include "audp.h"
#include <fcntl.h>


/*********************************************************/
/*                                                       */
/* AUDP Encoding/Decoding methods                        */
/*                                                       */
/*********************************************************/

/* AUDP init 
------------*/
audp_t* audp_init()
{
  audp_t* audp;
  
  audp = (audp_t*)malloc(sizeof(audp_t));
  audp->audp_header = (audp_header_t*)malloc(sizeof(audp_header_t));
 
  /* Audio header initialization */
  audp->audp_header->packet_size = 0;
  audp->audp_header->packet_id = 0;

  /* Default parameters  */
  audp->audp_header->format = audp_int16 | audp_local_endianess();
  audp->audp_header->channels = 2;
  audp->audp_header->sampling_rate = audp_sr44;
  audp_set_swapper(audp);
  audp->swap = 0;
  audp->head = 1;
  return audp;
}

/* AUDP encoding
---------------*/
int audp_encode(audp_t* audp, unsigned char* buffer,int len, unsigned char** packet)
{
  int ret;

  audp->audp_header->packet_size = len;
  ret = audp_header_to_mem(audp, *packet);

  if(len>MAX_AUDIO_SIZE)
    {
      printf("Buffer too long to be encoded\n");
      return -1;
    }

  *packet += HEADER_LEN;
  bcopy(buffer,*packet,len);
  if (audp->swap == 1)
    {
      audp->swapper(*packet, len);
    }
  *packet -= HEADER_LEN;
  return (len+HEADER_LEN);


}


/* AUDP decoding
----------------*/
int audp_decode(audp_t* audp, unsigned char* buffer, unsigned char* packet)
{
  int ret;

  ret = audp_mem_to_header(packet, audp);  
  packet += HEADER_LEN;
  if (audp->swap == 1)
    {
      audp->swapper(packet, audp->audp_header->packet_size);
    }
  bcopy(packet, buffer, audp->audp_header->packet_size);
  packet -= HEADER_LEN;
  return (audp->audp_header->packet_size);

}

/* AUDP delete 
--------------*/
void audp_delete(audp_t* audp)
{
  free(audp->audp_header);
  free(audp);
}


/***************************************************************/
/*                                                             */
/* Setting/Getting function                                    */
/*                                                             */
/***************************************************************/

/* Sampling Rate */

int audp_get_sampling_rate(audp_t* audp)
{

  switch(audp->audp_header->sampling_rate)
    {
    case audp_sr22:
      return 22050;
    case audp_sr32:
      return 32000;
    case audp_sr44:
      return 44100;
    case audp_sr48:
      return 48000;
    default:
      return -1;
    }

}

int audp_set_sampling_rate(audp_t* audp, int sampling_rate)
{
  switch(sampling_rate)
    {
    case 22050:
      audp->audp_header->sampling_rate = audp_sr22;
      return 22050;
    case 32000:
      audp->audp_header->sampling_rate = audp_sr32;
      return 32000;
    case 44100:
      audp->audp_header->sampling_rate = audp_sr44;
      return 44100;
    case 48000:
      audp->audp_header->sampling_rate = audp_sr48;
      return 48000;  
    default:
      /* Unknown rate: return the previous value */
      return audp_get_sampling_rate(audp);
    }

}

/* Format */
/* to be continued */


unsigned char audp_get_format(audp_t* audp)
{
  return (audp->audp_header->format & 0x3f);
}

unsigned char audp_set_format(audp_t* audp, unsigned char fm_mask)
{
  audp->audp_header->format = (audp->audp_header->format & 0xc0) | fm_mask;
  audp_set_swapper(audp);
  return(audp->audp_header->format);
}


/* Number of channels */

int audp_get_channels(audp_t* audp)
{
  return (int)audp->audp_header->channels;
}

int audp_set_channels(audp_t* audp, int channels)
{
  if (channels<=32)
    {
      return (audp->audp_header->channels = (unsigned char)channels);
    }
  else
    {
      return (audp_get_channels(audp));
    }
}


/* Endianess Setup */

void audp_set_LE(audp_t* audp) /* Set to Little Endian */
{

  if (audp_local_endianess() != audp_LE)
    {
      audp->swap = 1;
    }
  else
    {
      audp->swap = 0;
    }
  audp->audp_header->format = audp_LE | (audp->audp_header->format & 0x3f); 
}

void audp_set_BE(audp_t* audp) /* Set to Big Endian */
{

  if (audp_local_endianess() != audp_BE)
    {
      audp->swap = 1;
    }
  else
    {
      audp->swap = 0;
    }
  audp->audp_header->format = audp_BE | (audp->audp_header->format & 0x3f);
}

unsigned char audp_get_endian(audp_t* audp)
{
  return (audp->audp_header->format & 0xc0);
}


/* Swapping Function setup:
---------------------------*/
void audp_set_swapper(audp_t* audp)
{
  
  switch(audp_sizeof_format(audp->audp_header->format))
    {
    case 0:
      audp->swapper = audp_noswap;
      break;
    case 1:
      audp->swapper = audp_noswap;
      break;
    case 2:
      audp->swapper = audp_swap2;
      break;
    case 4:
      audp->swapper = audp_swap2;
      break;  
    default:
      printf("No swapping function for this format\n");
      break;
  }
}


/************************************************************/
/*                                                          */
/* audp openning/closing transport methods                  */
/*                                                          */
/************************************************************/

/******************/
/* audp_open_send */
/******************/
audp_t* audp_send_open(char addr[256], int port, int ttl) /* open transmiter */
{
  audp_t* audp;
  /*  struct hostent *hp = (struct hostent*)malloc(sizeof(struct hostent)); */
 

  audp = audp_init();

  /* UDP communication initialization */
  bzero(&audp->sockaddr, sizeof(audp->sockaddr));
  if (inet_aton( addr, &audp->sockaddr.sin_addr) == 0) 
    {
      struct hostent *h = gethostbyname(addr);
      if (h == NULL) 
	{
	  perror("unable to resolve destination address"); 
	  return NULL;
	}
      memcpy(&(audp->sockaddr.sin_addr), h->h_addr_list[0], sizeof(audp->sockaddr.sin_addr));
    }


  if((audp->sockd = socket(AF_INET, SOCK_DGRAM,0)) < 0)
    {
      perror("socket: socket creation error");
      return NULL;
    }
 

  if (IN_MULTICAST(ntohl(audp->sockaddr.sin_addr.s_addr))) 
    {
      /*   perror("multicast sender started"); */

      if (setsockopt(audp->sockd, IPPROTO_IP, IP_MULTICAST_TTL, (char *) &ttl, sizeof(ttl)) != 0) 
	{
	  perror("setsockopt IP_MULTICAST_TTL");
	  close(audp->sockd);
	  return NULL;
	}
    }
  
  audp->sockaddr.sin_port = htons(port);
  audp->sockaddr.sin_family = AF_INET;

  return audp;
  
}


/******************/
/* audp_open_recv */
/******************/


audp_t* audp_recv_open(char* addr, int port) /* open transmiter */
{
  audp_t* audp;
  struct hostent *hp;
  struct in_addr maddr;

  audp = audp_init();

  /* UDP communication initialization */

  bzero(&audp->sockaddr, sizeof(audp->sockaddr));
  bzero(&maddr, sizeof(maddr));

  if (addr[0] != '\0')
    {
      if (inet_aton(addr, &maddr) == 0) 
	{
	  if ((hp = gethostbyname(addr)))
	    {
	      memcpy(&maddr, hp->h_addr_list[0], sizeof(maddr));
	    }
	  else
	    {
	      perror("unable to resolve multicast group address");
	      return NULL;
	    }
	}
    }

  if((audp->sockd = socket(AF_INET, SOCK_DGRAM,0)) < 0)
    {
      perror("socket creation error");
      return NULL;
    }
  

  audp->sockaddr.sin_port = htons(port);
  audp->sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  audp->sockaddr.sin_family = AF_INET;

  if (bind(audp->sockd, (struct sockaddr *)&audp->sockaddr, sizeof(audp->sockaddr)) != 0)
    {
      perror("bind error");
      return NULL;
    }

  memcpy(&(audp->sockaddr.sin_addr), &maddr, sizeof(audp->sockaddr.sin_addr));

  if (IN_MULTICAST(ntohl(audp->sockaddr.sin_addr.s_addr))) 
  {
      struct ip_mreq  imr;
      		
      /*   perror("multicast receiver started"); */
      imr.imr_multiaddr.s_addr = audp->sockaddr.sin_addr.s_addr;
      imr.imr_interface.s_addr = INADDR_ANY;
      if (setsockopt(audp->sockd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *) &imr, sizeof(struct ip_mreq)) != 0) {
	perror("setsockopt IP_ADD_MEMBERSHIP");
	close(audp->sockd);
	return NULL;
      }      
    }
  

  /* set last format(header) received */

  memset(audp->current_format, 0, FORMAT_LEN);
  return audp;
  
}


/* Ending audp communication */
void audp_close(audp_t* audp)
{
  if (IN_MULTICAST(ntohl(audp->sockaddr.sin_addr.s_addr))) 
    {
      struct ip_mreq  imr;
      imr.imr_multiaddr.s_addr = audp->sockaddr.sin_addr.s_addr;
      imr.imr_interface.s_addr = INADDR_ANY;
      if (setsockopt(audp->sockd, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char *) &imr, sizeof(struct ip_mreq)) != 0) 
	{
	  perror("setsockopt IP_DROP_MEMBERSHIP");
	  /*   return NULL; */
	}
    }
  close (audp->sockd);
  audp_delete(audp);
  
}


/****************************************************************/
/*                                                              */
/* audp sending/receiving functions                             */
/*                                                              */
/****************************************************************/

/*************************/
/* audp sending function */
/*************************/

int audp_send(audp_t* audp, unsigned char* pbuff, int lenbuff)
{
  unsigned char* ptr = (unsigned char *)malloc(lenbuff + sizeof(audp_header_t));
  int udp_packet_size;
  int ret;
  fd_set f_write;
  struct timeval *tv;
  short test;

  tv = (struct timeval*)malloc(sizeof(struct timeval));

  if(audp->head == 1)
    {
      udp_packet_size = audp_encode(audp, pbuff, lenbuff, &ptr);
    }
  else
    {
      udp_packet_size = lenbuff;
      bcopy(pbuff, ptr, lenbuff);
    }
  tv->tv_sec = 1;
  tv->tv_usec = 0;
  
  FD_ZERO(&f_write);
  FD_SET(audp->sockd, &f_write);
  
  ret = select(audp->sockd+1,NULL, &f_write, NULL, tv);
  if(ret < 0)
    {
      perror("audp_send select");
      free(ptr);
      free(tv);
      return -1;
    }

  ret = -1;

  if (FD_ISSET(audp->sockd, &f_write))
   {
      ret = sendto(audp->sockd, ptr, udp_packet_size, 0, &audp->sockaddr, sizeof(audp->sockaddr));
      if (ret<0)
	{
	  free(ptr);
	  free(tv);
	  return -1;
	} 
      audp->audp_header->packet_id += 1;
   }
  free(ptr);
  free(tv);
  return ret;


}


/***************************/
/* audp receiving function */
/***************************/
/* audp_recv is a blocking function. It stays blocked on the "recvfrom" call until it receives a udp packet */

int audp_recv(audp_t* audp, unsigned char* pbuff)
{
  static unsigned char packet[MAX_UDP_SIZE];
  int ret;
  fd_set fdset;
  struct timeval tv;


  FD_ZERO(&fdset);
  FD_SET(audp->sockd, &fdset);
  memset(&tv, 0, sizeof(tv));

  select(audp->sockd+1, &fdset, NULL, NULL, &tv);

  if (!FD_ISSET(audp->sockd, &fdset))
    return 0;

  ret = recvfrom(audp->sockd, &packet, MAX_UDP_SIZE, 0, NULL, NULL);
  if (ret>0)
    {
      if (audp->head == 1)
	{
	  ret = audp_decode(audp, pbuff, &packet);
	}
      else
	{
	  bcopy(&packet, pbuff, ret);
	  audp->audp_header->packet_size = ret;
/*  	   perror("no head"); */
	}
      
    }

  return ret;

}

/************************************************************/
/*                                                          */
/* AUDP utility methods                                     */
/*                                                          */
/************************************************************/

/*********************/
/* Endianess checkup */
/*********************/
unsigned char audp_local_endianess()
{
union { int U; unsigned char C[4]; } u;
const unsigned char C[4] = { 1, 2, 4, 8 };
const int I4L = 0x08040201;
const int I4B = 0x01020408;


  u.C[0] = C[0];
  u.C[1] = C[1];
  u.C[2] = C[2];
  u.C[3] = C[3];   /* Implicit check that sizeof (int) is 4 */
  if (u.U == I4L)
    return audp_LE;
  else if (u.U == I4B)
    return audp_BE;
  else
    printf ("Unsupported endianess \n");

    return -1;
}




/* Size of format
----------------*/
int audp_sizeof_format(unsigned char format)
{
  switch(format & 0x3f)
    {
    case audp_undef:
      return 0;
      break;
    case audp_int16:
      return sizeof(audp_int16_t);
      break;
    case audp_uint16:
      return sizeof(audp_uint16_t);
      break;
    case audp_int32:
      return sizeof(audp_int32_t);
      break;
    case audp_uint32:
      return sizeof(audp_uint32_t);
      break;
    case audp_float32:
      return sizeof(audp_float32_t);
      break;
    default:
      return -1;
      break;
  }
}




/****************************************************************/
/*                                                              */
/* Buffer swapping function                                     */
/*                                                              */
/****************************************************************/

void audp_noswap(unsigned char *buffer, int buflen)
{
}

void audp_swap2(unsigned char *buffer, int bufflen)
{
  int i;
  unsigned char c;

  for (i=0; i<bufflen; i+=2)
    {
      c = buffer[i];
      buffer[i] = buffer[i+1];
      buffer[i+1] = c;
    }
}

void audp_swap4(unsigned char *buffer, int bufflen)
{
  int i;
  unsigned char c;

  for (i=0; i<bufflen; i+=4)
    {
      c = buffer[i];
      buffer[i] = buffer[i+3];
      buffer[i+3] = c;
      c = buffer[1];
      buffer[i+1] = buffer[i+2];
      buffer[i+2] = c;
    }
}

/****************************************************************/
/*                                                              */
/* Memory <--> audp_header                                      */
/*                                                              */
/****************************************************************/

/* audp_header --> memory
-------------------------*/
int audp_header_to_mem(audp_t* audp, unsigned char* ptr)
{
  unsigned char * cur = ptr;
  unsigned char c;

  bcopy(&audp->audp_header->format,cur,1);
  cur += 1;
  bcopy(&audp->audp_header->sampling_rate,cur,1);
  cur += 1;
  bcopy(&audp->audp_header->channels,cur,1);
  cur += 1;
  if (audp->swap == 1)
    {
      SWAP_2(audp->audp_header->packet_size, c);
      bcopy(&audp->audp_header->packet_size,cur,sizeof(audp->audp_header->packet_size));
      cur += sizeof(audp->audp_header->packet_size);
      SWAP_2(audp->audp_header->packet_size, c);
      SWAP_4(audp->audp_header->packet_id, c);
      bcopy(&audp->audp_header->packet_id,cur,sizeof(audp->audp_header->packet_id));
      cur += sizeof(audp->audp_header->packet_id); 
      SWAP_4(audp->audp_header->packet_id, c);
    }
  else
    {
      bcopy(&audp->audp_header->packet_size,cur,sizeof(audp->audp_header->packet_size));
      cur += sizeof(audp->audp_header->packet_size);
      bcopy(&audp->audp_header->packet_id,cur,sizeof(audp->audp_header->packet_id));
      cur += sizeof(audp->audp_header->packet_id);
    }
  return (cur - ptr);

}

/* memory --> audp_header
-------------------------*/
int audp_mem_to_header(unsigned char* ptr, audp_t* audp)
{
  unsigned char* cur = ptr;
  unsigned char c;
  int ret = 0;

  if (memcmp(ptr, audp->current_format, FORMAT_LEN) != 0)
    {
      bcopy(cur,audp->current_format, FORMAT_LEN); 
      bcopy(cur,&audp->audp_header->format,1);
      cur += 1;
      bcopy(cur,&audp->audp_header->sampling_rate,1);
      cur += 1;
      bcopy(cur,&audp->audp_header->channels,1);
      cur += 1;
      audp_set_swapper(audp);
      ret = 1;
    }
  else
    {
      cur += FORMAT_LEN;
    }
  if (audp_get_endian(audp) != audp_local_endianess())
    {
      audp->swap = 1;
      bcopy(cur,&audp->audp_header->packet_size,sizeof(audp->audp_header->packet_size));
      SWAP_2(audp->audp_header->packet_size, c);
      cur += sizeof(audp->audp_header->packet_size);
      bcopy(cur,&audp->audp_header->packet_id,sizeof(audp->audp_header->packet_id));
      SWAP_4(audp->audp_header->packet_id, c);
      cur += sizeof(audp->audp_header->packet_id);
    }
  else
    {
      audp->swap = 0;
      bcopy(cur,&audp->audp_header->packet_size,sizeof(audp->audp_header->packet_size));
      cur += sizeof(audp->audp_header->packet_size);
      bcopy(cur,&audp->audp_header->packet_id,sizeof(audp->audp_header->packet_id));
      cur += sizeof(audp->audp_header->packet_id);
    }
  return ret;

}



					    










