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

package ircam.jmax.editors.table;

import ircam.jmax.toolkit.*;
import ircam.jmax.editors.table.renderers.*;
import ircam.jmax.editors.table.menus.*;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

/**
* A JPanel that is able to show the content of a TableDataModel.*/
public class TableDisplay extends /*PopupToolbarPanel*/JPanel
{
  
  /**
  * Constructor. */
  public TableDisplay(TablePanel tp)
  {
    super();
    
    panel = tp;
    setBackground(Color.white);
    
   /* addMouseListener(new MouseListener(){
      public void mouseClicked(MouseEvent e){}
      public void mousePressed(MouseEvent e){}
      public void mouseReleased(MouseEvent e){}
      public void mouseEntered(MouseEvent e){}
      public void mouseExited(MouseEvent e){
        gc.display("");
        gc.displayInfo("");
      }
    });
    addMouseMotionListener(new MouseMotionListener(){
      public void mouseMoved(MouseEvent e)
    {
        //if(getMenu().isVisible()) return;
        
        if(!gc.getToolManager().getCurrentTool().getName().equals("zoomer"))
        {
          int index = gc.getAdapter().getInvX( e.getX());
          double value = gc.getAdapter().getInvY( e.getY());
          if( index < 0) index = 0;
          
          if (index < gc.getDataModel().getSize() && index >=0)
            if( gc.isIvec())
              gc.display( "( "+index+" , "+(int)value+" )");
            else
              gc.display( "( "+index+" , "+value+" )");
        }
    }
      public void mouseDragged(MouseEvent e)
    {
        //if(getMenu().isVisible()) return;
        
        String toolName = gc.getToolManager().getCurrentTool().getName();
        if(!toolName.equals("zoomer"))
        {
          int index = gc.getAdapter().getInvX( e.getX());
          double value = gc.getAdapter().getInvY( e.getY());
          if( index < 0) index = 0;
          
          if (index < gc.getDataModel().getSize() && index >=0)
            if( gc.isIvec())
              gc.display( "( "+index+" , "+(int)value+" )");
            else
              gc.display( "( "+index+" , "+value+" )");
        }
    }
    });*/
}

/**
* Constructor with a table render */
public TableDisplay(TableRenderer tr, TableGraphicContext gc, TablePanel tp)
{
  super();
  this.tr = tr;
  panel = tp;
  this.gc = gc;
  setBackground(Color.white);
  //popup = new TablePopupMenu(this);
}

/*public JPopupMenu getMenu()
{
  return popup;
}*/

/**
* Specify which render to use for this TableDisplay */
public void setRenderer(TableRenderer tr)
{
  this.tr = tr;
}

public TableRenderer getRenderer()
{
  return this.tr;
}

public void setGraphicContext(TableGraphicContext tgc)
{
  gc = tgc;
  //popup = new TablePopupMenu(this);
}
public TableGraphicContext getGraphicContext()
{
  return gc;
}
/**
* The paint routine just delegates to the RenderManager
 * the representation of its data.*/
public void paintComponent(Graphics g) 
{
  tr.render(g, g.getClipBounds());
}

public int getDisplayMode()
{
  return tr.getMode();
}

public Frame getFrame()
{
  return panel.getEditorContainer().getFrame();
}

//--- Fields
TableRenderer tr;
//TablePopupMenu popup;
TableGraphicContext gc;
public TablePanel panel;
}

