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
import java.awt.event.*;
import java.io.*;

import javax.swing.*;
import javax.swing.border.*;
import javax.swing.event.*;
import javax.swing.table.*;

import ircam.fts.client.*;
import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.widgets.*;

public class FinderTablePanel extends JPanel implements JMaxToolPanel{

  static class FinderTableCellRenderer extends DefaultTableCellRenderer
  {
    public Component getTableCellRendererComponent(JTable table, Object obj, 
						   boolean selected, boolean hasFocus, int row, int column)
      {
	  super.getTableCellRendererComponent(table, obj, selected, hasFocus, row, column);
	  
	  setText((String) obj);
	  setIcon(ObjectSetViewer.getObjectIcon((FtsGraphicObject)((FinderTableModel)table.getModel()).
						getListModel().getElementAt(row)));
	  
	  return this;
      }
  }

  public FinderTablePanel()
  {
    setLayout( new BoxLayout( this, BoxLayout.Y_AXIS));
    
    try
	{
	    set = new FtsObjectSet();
	}
    catch(IOException e)
	{
	    System.err.println("[FinderTablePanel]: Error in FtsObjectSet creation!");
	    e.printStackTrace();
	}

    try
	{
	    ftsFinder = new FtsFinderObject();
	}
    catch(IOException e)
	{
	    System.err.println("[FinderTablePanel]: Error in FtsFinderObject creation!");
	    e.printStackTrace();
	}

    /* ############ TextField ###################### */
    JPanel labelPanel = new JPanel();
    labelPanel.setBorder( new EmptyBorder( 15, 15, 15, 15));
    labelPanel.setLayout( new BoxLayout( labelPanel, BoxLayout.X_AXIS));
    labelPanel.setOpaque( false);

    JLabel label = new JLabel("Find: ");
    label.setHorizontalTextPosition(label.RIGHT);
    label.setDisplayedMnemonic('T');
    label.setToolTipText("The labelFor and displayedMnemonic properties work!");

    textField = new JTextField( 30);

    textField.setBackground( Color.white); // ???

    label.setLabelFor( textField);
    textField.getAccessibleContext().setAccessibleName( label.getText());
    textField.addActionListener( new ActionListener() {
      public void actionPerformed( ActionEvent event)
	{
	  find();
	}
    });

    labelPanel.add( label);
    labelPanel.add( textField);

    /* ############################################################ */

    tableModel = new FinderTableModel(set);
    table = new JTable(tableModel);
    table.setPreferredScrollableViewportSize(new Dimension(400, 200));
    table.setRowHeight(17);
    table.getColumnModel().getColumn(0).setCellRenderer(new FinderTableCellRenderer());
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

    add( labelPanel);
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
	 public void objectAdded(FtsObject object){};
	 public void objectRemoved(FtsObject object)
	 {
	   if(!atomic) 
	     SwingUtilities.invokeLater(new Runnable() {
		 public void run()
		 { 
		   find();
		 }});
	 };
	 public void connectionAdded(FtsConnection connection){};
	 public void connectionRemoved(FtsConnection connection){};
	 public void atomicAction(boolean active)
	 {
	   atomic = active;
	   if(!atomic) find();
	 };
       });
  }

 public void find()
  {
    String query;
    MaxVector args;
    Cursor temp = getCursor();

    setCursor( Cursor.getPredefinedCursor( Cursor.WAIT_CURSOR));

    query = textField.getText();
    args = new MaxVector();
    FtsParse.parseAtoms(query, args);
    
    if(args.size()>0)
	ftsFinder.find(set, args);

    setCursor(temp);
  }

  public void findFriends(FtsGraphicObject object)
  {
    if(object.isError()) return;

    Cursor temp = getCursor();
    setCursor( Cursor.getPredefinedCursor( Cursor.WAIT_CURSOR));
    textField.setText("");

    ftsFinder.findFriends(object, set);    

    setCursor(temp);
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
  
  protected JTable table;  
  private JTextField textField;
  private FtsObjectSet set;
  private FtsFinderObject ftsFinder;
  protected FinderTableModel tableModel;
  private ObjectSelectedListener objectSelectedListener;
}




