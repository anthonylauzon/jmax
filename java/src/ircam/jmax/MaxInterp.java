/* this class is just a normal interpreter with knowledge of the Ermes environment */

package ircam.jmax;

import tcl.lang.*;
import java.util.*;
import ircam.jmax.editors.ermes.*; // ????
import ircam.jmax.*;
import ircam.jmax.fts.*;

/**
 * An extension of the TCL interpreter with knowledge of MaxApplication
 */
public class MaxInterp extends Interp {
	public MaxInterp(Console con) {
		super(con);
	}
	
	public MaxInterp(String args[], Console con) {
		super(args, con);
	}
  
  ErmesObject getErmesObject(int id) {
	  ErmesObject aObject= null;
	  
	  if (MaxApplication.getApplication().itsSketchWindow == null) return null;
	  for (Enumeration e = MaxApplication.getApplication().itsSketchWindow.itsSketchPad.itsElements.elements();e.hasMoreElements();) {
	    aObject=(ErmesObject) e.nextElement();
	    if (aObject.itsFtsObject.getObjId() == id) break;
	  }
	  if (aObject == null) return null;
	  else return aObject;
	}
}
