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

package ircam.jmax.toolkit;

import javax.swing.*;
import javax.swing.border.*;
import javax.swing.event.*;
import javax.swing.table.*;
import javax.swing.undo.*;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.io.*;
import java.lang.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.fts.client.*;

  /**
   * The graphic component containing the tracks of a Sequence.
   */
public class JMaxMatTable extends JTable 
{    
  FtsObjectCellEditor ftsObjEditor;
  FtsObjectCellRenderer ftsObjRenderer;
  JMaxTableListener listener;
  ComponentAdapter componentListener;
  DefaultCellEditor textCellEditor;
  
  public static final Color matGridColor = new Color(220, 220, 220);
  public static final Color rowsIdColor = new Color(245, 245, 245);
  public static final int COLUMN_MIN_WIDTH = 60;
  
  boolean shiftPressed = false;
  boolean ctrlPressed = false;
  
  class DeleteSelectionAction extends AbstractAction {
    DeleteSelectionAction(JMaxMatTable table)
	  {
      super("Delete Selection");
      this.table = table;
    }
    
    public void actionPerformed(ActionEvent e)
	  {
      table.getListener().deleteSelection();
    }
    JMaxMatTable table;
  } 

  class HideAction extends AbstractAction {
    HideAction(JMaxMatTable table)
    {
      super("Hide");
      this.table = table;
    }
    
    public void actionPerformed(ActionEvent e)
    {
      Container cont = table.getTopLevelAncestor();
      if(cont instanceof Window)
        ((Window)cont).setVisible(false);
    }
    JMaxMatTable table;
  } 
  
  public JMaxMatTable(TableModel model, JMaxTableListener listener)
  {
    super(model);
    this.listener = listener;
    
    ftsObjEditor = new FtsObjectCellEditor( this);
    ftsObjRenderer = new FtsObjectCellRenderer();
    
    setGridColor( matGridColor);
		setShowGrid(true);
    setSelectionMode(ListSelectionModel.SINGLE_INTERVAL_SELECTION);
    
    getInputMap().put( KeyStroke.getKeyStroke(KeyEvent.VK_BACK_SPACE, Toolkit.getDefaultToolkit().getMenuShortcutKeyMask()), "deleteSelection");
    getInputMap().put( KeyStroke.getKeyStroke(KeyEvent.VK_DELETE, Toolkit.getDefaultToolkit().getMenuShortcutKeyMask()), "deleteSelection");
    getActionMap().put("deleteSelection", new DeleteSelectionAction(this));
    getInputMap().put( KeyStroke.getKeyStroke(KeyEvent.VK_W, Toolkit.getDefaultToolkit().getMenuShortcutKeyMask()), "hide");
    getActionMap().put("hide", new HideAction(this));
  
    /*addMouseListener( new MouseAdapter() {
			public void mouseEntered(MouseEvent e)
      {
        requestFocus();
      }
		});*/        
  }
   
  public JMaxTableListener getListener()
  {
    return listener;
  }
  
  public TableCellEditor getCellEditor(int row,int col)
  {
    if( getModel().getValueAt(row, col) instanceof FtsObject)
      return ftsObjEditor;
    else
      return super.getCellEditor(row, col);
  }
  
  public TableCellRenderer getCellRenderer(int row,int col)
  {
    TableCellRenderer renderer;
    if( getModel().getValueAt(row, col) instanceof FtsObject)
      renderer = ftsObjRenderer;
    else
      renderer =  super.getCellRenderer(row, col);
    
    Component comp = renderer.getTableCellRendererComponent(this, getModel().getValueAt(row, col), false, false, row, col);
    if(highlighted.contains(new Integer(row)))
      comp.setBackground(Color.green);
    else
      comp.setBackground(Color.white);
    
    return renderer;
  }
  
  protected void processKeyEvent(KeyEvent e)
  {
    if(isEditing() || !(e.getKeyText(e.getKeyCode()).equals("Command") && e.getKeyChar() == KeyEvent.CHAR_UNDEFINED))
      super.processKeyEvent(e);
  }
  
