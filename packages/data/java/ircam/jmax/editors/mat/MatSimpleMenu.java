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

package ircam.jmax.editors.mat;

import java.awt.*;
import java.awt.event.*;
import java.awt.datatransfer.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.menus.*;
import ircam.jmax.toolkit.actions.*;

public class MatSimpleMenu extends EditorMenu
{
  EditorContainer container;
  MatDataModel model;
  
  public MatSimpleMenu(EditorContainer container)
  {
    super("Mat");

    this.container = container;
    model = ((MatPanel)container.getEditor()).getMatModel();

    setHorizontalTextPosition(AbstractButton.LEFT);
    setMnemonic(KeyEvent.VK_M);
    
		add( new EditorAction("Append Row", "append row", KeyEvent.VK_R, KeyEvent.VK_R, true){
      public void doAction(EditorContainer container)
      {
        model.requestAppendRow();
      }
    });
    		
    if(model.canAppendColumn())
    {
      add( new EditorAction("Append Column", "append col", KeyEvent.VK_L, KeyEvent.VK_C, true){
        public void doAction(EditorContainer container)
        {
          model.requestAppendColumn();
        }
      });
    }	    
    
    addSeparator();
            
    add( new EditorAction("Close", "close", KeyEvent.VK_C, KeyEvent.VK_W, true){
      public void doAction(EditorContainer container)
    {
        container.getEditor().close(true);
        System.gc();
    }
    });
  }
  
  public void setFont(Font font)
  {
    super.setFont(font);
    
    JMenuItem item;
    for(int i=0; i<getItemCount(); i++)
    {
      item = getItem(i);
      if(item != null)
        item.setFont(font);
    }
  }
}





