//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 
// Based on Max/ISPW by Miller Puckette.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.editors.qlist;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.mda.*;
import ircam.jmax.utils.*;
import ircam.jmax.toolkit.*;

import java.awt.datatransfer.*;
import java.io.*;
import java.util.*;
import javax.swing.*;

/**
 * A concrete implementation of the SequenceDataModel,
 * this class represents a model of a set of tracks.
 */
public class FtsQListObject extends FtsObjectWithEditor {

  /**
   * constructor.
   */
    public FtsQListObject(Fts fts, FtsObject parent, String variableName, String classname, int nArgs, FtsAtom args[])
    {
	super(fts, parent, variableName, classname, classname + " " + FtsParse.unparseArguments(nArgs, args));
    }

    //////////////////////////////////////////////////////////////////////////////////////
    //// MESSAGES called from fts.
    //////////////////////////////////////////////////////////////////////////////////////

    /**
     * Fts callback: open the editor associated with this FtsSequenceObject.
     * If not exist create them else show them.
     */
    public void openEditor(int nArgs, FtsAtom args[])
    {
	if(getEditorFrame() == null)	    
	    setEditorFrame( new QList(this));
	
	showEditor();
    }

    /**
     * Fts callback: destroy the editor associated with this FtsSequenceObject.
     */
    public void destroyEditor(int nArgs, FtsAtom args[])
    {
	disposeEditor();
    }

    public void setAtomList(int nArgs, FtsAtom args[])
    {
      list = (FtsAtomList)args[0].getObject();
    }

    public FtsAtomList getAtomList()
    {
	return list;
    }

    /* messages to the server */

    public void closeEditor()
    {
	sendMessage(FtsObject.systemInlet, "close_editor", 0, null);
    }

    FtsAtomList list;
}