  /* ???????????????????????????????????????????????????*/ 
  /*public void changeSelection(int row, int column, boolean toggle, boolean extend)
  {
    super.changeSelection(row, column, toggle, extend);
    if(editCellAt(row, column))
      getEditorComponent().requestFocusInWindow();
  }*/
  public boolean editCellAt(int row, int column, EventObject e)
  {
    boolean result = super.editCellAt(row, column, e);
    final Component editor = getEditorComponent();
    if (editor != null && editor instanceof JTextField)
    {
      if (e == null)
      {
        ((JTextField)editor).selectAll();
        editor.requestFocusInWindow();
      }
      else
      {
        SwingUtilities.invokeLater(new Runnable(){
          public void run()
          {
            ((JTextField)editor).selectAll();
            editor.requestFocusInWindow();
          }
        });
      }
    }
    return result;
  }
  public Component prepareEditor(TableCellEditor editor, int row, int column)
  {
    Component c = super.prepareEditor(editor, row, column);
    if (c instanceof JTextField)
      ((JTextField)c).selectAll();
    return c;
  }
  /* ??????????????????????????????????????????????????????? */
  
  Vector highlighted = new Vector();
  public void highlightLine(int index)
  {
    if(index < 0)
      highlighted.removeAllElements();
    else
      highlighted.addElement( new Integer(index));
  }
  /************************     FtsObject Table CellEditor ***********************************/
  public class FtsObjectCellEditor extends AbstractCellEditor implements TableCellEditor/*, ActionListener*/ 
  {
    FtsObject currentObject = null;
    JMaxMatTable table;
    JLabel editor;  
    
    public FtsObjectCellEditor(JMaxMatTable tab) 
    {
      this.table = tab;
      editor = new JLabel();
      editor.setBackground(Color.gray);
      editor.setBorder(BorderFactory.createEtchedBorder());
      editor.addMouseListener( new MouseAdapter(){
        public void mousePressed(MouseEvent e) 
        {
           if(e.getClickCount() > 1) 
          {
             SwingUtilities.invokeLater(new Runnable() {
               public void run()
               { 
                 if(currentObject instanceof FtsObjectWithEditor)
                   ((FtsObjectWithEditor)currentObject).requestOpenEditor();
               }
             });
             stopCellEditing();
             java.util.Timer timer = new java.util.Timer();
             timer.schedule(new StopTask(timer, FtsObjectCellEditor.this), 600);
          } 
        }
      });
    }
    
    class StopTask extends TimerTask {
      java.util.Timer timer;
      FtsObjectCellEditor editor;
      public StopTask(java.util.Timer timer, FtsObjectCellEditor editor)
      {
        this.timer = timer;
        this.editor = editor;
      }
      public void run() {
        editor.fireEditingStopped();
        timer.cancel(); //Terminate the timer thread
      }
    }    
    
    public Object getCellEditorValue() {
      return currentObject;
    }
    
    public Component getTableCellEditorComponent(JTable table, Object value, boolean isSelected, int row, int column) 
    {
      currentObject = (FtsObject)value;
      if(value instanceof FtsGraphicObject)
      {
        String name = ((FtsGraphicObject)value).getVariableName();
        String description = ((FtsGraphicObject)value).getDescription();
        
        if(name != null && !name.equals(""))
          editor.setText(description + " [" + name + "]");
        else
          editor.setText(description);
      }
      else 
      {
        String description = ((FtsObject)value).getDescription();
        
        if(description.charAt(0) != '{')
          editor.setText(description + " [#" + ((FtsObject)value).getID() + "]");
        else
          editor.setText(description);
      }
      
      return editor;
    }
  }
  
  public class FtsObjectCellRenderer extends JLabel implements TableCellRenderer 
  {  
    public FtsObjectCellRenderer() 
    {
      setOpaque( true); 
      setBorder( BorderFactory.createEtchedBorder());
      setBackground( JMaxMatTable.rowsIdColor);
      setHorizontalTextPosition( SwingConstants.CENTER);
    }
    
    public Component getTableCellRendererComponent(JTable table, Object value, boolean isSelected, boolean hasFocus,int row, int column) 
    {      
      if(value instanceof FtsGraphicObject)
      {
        String name = ((FtsGraphicObject)value).getVariableName();
        String description = ((FtsGraphicObject)value).getDescription();
      
        if(name != null && !name.equals(""))
          setText(description + " [" + name + "]");
        else
          setText(description);
      }
      else 
      {
        String description = ((FtsObject)value).getDescription();
        
        if(description.charAt(0) != '{')
          setText(description + " [#" + ((FtsObject)value).getID() + "]");
        else
          setText(description);
      }
      
      setHorizontalTextPosition( SwingConstants.CENTER);
      return this;
    }
  }  
}















