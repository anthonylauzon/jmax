package ircam.jmax.fts; 

import java.io.*;
import java.util.*;
import ircam.jmax.*;
import ircam.jmax.mda.*;


/** A FtsGroupData is the Max Data instance containing a set of Ftsobjects.
 * 
 */

public class FtsGroupData extends MaxTclData
{
  Vector group;
  FtsObject patcher;

  public FtsGroupData()
  {
    super(MaxDataType.getTypeByName("group"));
  }

  void setGroup(Vector which)
  {
    this.group = which;
  }

  Vector getGroup()
  {
    return group;
  }

  /** Get the content (a group) as a vector */

  public Object getContent()
  {
    return group;
  }
  
  public void  setContent(Object content)
  {
    group = (Vector) content;
  }


  /** Save the content (a patcher) as TCL code */

  public void saveContentAsTcl(PrintWriter pw)
  {
    // Sync to fts to be sure we have all the info.

    MaxApplication.getFtsServer().syncToFts();

    FtsObject aObject;
    for (Enumeration e=group.elements(); e.hasMoreElements();) {
      aObject = (FtsObject) e.nextElement();
      aObject.saveAsTcl(pw);
    }
  }
}






