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

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.widgets.*;

public class RuntimeErrorsTablePanel extends JPanel implements JMaxToolPanel{

  /*private final static Color selectedColor = new Color(204, 204, 255);*/
  static class RuntimeErrorsTableCellRenderer extends DefaultTableCellRenderer
  {
    public Component getTableCellRendererComponent(JTable table, Object obj, 
						   boolean selected, boolean hasFocus, int row, int column)
      {
	  super.getTableCellRendererComponent(table, obj, selected, hasFocus, row, column);
	  
	  setText((String) obj);
	  setIcon( ObjectSetViewer.getObjectIcon(((RuntimeError)((RuntimeErrorsTableModel)table.getModel())
						  .getListModel().getElementAt(row)).getObject()));
 
	  return this;
      }
  }

  public RuntimeErrorsTablePanel()
  {
    try
	{
	   runtimeErrorsSource = new FtsRuntimeErrors();
	}
    catch(IOException e)
	{
	    System.err.println("[RuntimeErrorsTablePanel]: Error in FtsRuntimeErrors creation!");
	    e.printStackTrace();
	}

    tableModel = new RuntimeErrorsTableModel(runtimeErrorsSource);
    table = new JTable(tableModel);
    table.setPreferredScrollableViewportSize(new Dimension(400, 200));
    table.setRowHeight(17);
    table.getColumnModel().getColumn(0).setPreferredWidth(100);
    table.getColumnModel().getColumn(0).setMaxWidth(150);
    table.getColumnModel().getColumn(2).setPreferredWidth(50);
    table.getColumnModel().getColumn(2).setMaxWidth(50);
    table.getColumnModel().getColumn(0).setCellRenderer(new RuntimeErrorsTableCellRenderer());

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
			
			if ((index >= 0) && (index < tableModel.getListModel().getSize()))
			    {
				if (objectSelectedListener != null)
				{
				    FtsGraphicObject object = ((RuntimeError)tableModel.
							       getListModel().getElementAt(index)).getObject();
				    
				    objectSelectedListener.objectSelected(object);
				}
			    }
		    }
	    }
	});
    table.addKeyListener(new KeyListener(){
	    public void keyTyped(KeyEvent e){}
	    public void keyPressed(KeyEvent e)
	    {
		if((e.getKeyCode() == KeyEvent.VK_DELETE)||(e.getKeyCode() == KeyEvent.VK_BACK_SPACE))
		    {
			runtimeErrorsSource.removeErrors(table.getSelectedRows());
		    }
	    }
	    public  void keyReleased(KeyEvent e){}
	});

    JScrollPane scrollPane = new JScrollPane(table);

    scrollPane.setAlignmentX( LEFT_ALIGNMENT);
    scrollPane.setAlignmentY( TOP_ALIGNMENT);

    setLayout( new BoxLayout( this, BoxLayout.Y_AXIS));
    
    add( scrollPane);

    tableModel.getListModel().addListDataListener(new ListDataListener(){
	    public void contentsChanged(ListDataEvent evt)
	    {
		table.revalidate();
		table.repaint();
		table.getSelectionModel().clearSelection();
	    };
	    public void intervalRemoved(ListDataEvent e){};
	    public void intervalAdded(ListDataEvent e){};
	});
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

  protected JTable table;
  protected RuntimeErrorsTableModel tableModel;
  protected FtsRuntimeErrors runtimeErrorsSource;
  private ObjectSelectedListener objectSelectedListener;
}




