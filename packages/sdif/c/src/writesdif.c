/* $Id: writesdif.c,v 1.1 2002/08/30 14:07:13 schwarz Exp $
 *
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999, 2002 by IRCAM-Centre Georges Pompidou, Paris, France.
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

/*
  This file's authors:  Diemo Schwarz, 9.8.2002
 
  Message Interface:
  - INIT [str]:		str = filename::selection, store filename and 
			matrix selection
  - DEFINE typedef	add sdif description types in string typedef 
  - OPEN [name]:	store filename and matrix selection for output data,
			open the file
  - START/RECORD/bang:	set start time,
			rewind and truncate file and write the header
  - STOP/CLOSE:		stop writing and close the file
  - DELETE:		close the file if still open
  - PAUSE, toggle:	todo: pause recording

  - float/int:		write singleton matrix
  - list:		write matrix with list in one row
  - matrix:		todo: write matrix

  TODO: 
  - how to write multiple matrices in one frame?
  - how to write multiple frames in one file? --> multi inputs

  DONE:
  - remove mem leaks with selection/filename

 */

/* 
 * $Log: writesdif.c,v $
 * Revision 1.1  2002/08/30 14:07:13  schwarz
 * First working version of sdif package.
 * In/out of scalars (singleton matrices).
 * Definition of file-scope description types.
 * Compiles under jmax 2.5.3 and 3.
 */ 

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include <fts/fts.h>
#include <fmat.h>
#include "jmaxsdif.h"


#define DEBUG_OUTPUT	0


/* NOT const, since changed in putNVTgeneral by strtoNV */
static char CVSID [] = "$Id: writesdif.c,v 1.1 2002/08/30 14:07:13 schwarz Exp $";


typedef struct
{
    jmaxsdif_t		base;
    double		starttime;	/* time of start message */
    int			streamid;
    SdifSignature	framesig,	/* frame and matrix type to write */
			matrixsig;	/*   todo: copy list from selection */
} writesdif_t;



/* prototypes */
static void writesdif_init  (fts_object_t*, int, fts_symbol_t, int, const fts_atom_t*);
static void writesdif_delete(fts_object_t*, int, fts_symbol_t, int, const fts_atom_t*);
static void writesdif_open  (fts_object_t*, int, fts_symbol_t, int, const fts_atom_t*);
static void writesdif_define(fts_object_t*, int, fts_symbol_t, int, const fts_atom_t*);
static void writesdif_start (fts_object_t*, int, fts_symbol_t, int, const fts_atom_t*);
static void writesdif_stop  (fts_object_t*, int, fts_symbol_t, int, const fts_atom_t*);
static void writesdif_list  (fts_object_t*, int, fts_symbol_t, int, const fts_atom_t*);
static void writesdif_matrix(fts_object_t*, int, fts_symbol_t, int, const fts_atom_t*);

static int  writesdif_openfile(writesdif_t *this);
static int  writesdif_writeheader(writesdif_t *this);
static int  writesdif_writeframe(writesdif_t *this, SdifDataTypeET type, 
				 int nrow, int ncol, void *data);
static void writesdif_getselection(writesdif_t *this, SdifSelectionT *sel);
static void writesdif_parseselection(writesdif_t *this, const fts_atom_t *at);




/*
 *
 * Instantiation and System Methods
 *
 */



