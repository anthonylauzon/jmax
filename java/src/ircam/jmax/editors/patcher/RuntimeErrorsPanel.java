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

import java.util.*;
import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.border.*;
import javax.swing.tree.*;
import javax.swing.event.*;


import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.mda.*;
import ircam.jmax.widgets.*;

// ^^^^ Same thing as for the control panel; the find panel itself is
// ^^^^ ready to be used with a specific server, but it is not clear
// ^^^^ how to make the binding; static, dynamic, in parallel on all the server ?
// ^^^^ The is a big user environment question to be solved before the technical one.


public class RuntimeErrorsPanel extends JFrame implements FtsActionListener
{
  Fts fts;

  class FtsMutableTreeNode extends DefaultMutableTreeNode
  { 
      FtsObject ftsObj;
      public FtsMutableTreeNode(FtsObject obj, String name)
      {
	  super(name);
	  ftsObj = obj;
      }
      public FtsObject getFtsObject()
      {
	  return ftsObj;
      }
  }
  
  static void registerRuntimeErrorsPanel()
  {
    MaxWindowManager.getWindowManager().addToolFinder( new MaxToolFinder() {
      public String getToolName() { return "Runtime Errors Panel";}
      public void open() { RuntimeErrorsPanel.open(MaxApplication.getFts());}
    });
  }

  public static RuntimeErrorsPanel open(Fts fts)
  {
    if (runtimeErrorsPanel == null)
      runtimeErrorsPanel = new RuntimeErrorsPanel(fts);

    runtimeErrorsPanel.setVisible(true);

    return runtimeErrorsPanel;
  }

  public static RuntimeErrorsPanel getInstance(Fts fts)
  {
    if (runtimeErrorsPanel == null)
      runtimeErrorsPanel = new RuntimeErrorsPanel(fts);

    return runtimeErrorsPanel;
  }

