/* $Id$
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
 * This file's authors:  Diemo Schwarz, 9.8.2002
 *
 * TODO:
 * - add selections
 */

/* 
 * $Log$
 */


#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <fts/fts.h>
#include "jmaxsdif.h"


#define DEBUG_OUTPUT	1


/* where the hell is that defined?  urgh, ugly brutal test definition */
#define fmat_t double

typedef struct
{
    jmaxsdif_t		base;
    double		starttime;	/* time of start message */
    double		offset;		/* read start time in file
					   (output at real time zero) */
    double		currenttime;	/* time of current frame in ms */
    fmat_t		*currentmatrix;	/* next matrix to be sent in jmax format */
    int			matricesleft;

#if !JMAXSDIF_JMAX3
    fts_alarm_t		alarm;
#endif   
} readsdif_t;



/* prototypes */
static void readsdif_init  (fts_object_t*, int, fts_symbol_t, int, const fts_atom_t*);
static void readsdif_delete(fts_object_t*, int, fts_symbol_t, int, const fts_atom_t*);
static void readsdif_open  (fts_object_t*, int, fts_symbol_t, int, const fts_atom_t*);
static void readsdif_start (fts_object_t*, int, fts_symbol_t, int, const fts_atom_t*);
static void readsdif_stop  (fts_object_t*, int, fts_symbol_t, int, const fts_atom_t*);

#if JMAXSDIF_JMAX3
static void readsdif_emitdata(fts_object_t*, int, fts_symbol_t, int, const fts_atom_t*);
#else
static void readsdif_emitdata(fts_alarm_t *alarm, void *o);
#endif

static void readsdif_schedulenext(fts_object_t *);

static int  readsdif_openfile(readsdif_t *this);
static int  readsdif_nextmatrix(readsdif_t *this);
static int  readsdif_readmatrix(readsdif_t *this);




/*
 *
 * Instantiation and System Methods
 *
 */


/* init object */
fts_status_t readsdif_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
    int numoutlet = 0;
    int i;

    /* post("instantiate ac=%d\n", ac); */

    /* process argument to determine number of outlets */
    if (ac >= 1  &&  fts_is_symbol(at))
    {
	SdifSelectionT sel;
	const char *name = (const char *) fts_symbol_name(fts_get_symbol(at));

	SdifGetFilenameAndSelection(name, &sel);

	/* num. outlets are number of select matrices or at least one
           outlet for all matrices, if none are selected */
	numoutlet = SdifListIsEmpty(sel.matrix) ?  1  
						:  SdifListGetNbData(sel.matrix);
    }

    fts_class_init(cl, sizeof(readsdif_t), 1, numoutlet + 1, NULL);

    /* declare system methods */
    fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init,   readsdif_init);
    fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, readsdif_delete);

    /* declare input message methods */
    fts_method_define_varargs(cl, 0, 		   s_open,	 readsdif_open);

    fts_method_define_varargs(cl, 0, 		   fts_s_bang,	 readsdif_start);
    fts_method_define_varargs(cl, 0, 		   fts_s_start,	 readsdif_start);

    fts_method_define_varargs(cl, 0, 		   fts_s_stop,	 readsdif_stop);
    fts_method_define_varargs(cl, 0, 		   s_close,	 readsdif_stop);

    /* type the outlets */
    for (i = 0; i < numoutlet; i++)
	fts_outlet_type_define(cl, i, fts_s_float, 0, 0);

    fts_outlet_type_define(cl, numoutlet, fts_s_bang, 0, 0);

    return fts_Success;
}


/*
 *
 * Message Interface Methods
 *
 */

static void readsdif_init(fts_object_t *o, int winlet, fts_symbol_t s, 
			  int ac, const fts_atom_t *at)
{
    readsdif_t *this = (readsdif_t *) o;

    /* init object */
    this->base.objectname = "readsdif";
    this->base.filename	= strdup("");
    this->base.file	= NULL;
    this->matricesleft  = 0;

    /* process arguments to get filename */
    while (ac)
    {
	if (fts_is_symbol(at))
	{
	    const char *name = fts_symbol_name(fts_get_symbol(at));
	    free(this->base.filename);
	    this->base.filename  = strdup(name);
#if DEBUG_OUTPUT
	    post("readsdif init ac=%d at='%s'\n", ac, name);
#endif
	}
	ac--;
	at++;
    }

#if !JMAXSDIF_JMAX3
    fts_alarm_init(&this->alarm, 0, readsdif_emitdata, this);
#endif
}


