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
 */

#ifndef _UNIXFUNC_H_
#define _UNIXFUNC_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**********************************/
/* SOCKET STUFF from <netinet/in.h> */
/**********************************/
typedef uint32_t in_addr_t;
struct in_addr {
	in_addr_t s_addr;
};

struct sockaddr_in {
	unsigned char	sin_len;
	unsigned char	sin_family;
	unsigned short	int sin_port;
	struct	in_addr sin_addr;
	char	sin_zero[8];
};

#define INADDR_ANY ((in_addr_t) 0x00000000)

/**********************************/
/* SOCKET STUFF from <sys/socket> */
/**********************************/
struct sockaddr {
	unsigned char	sa_len;			/* total length */
	unsigned char	sa_family;		/* address family */
	char		sa_data[14];/* actually longer; address value */
};

#define AF_INET 2
#define SOCK_STREAM 1

typedef int ssize_t;

extern int socket (int __domain, int __type, int __protocol);
extern int accept(int __fd, const struct sockaddr *addr, int *addr_len);
extern int bind(int __fd, const struct sockaddr *addr, int addr_len);
extern int listen(int __fd, int __n);
extern ssize_t sendto(int __fd, const void *buf, size_t __n, int __flags, const struct sockaddr *addr, int addr_len);
extern uint32_t htonl(uint32_t __hostlong);
extern uint16_t htons(uint16_t __hostshort);

/**********************************/
/* FILE         from <stdlib.h>   */
/**********************************/
extern char *realpath(const char *path, char *resolved_path);

/***************************************/
/* FILE descriptors  from <unistd.h>   */
/***************************************/

extern int dup (int __fd);
extern int dup2 (int __fd, int __fd2);

#endif