/* init object */
fts_status_t writesdif_instantiate(fts_class_t *c, int ac, const fts_atom_t *at)
{
    int numinlet = 0;
    int i;

#if DEBUG_OUTPUT
    post("writesdif instantiate ac=%d\n", ac); */
#endif

    /* process argument to determine number of inlets */
    if (ac >= 1  &&  fts_is_symbol(at))
    {
	const char *name = fts_symbol_name(fts_get_symbol(at));
	SdifSelectionT sel;

	SdifGetFilenameAndSelection(name, &sel);

	/* num. inlets are number of select matrices or at least one
           inlet for all matrices, if none are selected */
	numinlet = SdifListIsEmpty(sel.matrix) ?  1  
					       :  SdifListGetNbData(sel.matrix);
    }

    fts_class_init(c, sizeof(writesdif_t), numinlet ? numinlet : 1, 0, NULL);

    /* declare system methods */
    fts_method_define_varargs(c, fts_SystemInlet, fts_s_init,   writesdif_init);
    fts_method_define_varargs(c, fts_SystemInlet, fts_s_delete, writesdif_delete);

    /* declare input message methods */
    fts_method_define_varargs(c, 0, s_open,	 writesdif_open);

    fts_method_define_varargs(c, 0, s_define,	 writesdif_define);

    fts_method_define_varargs(c, 0, fts_s_bang,	 writesdif_start);
    fts_method_define_varargs(c, 0, fts_s_start, writesdif_start);
    fts_method_define_varargs(c, 0, s_record,	 writesdif_start);

    fts_method_define_varargs(c, 0, fts_s_stop,	 writesdif_stop);
    fts_method_define_varargs(c, 0, s_close,	 writesdif_stop);

    /* data input */
    fts_method_define_varargs(c, 0, fts_s_float, writesdif_list);
    fts_method_define_varargs(c, 0, fts_s_int,   writesdif_list);
    fts_method_define_varargs(c, 0, fts_s_list,  writesdif_list);
    fts_method_define_varargs(c, 0, fmat_symbol, writesdif_matrix);

    return fts_Success;
}




/*
 *
 * Message Interface Methods
 *
 */

static void writesdif_init(fts_object_t *o, int winlet, fts_symbol_t s, 
			  int ac, const fts_atom_t *at)
{
    writesdif_t *this = (writesdif_t *) o;

    /* init object */
    this->base.objectname = "writesdif";
    this->base.filename	= strdup("");	/* always allocate memory for name */
    this->base.file	= NULL;
    this->streamid	= 0;
    this->framesig	= eEmptySignature;
    this->matrixsig	= eEmptySignature;

    /* process arguments to get filename */
    while (ac)
    {
	if (fts_is_symbol(at))
	{
	    writesdif_parseselection(this, at);
#if DEBUG_OUTPUT
	    post("writesdif init ac=%d at='%s'\n", 
		 ac, fts_symbol_name(fts_get_symbol(at)));
#endif
	}
	ac--;
	at++;
    }
}


static void writesdif_delete(fts_object_t *o, int winlet, fts_symbol_t s, 
			    int ac, const fts_atom_t *at)
{
    writesdif_t *this = (writesdif_t *) o;

    writesdif_stop(o, winlet, s, ac, at);
    free(this->base.filename);
#if DEBUG_OUTPUT
    post("writesdif delete\n");
#endif
}


/* set file name, open file, but don't write headers yet
 * arguments: filename (with selection)
 */
static void writesdif_open(fts_object_t *o, int winlet, fts_symbol_t s, 
			  int ac, const fts_atom_t *at)
{
    writesdif_t *this = (writesdif_t *) o;
    char *file;

    writesdif_stop(o, winlet, s, ac, at);

    /* parse argument */
    if (ac && fts_is_symbol(at))
    {
	writesdif_parseselection(this, at);
    }

    /* open file given here or in init.  
       Do not write headers here, but in function in start (after a rewind) */
    writesdif_openfile(this);

#if DEBUG_OUTPUT
    post("writesdif open `%s'\n", this->base.filename);
#endif
}



/* define sdif types given in string arguments */
static void writesdif_define(fts_object_t *o, int winlet, fts_symbol_t s, 
			     int ac, const fts_atom_t *at)
{
    writesdif_t *this = (writesdif_t *) o;

    if (this->base.file)
    {
	/* parse argument */
	while (ac)
	{
	    if (fts_is_symbol(at))
	    {
		const char  *def = fts_symbol_name(fts_get_symbol(at));
		SdifStringT *str = SdifStringNew();

		SdifStringAppend(str, def);
		SdifFGetAllTypefromSdifString(this->base.file, str);
#if DEBUG_OUTPUT
		post("writesdif define <%s>\n", def);
		/* SdifFPrintAllType(this->base.file); */
#endif
	    }
	    ac--;
	    at++;
	}
    }
    else
    {
	post("writesdif: define: file must be opened before types can be defined\n");
    }
}


