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

package ircam.jmax.editors.patcher;

import java.awt.*;
import java.io.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;
import javax.swing.table.*;

import ircam.ftsclient.*;
import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.widgets.*;

public class ErrorTablePanel extends JPanel implements JMaxToolPanel{

  /*private final static Color selectedColor = new Color(204, 204, 255);*/
  static class ErrorTableCellRenderer extends DefaultTableCellRenderer
  {
    public Component getTableCellRendererComponent(JTable table, Object obj, 
						   boolean selected, boolean hasFocus, int row, int column)
      {
	  super.getTableCellRendererComponent(table, obj, selected, hasFocus, row, column);
	  
	  setText((String) obj);
	  setIcon( ErrorTablePanel.errorIcon);
	  
	  return this;
      }
  }

  public ErrorTablePanel()
  {
    try
	{
	    set = new FtsObjectSet();
	}
    catch(IOException e)
	{
	    System.err.println("[ErrorTablePanel]: Error in FtsObjectSet creation!");
	    e.printStackTrace();
	}

    try
	{
	    errorFinder = new FtsErrorFinderObject();
	}
    catch(IOException e)
	{
	    System.err.println("[ErrorTablePanel]: Error in FtsErrorFinderObject creation!");
	    e.printStackTrace();
	}

    tableModel = new ErrorTableModel(set);
    table = new JTable(tableModel);
    table.setPreferredScrollableViewportSize(new Dimension(400, 200));
    table.setRowHeight(17);
    table.getColumnModel().getColumn(0).setPreferredWidth(150);
    table.getColumnModel().getColumn(0).setMaxWidth(150);
    table.getColumnModel().getColumn(0).setCellRenderer(new ErrorTableCellRenderer());
    table.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);

    table.addMouseListener(new MouseListener(){
	    public void mouseEntered(MouseEvent e) {} 
	    public void mouseExited(MouseEvent e) {}
	    public void mousePressed(MouseEvent e) {}
	    public void mouseReleased(MouseEvent e) {}
	    public void mouseClicked(MouseEvent e)
	    {
		if (e.getClickCount() == 2)
		    {
			int index = table.rowAtPoint(e.getPoint());
			
			if ((index >= 0) && (index < set.getSize()))
			    {
				if (objectSelectedListener != null)
				    {
					FtsGraphicObject object = (FtsGraphicObject) set.getElementAt(index);
					
					objectSelectedListener.objectSelected(object);
				    }
			    }
		    }
	    }
	});
    
    JScrollPane scrollPane = new JScrollPane(table);

    scrollPane.setAlignmentX( LEFT_ALIGNMENT);
    scrollPane.setAlignmentY( TOP_ALIGNMENT);

    setLayout( new BoxLayout( this, BoxLayout.Y_AXIS));
    
    add( scrollPane);

    set.addListDataListener(new ListDataListener(){
	    public void contentsChanged(ListDataEvent evt)
	    {
		table.revalidate();
		table.repaint();
		table.getSelectionModel().clearSelection();
	    };
	    public void intervalRemoved(ListDataEvent e){};
	    public void intervalAdded(ListDataEvent e){};
	});
    
    FtsPatcherObject.addGlobalEditListener(new FtsEditListener(){	    
	    public void objectAdded(FtsObject object)
	    {
		if(!atomic) 
		    SwingUtilities.invokeLater(new Runnable() {
			    public void run()
			    { 
				findErrors();
			    }});
	    };
	    public void objectRemoved(FtsObject object)
	    {
		if(!atomic) 
		    SwingUtilities.invokeLater(new Runnable() {
			    public void run()
			    { 
				findErrors();
			    }});
	    };
	    public void connectionAdded(FtsConnection connection){};
	    public void connectionRemoved(FtsConnection connection){};
	    public void atomicAction(boolean active)
	    {
		atomic = active;
		if(!atomic) findErrors();
	    };
      });

    findErrors();
  }

  public void findErrors()
  {
      errorFinder.findErrors(set);
  }

  public void setObjectSelectedListener(ObjectSelectedListener objectSelectedListener)
  {
    this.objectSelectedListener = objectSelectedListener;
  }

  public void setSelectionListener(ListSelectionListener l)
  {
      table.getSelectionModel().addListSelectionListener(l);
  }

  /* ToolPanel interface */
  public ToolTableModel getToolTableModel()
  {
      return tableModel;
  }
  public ListSelectionModel getListSelectionModel()
  {
      return table.getSelectionModel();
  }
  
  private boolean atomic = false;
  
  private JTable table;
  private FtsObjectSet set;
  private FtsErrorFinderObject errorFinder;
  private ErrorTableModel tableModel;
  private ObjectSelectedListener objectSelectedListener;
  public static ImageIcon errorIcon = SystemIcons.get("_error_object_");
  public static ImageIcon patcherIcon = SystemIcons.get("_patcher_");
}




