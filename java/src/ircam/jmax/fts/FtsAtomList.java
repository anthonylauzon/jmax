package ircam.jmax.fts;

import java.io.*;
import java.util.*;

import ircam.jmax.mda.*;
import ircam.jmax.utils.*;

/** This class represent an Integer list in 
 *  FTS
 */

public class FtsAtomList implements FtsDataObject
{
  FtsObject object = null; 
  Vector values = new Vector();

  public FtsAtomList()
  {
  }

  public FtsAtomList(FtsObject object)
  {
    this.object = object;
  }

  // Max Data implementation

  public MaxDocument getDocument()
  {
    return object.getParent().getDocument();
  }

  public String getName()
  {
    return object.getObjectName();
  }

  public int getSize()
  {
    return values.size();
  }

  public Vector getValues()
  {
    return values;
  }

  /** Set the corresponding FTS objetc */

  void setObject(FtsObject object)
  {
    this.object = object;
  }

  /** Get the list in text form */

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
    if (object != null)
      {
	Fts.getServer().sendObjectMessage(object, -1, "update", (Vector) null);
	Fts.sync();
      }
  }

  void updateFromMessage(FtsMessage msg)
  {
    int updateSize;

    updateSize = msg.getNumberOfArguments();

    updateSize -= 2; // Skip target and the "atomList" atom

    values.removeAllElements();

    for (int i = 0 ; i < updateSize; i++)
      values.addElement(msg.getArgument(i + 2));

  }

  /** Declare that a range in the list has been changed
   * and this range need to be sent to FTS
   */

  public void changed()
  {
    // Mandare un messaggio set_atom_list all'oggetto
    // Senza offset

    if (object != null)
      {
	Fts.getServer().sendSetMessage(object, values);
	object.setDirty();
      }
  }
}