/* opens the file and starts reading
 * arguments: start [time], start reading at given time in file.  Can
 * be negative, then reading is delayed by the given time.
 */
static void writesdif_start(fts_object_t *obj, int winlet, fts_symbol_t s, 
			    int ac, const fts_atom_t *at)
{
    writesdif_t *this = (writesdif_t *) obj;

#if DEBUG_OUTPUT
    post("writesdif start\n");
#endif

    /* start writing */
    if (writesdif_writeheader(this))
    {
	this->base.state = state_running;
	this->starttime  = jmaxsdif_get_time();
    }
    else
    {
	post("writesdif: start: file `%s' could not be opened\n", 
	     this->base.filename);
    }
}


static void writesdif_stop(fts_object_t *obj, int winlet, fts_symbol_t s, 
			   int ac, const fts_atom_t *at)
{
    jmaxsdif_t *base = (jmaxsdif_t *) obj;

#if DEBUG_OUTPUT
    post("writesdif stop\n");
#endif
    base->state = state_stopped;
    jmaxsdif_closefile(base);
}


/* called on input of a scalar (numerical atom) or list */
static void writesdif_list(fts_object_t *obj, int winlet, fts_symbol_t s, 
			   int ac, const fts_atom_t *at)
{
#   define	MAX_ROW  256
    writesdif_t *this    = (writesdif_t *) obj;
    int		n	 = 0;
    float	data [MAX_ROW];

    if (this->base.state == state_running)
    {   /* get data */
	while (ac)
	{
	    if (fts_is_number(at))
	    {
		data[n++] = fts_get_number_float(at);
	    }
	    else
	    {
		post("writesdif: ignored non-numeric list member %d\n", n);
	    }
	    ac--;
	    at++;
	}
	writesdif_writeframe(this, eFloat4, 1, n, data);
    }
}


static void writesdif_matrix(fts_object_t *obj, int winlet, fts_symbol_t s, 
			     int ac, const fts_atom_t *at)
{
    writesdif_t *this    = (writesdif_t *) obj;
    fmat_t	*x	 = fmat_atom_get(at);
    int		m	 = fmat_get_m(x);
    int		n	 = fmat_get_n(x);
    float	*data    = fmat_get_ptr(x);

    /* if (fts_get_symbol(at) == fmat_symbol) */
    post("matrix (%d, %d) o %d\n", m, n, fts_is_object(at));
    x = (fmat_t*) fts_get_object(at);
    writesdif_writeframe(this, eFloat4, m, n, data);
}




/*
 *
 * Data Input Methods
 *
 */

/* open file in this->base.filename
 * return: success 
 */
static int writesdif_openfile(writesdif_t *this)
{
    /* check if all signatures given */
    if (this->framesig == eEmptySignature)
    {
	post("writesdif: output type not given!\n");
	return 0;
    }

    if (!this->base.filename  ||  !this->base.filename[0])
    {
	post("writesdif: no filename given!\n");
	return 0;
    }

    if ((this->base.file = SdifFOpen(this->base.filename, eWriteFile)))
    {
	time_t  now   = time (NULL);
	char    *user = getenv("USER");
	SdifNameValuesLT *nvt = this->base.file->NameValues;

#define nvsput(tag, value) SdifNameValuesLPutCurrNVT(nvt, tag, value)
#define nviput(tag, value) nvsput (tag, itoa(value))
#define nvfput(tag, value) nvsput (tag, ftoa(value))
#define nvcput(tag, value) SdifNameValuesLPutCurrNVTTranslate(nvt, tag, value)

	/* store our object in file structure */
	int u = SdifFAddUserData(this->base.file, this);
	assert(u == 0);

	/* Information table chunk creation */
	SdifNameValuesLNewTable(nvt, _SdifNVTStreamID);
	nvsput ("TableName",       "WriterInfo");
	nvsput ("WrittenBy",       "jMaxSdifWriter");
	nvsput ("Version",	   JMAXSDIFVERSION);
	nvcput ("SourceRevision",  CVSID);
	nvcput ("Date",		   ctime(&now));
	/* nvcput ("Machine",	   machine(0)??? ); */
	nvsput ("User",		   user ? user : "unknown");

	/* SdifFPrint* functions need this */    
	this->base.file->TextStream = stdout;
    }
    else
	this->base.file = NULL;

    return this->base.file != NULL;
}