  protected RuntimeErrorsPanel(Fts f)
  {
    super( "Runtime Errors Panel");

    this.fts = f;

    try
	{
	    runtimeErrorsSource  = (FtsRuntimeErrors) fts.makeFtsObject(fts.getRootObject(), "__runtimeerrors");
	}
    catch (FtsException e)
	{
	    System.out.println("System error: cannot get runtimeErrors object");
	}
    
    jList = new JList(runtimeErrorsSource);
    jList.setCellRenderer( new DefaultListCellRenderer()
	{
	    public Component getListCellRendererComponent( JList jlist, Object obj, int i, boolean selected, boolean hasFocus)
	    {
		super.getListCellRendererComponent(jlist, obj, i, selected, hasFocus);

		if (obj != null)
		    {
			setText(((RuntimeError)obj).getDescription()+" "+((RuntimeError)obj).getCount());	   
			setIcon(ObjectSetViewer.getObjectIcon(((RuntimeError)obj).getObject()));
		    }
      
		return this;
	    } 
	});

    JScrollPane listScrollPane = new JScrollPane();
    listScrollPane.getViewport().setView( jList);
    
    jList.addMouseListener(new MouseListener(){
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
			    FtsObject object = ((RuntimeError)runtimeErrorsSource.getElementAt(index)).getObject();
			    RuntimeErrorsPanel.this.setCursor( Cursor.getPredefinedCursor( Cursor.WAIT_CURSOR));
			    ((FtsPatcherObject)object.getParent()).requestShowObject(object);
			    ((FtsPatcherObject)object.getParent()).requestStopWaiting(RuntimeErrorsPanel.runtimeErrorsPanel);
			}
		    }
	    }
	});

    jList.addListSelectionListener(new ListSelectionListener() {
	    public void valueChanged(ListSelectionEvent e) {

		if (e.getValueIsAdjusting()) return;
		
		JList lsm = (JList)e.getSource();
		if (lsm.isSelectionEmpty()) 
		    {
			((DefaultTreeCellRenderer)tree.getCellRenderer()).setLeafIcon(null);
			tree.setModel(emptyTreeModel);
		    } 
		else 
		    {
			int selRow = lsm.getMinSelectionIndex();
			DefaultMutableTreeNode top, node, start;
			top = start = node = null;
			FtsObject obj;
			for(Enumeration enum =((RuntimeError)runtimeErrorsSource.getElementAt(selRow)).getObject().getGenealogy();
 			    enum.hasMoreElements(); )
			    {
				obj = (FtsObject)enum.nextElement();
				if(top==null)
				    {
					top = new FtsMutableTreeNode(obj, (obj.getDocument()!=null) ? 
								     obj.getDocument().getName() : obj.getDescription());
					start = top;
				    }				
				else
				    {
					node = new FtsMutableTreeNode(obj, (!obj.getDescription().equals("")) ? 
								      obj.getDescription() : obj.getComment());
					top.add(node);
					top = node;
				    }
			    }
			((DefaultTreeCellRenderer)tree.getCellRenderer()).
			    setLeafIcon(ObjectSetViewer.getObjectIcon(((FtsMutableTreeNode)node).getFtsObject()));
      
			DefaultTreeModel treeModel = new DefaultTreeModel(start);
			tree.setModel(treeModel);
			
			if(node!=null)
			    tree.setSelectionPath(new TreePath(node.getPath()));
		    }
	    }
	});
    
    runtimeErrorsSource.addListDataListener(new ListDataListener(){
	    public void contentsChanged(ListDataEvent e)
	    {
		((DefaultTreeCellRenderer)tree.getCellRenderer()).setLeafIcon(null);
		tree.setModel(emptyTreeModel);
	    }
	    public void intervalRemoved(ListDataEvent e){}
	    public void intervalAdded(ListDataEvent e){}
	});
	
    emptyTreeModel = new DefaultTreeModel(new DefaultMutableTreeNode());
      
    tree = new JTree(emptyTreeModel);
    tree.getSelectionModel().setSelectionMode(TreeSelectionModel.SINGLE_TREE_SELECTION);
    ((DefaultTreeCellRenderer)tree.getCellRenderer()).setOpenIcon(ErrorTablePanel.patcherIcon);
    ((DefaultTreeCellRenderer)tree.getCellRenderer()).setClosedIcon(ErrorTablePanel.patcherIcon);
    ((DefaultTreeCellRenderer)tree.getCellRenderer()).setLeafIcon(null);
    tree.setExpandsSelectedPaths(true);
    tree.setToggleClickCount(10);
      
    tree.addMouseListener(new MouseListener(){
	    public void mouseEntered(MouseEvent e) {} 
	    public void mouseExited(MouseEvent e) {}
	    public void mousePressed(MouseEvent e) {}
	    public void mouseReleased(MouseEvent e) {}
	    public void mouseClicked(MouseEvent e)
	    {
		if (e.getClickCount() == 2)
		    {
			TreePath path = tree.getPathForLocation(e.getX(), e.getY());
			FtsObject obj = (FtsObject)((FtsMutableTreeNode)path.getLastPathComponent()).getFtsObject();
      
			RuntimeErrorsPanel.this.setCursor( Cursor.getPredefinedCursor( Cursor.WAIT_CURSOR));
      
			if(obj instanceof FtsPatcherObject)
			    {
				obj.sendMessage(FtsObject.systemInlet, "open_editor");
				((FtsPatcherObject)obj).requestStopWaiting(RuntimeErrorsPanel.runtimeErrorsPanel);
			    }
			else
			    {
				((FtsPatcherObject)obj.getParent()).requestShowObject(obj);
				((FtsPatcherObject)obj.getParent()).requestStopWaiting(RuntimeErrorsPanel.runtimeErrorsPanel);
			    }
		    }
	    }
	});
      
    //Create the scroll pane and add the tree to it. 
    JScrollPane treeView = new JScrollPane(tree);
    treeView.setPreferredSize(new Dimension(400, 100));
    treeView.setMinimumSize(new Dimension(100, 50));
    /////////////////////////////

    //Add the scroll panes to a split pane.
    JSplitPane splitPane = new JSplitPane(JSplitPane.VERTICAL_SPLIT);
    splitPane.setTopComponent(listScrollPane);
    splitPane.setBottomComponent(treeView);
    splitPane.setDividerLocation(100); 
    splitPane.setPreferredSize(new Dimension(400, 200));

    getContentPane().add( splitPane);

    pack();
    validate();
  }

  public void ftsActionDone()
  {
      setCursor(Cursor.getDefaultCursor());
  }
  private static RuntimeErrorsPanel runtimeErrorsPanel = null;
  private FtsRuntimeErrors runtimeErrorsSource;

  private JList jList;
  private JTree tree;
  private DefaultTreeModel emptyTreeModel;
}

