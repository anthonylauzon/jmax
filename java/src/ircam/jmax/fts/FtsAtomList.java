package ircam.jmax.fts;

import java.io.*;
import java.util.*;


/** This class represent an Integer list in 
 *  FTS
 */

public class FtsAtomList
{
  FtsObject object = null; 
  Vector values = new Vector();

  public FtsAtomList()
  {
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
    FtsParse.parseObjectArguments(value, values);
    changed();
  }

  /** Get the whole content  of the list from FST */

  public void forceUpdate()
  {
    if (object != null)
      {
	FtsServer.getServer().sendObjectMessage(object, -1, "update", null);
	FtsServer.getServer().syncToFts();
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
      FtsServer.getServer().sendSetAtomListMessage(object, values);
  }

  /** Saving as tcl, both embedded in a .tpa or in a table file*/

  public void saveAsTcl(PrintWriter pw)
  {
    // To be defined
  }
}