static void readsdif_delete(fts_object_t *o, int winlet, fts_symbol_t s, 
			    int ac, const fts_atom_t *at)
{
    readsdif_t *this = (readsdif_t *) o;

    readsdif_stop(o, winlet, s, ac, at);
    free(this->base.filename);
#if DEBUG_OUTPUT
    post("readsdif delete\n");
#endif
}


/* rather: set file name
 * arguments: filename (with selection)
 *
 * todo: OPEN should only open the file.  START should position on the
 * first frame after the given time (usually the first frame in the
 * file) and preread some data into the queue.  STOP should just stop
 * reading and not close the file.  CLOSE or DELETE should close the file.
 */
static void readsdif_open(fts_object_t *o, int winlet, fts_symbol_t s, 
			  int ac, const fts_atom_t *at)
{
    readsdif_t *this = (readsdif_t *) o;
    char *file;

    readsdif_stop(o, winlet, s, ac, at);

    /* parse argument
       todo: allow selection only, leave filename intact */
    if (ac && fts_is_symbol(at))
    {
	const char *name = (const char *) fts_symbol_name(fts_get_symbol(at));
	free(this->base.filename);
	this->base.filename  = strdup(name);
    }
    
#if DEBUG_OUTPUT
    post("readsdif open '%s'\n", this->base.filename);
#endif
    /* do not open the file here, since then we'd need a rewind
       function in start */
}



/* opens the file and starts reading
 * arguments: start [time], start reading at given time in file.  Can
 * be negative, then reading is delayed by the given time.
 */
static void readsdif_start(fts_object_t *obj, int winlet, fts_symbol_t s, 
			  int ac, const fts_atom_t *at)
{
    readsdif_t *this = (readsdif_t *) obj;

    readsdif_stop(obj, winlet, s, ac, at);

    /* parse argument */
    if (ac && fts_is_number(at))
	this->offset = fts_get_number_float(at);
    else
	this->offset = 0.0;

#if DEBUG_OUTPUT
    post("readsdif start %g\n", this->offset);
#endif

    /* actually open file --> rewind */
    if (readsdif_openfile(this))
    {
	this->base.state = state_running;
	this->starttime  = jmaxsdif_get_time();

	/* read first matrix, schedule for output at right time */
	readsdif_schedulenext(obj);
    }
    else
    {
	post("readsdif: start: file '%s' could not be opened\n", 
	     this->base.filename);
    }
}


static void readsdif_stop(fts_object_t *obj, int winlet, fts_symbol_t s, 
			  int ac, const fts_atom_t *at)
{
    jmaxsdif_t *base = (jmaxsdif_t *) obj;

#if DEBUG_OUTPUT
    post("readsdif stop\n");
#endif
    base->state = state_stopped;
    jmaxsdif_closefile(base);
}


static void readsdif_schedulenext(fts_object_t *obj)
{
    readsdif_t *this = (readsdif_t *) obj;
    double delta;

    /* read next matrix */
    if (readsdif_nextmatrix(this))
    {
	double now = jmaxsdif_get_time() - this->starttime;

	delta = (this->currenttime - this->offset) - now;
	if (delta < 0.0)
	    delta = 0.0;
	
	/*post("schedulenext %f (delta %f)\n", this->currenttime, delta);*/

	/* schedule next matrix, may be in 0 time */
#if JMAXSDIF_JMAX3	
	fts_timebase_add_call(timebase, obj, readsdif_emitdata, NULL, delta);
#else
	fts_alarm_set_delay(&this->alarm, delta);
	fts_alarm_arm(&this->alarm);
#endif
    }
    else
    {   /* bang last outlet to signal end of file */
	fts_outlet_bang(obj, fts_object_get_outlets_number(obj) - 1);
    }
}


#if JMAXSDIF_JMAX3
static void readsdif_emitdata(fts_object_t *obj, int winlet, fts_symbol_t s, 
			      int ac, const fts_atom_t *at)
