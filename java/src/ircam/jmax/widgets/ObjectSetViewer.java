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

package ircam.jmax.widgets;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.fts.client.*;
import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;

public class ObjectSetViewer extends JPanel {

  private final static ImageIcon patcherIcon = JMaxIcons.patcher;
  private final static ImageIcon objectIcon = JMaxIcons.object;
  private final static ImageIcon inletIcon = JMaxIcons.inlet;
  private final static ImageIcon outletIcon =  JMaxIcons.outlet;

  static class ObjectCellRenderer extends DefaultListCellRenderer
  {
    public Component getListCellRendererComponent( JList jlist, Object obj, int i, boolean selected, boolean hasFocus)
    {
      super.getListCellRendererComponent(jlist, obj, i, selected, hasFocus);

      if (obj != null)
	{
	  String text = ((FtsGraphicObject)obj).getDescription();	   
	  if(text.equals(""))
	      text = ((FtsGraphicObject)obj).getComment();
	  setText( text);
	  setIcon(ObjectSetViewer.getObjectIcon((FtsGraphicObject)obj));
	}
      
      return this;
    }
  }

  class ObjectSetViewerMouseListener implements MouseListener
  { 
    public void mouseEntered(MouseEvent e) {} 
    public void mouseExited(MouseEvent e) {}
    public void mousePressed(MouseEvent e) {}
    public void mouseReleased(MouseEvent e) {}

    public void mouseClicked(MouseEvent e)
    {
      if (e.getClickCount() == 2)
	{
	    int index = jList.locationToIndex(e.getPoint());
	  
	    if ((index >= 0) && (index < jList.getModel().getSize()))
	    {
	      if (objectSelectedListener != null)
		{
		  FtsGraphicObject object = (FtsGraphicObject) jList.getModel().getElementAt(index);

		  objectSelectedListener.objectSelected(object);
		}
	    }
	}
    }
  }

  public ObjectSetViewer()
  {
    jList = new JList();

    jList.setBackground( Color.white);

    // Create the cell renderer
    jList.setCellRenderer( new ObjectCellRenderer());

    jList.addMouseListener(new ObjectSetViewerMouseListener());
    
    JScrollPane scrollPane = new JScrollPane();
    scrollPane.getViewport().setView( jList);

    scrollPane.setAlignmentX( LEFT_ALIGNMENT);
    scrollPane.setAlignmentY( TOP_ALIGNMENT);

    setLayout( new BoxLayout( this, BoxLayout.Y_AXIS));
    
    add( scrollPane);
  }

  public void setModel( ListModel model)
  {
      jList.setModel( model);
      repaint();  // sometimes, it seems that setModel() does not force a refresh ???
      model.addListDataListener(new ListDataListener(){
	      public void contentsChanged(ListDataEvent e)
	      {
		  jList.clearSelection();
	      }
	      public void intervalRemoved(ListDataEvent e){}
	      public void intervalAdded(ListDataEvent e){}
	  });
  }

  public void setObjectSelectedListener(ObjectSelectedListener objectSelectedListener)
  {
    this.objectSelectedListener = objectSelectedListener;
  }
  
  public void setSelectionListener(ListSelectionListener l)
  {
      jList.addListSelectionListener(l);
  }

  public static ImageIcon getObjectIcon(FtsGraphicObject obj)
  {
      ImageIcon icon;
      String className = obj.getClassName();

      if (obj instanceof FtsTemplateObject)
	  icon = objectIcon;
      else 
	{
	  icon = JMaxClassMap.getIcon( className);

	  if ( icon == null)
	    icon = objectIcon;
	}

      return icon;
  }  
  protected JList jList;
  private ObjectSelectedListener objectSelectedListener;
}




