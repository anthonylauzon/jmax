package ircam.jmax.fts;

import java.io.*;
import java.util.*;

import ircam.jmax.mda.*;
import ircam.jmax.utils.*;

/** This class represent an Integer list in 
 *  FTS
 */

public class FtsAtomList extends FtsRemoteData
{
  /** Key for remote calls */

  static final int REMOTE_UPDATE = 1;
  static final int REMOTE_SET    = 2;

  MaxVector values = new MaxVector();

  public FtsAtomList()
  {
    super();
  }

  public int getSize()
  {
    return values.size();
  }

  public MaxVector getValues()
  {
    return values;
  }

  /** Get the list in text form; it should use the FtsParse unparser !!*/

  public String getValuesAsText()
  {
    StringBuffer buffer = new StringBuffer();
    boolean addBlank = false ;

    for (int i = 0; i < values.size(); i++)
      {
	Object element = values.elementAt(i);
	int n;

	if (addBlank)
	  buffer.append(" ");
	else
	  addBlank = true;

	buffer.append(element.toString());

	if (element.equals(";"))
	  {
	    buffer.append("\n");
	    addBlank = false;
	  }
      }

    return buffer.toString();
  }

  /** Set the list as text */
  
  public void setValuesAsText(String value)
  {
    values.removeAllElements();
    FtsParse.parseAtoms(value, values);
    changed();
  }

  /** Get the whole content  of the list from FTS */

  public void forceUpdate()
  {
    remoteCall(REMOTE_UPDATE);
    Fts.sync();
  }

  /** Declare that a range in the list has been changed
   * and this range need to be sent to FTS
   */

  public void changed()
  {
    remoteCall(REMOTE_SET, values);
  }


  /* a method inherited from FtsRemoteData */

  public final void call( int key, FtsMessage msg)
  {
    switch( key)
      {
      case REMOTE_SET:
	Object obj;

	values.removeAllElements();

	obj = msg.getNextArgument();

	while (obj != null)
	  {
	    values.addElement(obj);
	    obj = msg.getNextArgument();
	  }
	break;
      default:
	break;
      }
  }
}