#else
static void readsdif_emitdata(fts_alarm_t *alarm, void *obj)
#endif
{
    readsdif_t *this = (readsdif_t *) obj;

    if (this->base.state == state_running)
    {
	/* it's time to output the current matrix */
	fts_outlet_float(obj, 0, *this->currentmatrix);

	/* read next matrix, schedule next call of readsdif_emitdata */
	readsdif_schedulenext(obj);
    }
}



/*
 *
 * Data Input Methods
 *
 */


/* open file in this->filename
 * return: success 
 */
static int readsdif_openfile(readsdif_t *this)
{
    if ((this->base.file = SdifFOpen(this->base.filename, eReadFile)))
    {
	SdifErrorT*err;

	/* store our object in file structure */
	int u = SdifFAddUserData(this->base.file, this);
	assert(u == 0);

	SdifFReadGeneralHeader(this->base.file);
	SdifFReadAllASCIIChunks(this->base.file);
	if ((err = SdifFLastError(this->base.file)))
	    /* todo: SdifFNumErrors(file, eError) */
	{   /* close file on read error  */
	    post("readsdif: open: %s\n", err->UserMess);
	    jmaxsdif_closefile(&this->base);
	}
	else
	{
	    /* SdifFPrint* functions need this */
	    this->base.file->TextStream = stdout;	
#if DEBUG_OUTPUT
	    SdifFPrintAllASCIIChunks(this->base.file);
#endif
	}
    }

    this->matricesleft = 0;

    return this->base.file != NULL;
}


/**
 * readsdif_nextmatrix retrieves the next matrix from the current sdif
 * file and the time tag of its frame.  
 * output:  matrix in this->currentmatrix, frame time in this->currenttime
 * returns: false on end of file. 
 */
static int readsdif_nextmatrix(readsdif_t *this)
{
    int matrixfound = 0;

    /*post("nextmatrix (left %d)\n", this->matricesleft);*/
    while (!matrixfound  &&
	   SdifFCurrSignature(this->base.file) != eEmptySignature)
    {
	/* are there any matrices left in the frame to read? */
	if (this->matricesleft == 0)
	{   /* no: read next frame header */
	    if (!SdifFReadNextSelectedFrameHeader(this->base.file))
		return 0;		/* EXIT on eof */

	    /* re-initialise matrices left to read */
	    this->matricesleft = SdifFCurrNbMatrix(this->base.file);
	    this->currenttime  = SdifFCurrTime(this->base.file) * 1000.; /* in ms */
	}
	
	/* now we have either read a new frame header or there were still
	   unread matrices in the last frame */
	if (this->matricesleft > 0)
	{   /* go through one matrix */
	    SdifFReadMatrixHeader(this->base.file);

	    if (SdifFCurrMatrixIsSelected(this->base.file))
	    {
		if (readsdif_readmatrix(this))
		    matrixfound = 1;	/* EXIT with success */
		else
		    return 0;		/* EXIT on error */
	    }
	    else
	    {
		SdifFSkipMatrixData(this->base.file);
	    }
	    this->matricesleft--;
	}

	if (this->matricesleft == 0)
	{   /* no matrices left in frame to read, read next frame signature */
	    size_t bytesread;
	    SdifFGetSignature(this->base.file, &bytesread);
	}
    }

    /* don't schedule reading of next matrix on eof */
    return (matrixfound);
}


static int readsdif_readmatrix(readsdif_t *this)
{
    static fmat_t	blurb;

    size_t	bytesread = 0;
    SdifFileT*  file  = this->base.file;
    int		nrows = SdifFCurrNbRow(file);
    int		ncols = SdifFCurrNbCol(file);
    int		row, col;
    
#if DEBUG_OUTPUT
    post("readmatrix t=%g ms  nrows=%d  ncols=%d\n", 
	 this->currenttime, nrows, ncols);
#endif

    for (row = 0; row < nrows; row++)
    {
	bytesread += SdifFReadOneRow(file);

#if 0
	for (col = 1; col <= ncols; col++)
	{
	    /* copy value by value to jmax matrix */
	    this->currentmatrix = SdifFCurrOneRowCol(file, col);
	}
#else
	/* keep first value only */
	if (row == 0)
	{
	    blurb =  SdifFCurrOneRowCol(file, 1);
	    this->currentmatrix = &blurb;
	}
#endif
    }

    SdifFReadPadding(file, SdifFPaddingCalculate(file->Stream, bytesread));

    return 1;
}
