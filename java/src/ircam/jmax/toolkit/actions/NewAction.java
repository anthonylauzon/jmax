 //
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// See file COPYING.LIB for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 

package ircam.jmax.toolkit.actions;

import java.io.*;
import java.awt.*;
import java.awt.event.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.toolkit.*;

public class NewAction extends EditorAction
{
  Frame frame;

   public NewAction()
   {
	  super("New patcher", "new patcher", KeyEvent.VK_N, KeyEvent.VK_N, true);
   }

  public void doAction(EditorContainer container)
  {
    frame = container.getFrame();
    frame.setCursor(Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR));
    FtsPatcherObject patcher = null;
    
    try
      {
	patcher = new FtsPatcherObject();
      }
    catch(IOException e)
      {
	System.err.println("[NewAction]: Error in FtsPatcherObject creation!");
	e.printStackTrace();
      }

    if(patcher != null)
      {
	patcher.requestOpenEditor();
	patcher.requestStopWaiting(new FtsActionListener(){
	    public void ftsActionDone()
	    {
	      frame.setCursor(Cursor.getDefaultCursor());
              frame = null;
	    }
	  });
      }
    else
      frame = null;
  }
}