/* rewind file and start writing on previously opened file in this->base.file
 * return: success 
 */
static int writesdif_writeheader(writesdif_t *this)
{
    if (this->base.file)
    {
	/* rewind and trucate */
	SdifFRewind(this->base.file);
	SdifFTruncate(this->base.file);

	/* Write general header and ascii chunks */
	SdifFWriteGeneralHeader(this->base.file);
	SdifFWriteAllASCIIChunks(this->base.file);

	if (SdifFLastError(this->base.file))	
	    /* todo: SdifFNumErrors(file, eError) */
	{   /* close file on write error  */
	    SdifFClose(this->base.file);
	    this->base.file = NULL;
	}
    }
    else
	this->base.file = NULL;

    return this->base.file != NULL;
}


/**
 * writesdif_writeframe
 */
static int writesdif_writeframe(writesdif_t *this, SdifDataTypeET type, 
				int nrow, int ncol, void *data)
{
    double now = jmaxsdif_get_time() - this->starttime;

    return SdifFWriteFrameAndOneMatrix(this->base.file, 
		this->framesig,  this->streamid, now / 1000.0, 
		this->matrixsig, type, nrow, ncol, data);
}


/** copy filename and stream, frame, matrix selection to object */
static void writesdif_parseselection(writesdif_t *this, const fts_atom_t *at)
{
    const char *name = fts_symbol_name(fts_get_symbol(at));
    SdifSelectionT sel;
	
    /* split filename from selection, parse selection elements */
    SdifGetFilenameAndSelection(name, &sel);
    
    /* copy data we're interested in */
    free(this->base.filename);
    this->base.filename = strdup(sel.filename);
    writesdif_getselection(this, &sel);
    
    /* free memory used */
    SdifFreeSelection(&sel);
}


/** copy stream, frame, matrix selection to object */
static void writesdif_getselection(writesdif_t *this, SdifSelectionT *sel)
{
    SdifSignature frm = SdifSelectGetFirstSignature(sel->frame,  eEmptySignature);
    SdifSignature mtx = SdifSelectGetFirstSignature(sel->matrix, eEmptySignature);

    /* set streamid and frame/matrix signatures if given, 
       leave old values otherwise */
    this->streamid  = SdifSelectGetFirstInt(sel->stream,       this->streamid);
    
    /* set both frame and matrix signatures if only one given */
    if (frm && !mtx)
    {
	this->framesig  = frm;
	this->matrixsig = frm;
    } 
    else if (mtx && !frm)
    {
	this->framesig  = mtx;
	this->matrixsig = mtx;
    }
    else if (frm && mtx)
    {
	this->framesig  = frm;
	this->matrixsig = mtx;
    }
    /* else: nothing given, leave values in object as they were */

#if DEBUG_OUTPUT
    post("getselection: `%s' -> (%s, %s) -> (%s, %s)\n", 
	 sel->filename,
	 SdifSignatureToString(frm), 
	 SdifSignatureToString(mtx), 
	 SdifSignatureToString(this->framesig), 
	 SdifSignatureToString(this->matrixsig));
#endif
}
