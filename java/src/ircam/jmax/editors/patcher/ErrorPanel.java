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
import javax.swing.event.*;
import javax.swing.tree.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.mda.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.widgets.*;

// ^^^^ Same thing as for the control panel; the find panel itself is
// ^^^^ ready to be used with a specific server, but it is not clear
// ^^^^ how to make the binding; static, dynamic, in parallel on all the server ?
// ^^^^ The is a big user environment question to be solved before the technical one.

public class ErrorPanel extends JFrame implements FtsActionListener
{
    Fts fts;
    boolean atomic = false;
  
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
  
    static void registerErrorPanel()
    {
	MaxWindowManager.getWindowManager().addToolFinder( new MaxToolFinder() {
		public String getToolName() { return "Error Panel";}
		public void open() { ErrorPanel.open(MaxApplication.getFts());}
	    });
    }

    public static ErrorPanel open(Fts fts)
    {
	if (errorPanel == null)
	    errorPanel = new ErrorPanel(fts);
	
	errorPanel.setVisible(true);
	errorPanel.findErrors();
	
	return errorPanel;
    }

  public static ErrorPanel getInstance(Fts fts)
  {
    if (errorPanel == null)
      errorPanel = new ErrorPanel(fts);

    return errorPanel;
  }

  protected ErrorPanel(Fts f)
  {
    super( "Error Panel");

    this.fts = f;
    
    try
	{
	    set  = (FtsObjectSet) fts.makeFtsObject(fts.getRootObject(), "__objectset");
	}
    catch (FtsException e)
	{
	    System.out.println("System error: cannot get objectSet object");
	}

    tableModel = new ErrorTableModel(set);
    errorTable = new ErrorTablePanel(tableModel);

    errorTable.setObjectSelectedListener(new ObjectSelectedListener() {
	    public void objectSelected(FtsObject object)
	    {
		ErrorPanel.this.setCursor( Cursor.getPredefinedCursor( Cursor.WAIT_CURSOR));
		((FtsPatcherObject)object.getParent()).requestShowObject(object);
		((FtsPatcherObject)object.getParent()).requestStopWaiting(ErrorPanel.errorPanel);
	    }
	});

    emptyTreeModel = new DefaultTreeModel(new DefaultMutableTreeNode(""));

    errorTable.setSelectionListener(new ListSelectionListener() {
	public void valueChanged(ListSelectionEvent e) {
	    
	    if (e.getValueIsAdjusting()) return;
	    
	    ListSelectionModel lsm = (ListSelectionModel)e.getSource();
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
		    for(Enumeration enum =((FtsObject)tableModel.getObjectSet().getElementAt(selRow)).getGenealogy(); 
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
				    node = new FtsMutableTreeNode(obj, obj.getDescription());
				    top.add(node);
				    top = node;
				}
			}
		    DefaultTreeModel treeModel = new DefaultTreeModel(start);
		    ((DefaultTreeCellRenderer)tree.getCellRenderer()).setLeafIcon(ErrorTablePanel.errorIcon);
		    tree.setModel(treeModel);
		    
		    if(node!=null)
			tree.setSelectionPath(new TreePath(node.getPath()));
		}
	}
	});

    f.addEditListener(new FtsEditListener(){	    
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

    //////////////////////////////
    tree = new JTree(emptyTreeModel);
    tree.getSelectionModel().setSelectionMode(TreeSelectionModel.SINGLE_TREE_SELECTION);
    tree.setExpandsSelectedPaths(true);
    tree.setToggleClickCount(10);
    
    DefaultTreeCellRenderer renderer = new DefaultTreeCellRenderer();
    renderer.setLeafIcon(null);
    renderer.setOpenIcon(ErrorTablePanel.patcherIcon);    
    renderer.setClosedIcon(ErrorTablePanel.patcherIcon);    
    tree.setCellRenderer(renderer);

    //Listen for when the selection changes.
    tree.addTreeSelectionListener(new TreeSelectionListener() {
            public void valueChanged(TreeSelectionEvent e) {
                DefaultMutableTreeNode node = (DefaultMutableTreeNode) tree.getLastSelectedPathComponent();
                if (node == null) return;
	    }
        });
    
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

			ErrorPanel.this.setCursor( Cursor.getPredefinedCursor( Cursor.WAIT_CURSOR));

			if(obj instanceof FtsPatcherObject)
			    {
				obj.sendMessage(FtsObject.systemInlet, "open_editor");
				((FtsPatcherObject)obj).requestStopWaiting(ErrorPanel.errorPanel);
			    }
			else
			    {
				((FtsPatcherObject)obj.getParent()).requestShowObject(obj);
				((FtsPatcherObject)obj.getParent()).requestStopWaiting(ErrorPanel.errorPanel);
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
    splitPane.setTopComponent(errorTable);
    splitPane.setBottomComponent(treeView);
    splitPane.setDividerLocation(200); 
    splitPane.setPreferredSize(new Dimension(400, 300));

    getContentPane().add(splitPane);
    
    pack();
    validate();
  }

  public void findErrors()
  {
    Cursor temp = getCursor();

    setCursor( Cursor.getPredefinedCursor( Cursor.WAIT_CURSOR));
    fts.getErrorFinder().findErrors(fts.getRootObject(), set);
    setCursor(temp);
  }

  public void ftsActionDone()
  {
      setCursor(Cursor.getDefaultCursor());
  }
  private static ErrorPanel errorPanel = null;

  private ErrorTableModel tableModel;
  private ErrorTablePanel errorTable;
  private FtsObjectSet set;
  private JTree tree;
  private DefaultTreeModel emptyTreeModel;
}



