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

package ircam.jmax.toolkit.menus;

import java.util.*;

// import javax.swing.*;
import javax.swing.AbstractButton;

import ircam.jmax.fts.*;
import ircam.jmax.toolkit.actions.*;

public class DefaultHelpMenu extends EditorMenu {

  public DefaultHelpMenu()
  {
    super("Help");
    setHorizontalTextPosition( AbstractButton.LEFT);

    add( DefaultActions.statisticsAction, "System Info");
    addSeparator();
  }

  public void updateMenu()
  {
    if ( FtsHelpPatchTable.getNumSummaries() > numEntries) 
      {	      
	Enumeration en = FtsHelpPatchTable.getSummaries(); 
	int count = 1;
	while (en.hasMoreElements())
	  {
	    final String str = (String) en.nextElement();
	    if(count > numEntries)
	      {			      
		add( new OpenHelpSummaryAction( FtsHelpPatchTable.getHelpSummaryPatch(str)), str);
		numEntries++;
	      }
	    count++;
	  }
      }
  }

  private int numEntries = 0;
}
