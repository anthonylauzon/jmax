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

import java.io.*;
import java.awt.*;
import java.awt.event.*;

import javax.swing.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;

public class ObjectSetViewer extends JPanel {

  static class ObjectCellRenderer extends JLabel implements ListCellRenderer
  {
    private static ImageIcon patcherIcon = null;
    private static ImageIcon objectIcon = null;
    private static ImageIcon errorIcon = null;
    private static ImageIcon commentIcon = null;
    private static ImageIcon messageIcon = null;

    private final static Color selectedColor = new Color( 51, 153, 204);

    private static void loadImages()
    {
      patcherIcon = Icons.get("_patcher_");
      objectIcon  = Icons.get("_object_");
      errorIcon   = Icons.get("_error_object_");
      commentIcon = Icons.get("_comment_");
      messageIcon = Icons.get("_message_box_");
    }

    public ObjectCellRenderer() 
    {
      if (patcherIcon == null)
	loadImages();
      
      setOpaque(true);
    }

    public Component getListCellRendererComponent( JList jlist, Object obj, int i, boolean selected, boolean hasFocus)
    {
      if (obj != null)
	{
	  if ( selected)
	    setBackground( selectedColor);
	  else
	    setBackground( jlist.getBackground());

	  if (((FtsObject) obj).isError())
	    {
	      setText( ((FtsObject) obj).getDescription());
	      setIcon( errorIcon);
	    }
	  else if (obj instanceof FtsPatcherObject)
	    {
	      setText( ((FtsObject) obj).getDescription());
	      setIcon( patcherIcon);
	    }
	  else if (obj instanceof FtsMessageObject)
	    {
	      setText( ((FtsMessageObject) obj).getMessage());
	      setIcon( messageIcon);
	    }
	  else if (obj instanceof FtsCommentObject)
	    {
	      setText( ((FtsObject) obj).getDescription());
	      setIcon( commentIcon);
	    }
	  else
	    {
	      setText( ((FtsObject) obj).getDescription());
	      setIcon( objectIcon);
	    }
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
		  FtsObject object = (FtsObject) jList.getModel().getElementAt(index);

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
  }

  public void setObjectSelectedListener(ObjectSelectedListener objectSelectedListener)
  {
    this.objectSelectedListener = objectSelectedListener;
  }

  protected JList jList;
  private ObjectSelectedListener objectSelectedListener;
}


