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
import ircam.fts.client.*;
import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.widgets.*;

// ^^^^ Same thing as for the control panel; the find panel itself is
// ^^^^ ready to be used with a specific server, but it is not clear
// ^^^^ how to make the binding; static, dynamic, in parallel on all the server ?
// ^^^^ The is a big user environment question to be solved before the technical one.

public class ToolsPanel extends JFrame implements FtsActionListener
{
    class FtsMutableTreeNode extends DefaultMutableTreeNode
    { 
	FtsGraphicObject ftsObj;
	public FtsMutableTreeNode(FtsGraphicObject obj, String name)
	{
	    super(name);
	    ftsObj = obj;
	}
	public FtsGraphicObject getFtsObject()
	{
	    return ftsObj;
	}
    }
  
    static void registerToolsPanel()
    {
	MaxWindowManager.getWindowManager().addToolFinder( new MaxToolFinder() {
		public String getToolName() { return "Tools Panel";}
		public void open() { ToolsPanel.open();}
	    });
    }

    public static ToolsPanel open()
    {
	if (toolsPanel == null)
	    toolsPanel = new ToolsPanel();
	
	toolsPanel.setVisible(true);
	
	return toolsPanel;
    }

    public static ToolsPanel find(FtsGraphicObject obj)
    {
	if (toolsPanel == null)
	    toolsPanel = new ToolsPanel();
	
	toolsPanel.tabbedPane.setSelectedComponent(toolsPanel.getFinder());
	toolsPanel.setVisible(true);
	if(obj!=null) toolsPanel.getFinder().findFriends(obj);
	else toolsPanel.getFinder().find();
	
	return toolsPanel;
    }

  public static ToolsPanel getInstance()
  {
    if (toolsPanel == null)
      toolsPanel = new ToolsPanel();

    return toolsPanel;
  }

  protected ToolsPanel()
  {
    super( "Tools Panel");

    /* #############    listeners    ######################################## */
    objSelListener = new ObjectSelectedListener(){
	    public void objectSelected(FtsGraphicObject object)
	    {
		ToolsPanel.this.setCursor( Cursor.getPredefinedCursor( Cursor.WAIT_CURSOR));
		((FtsPatcherObject)object.getParent()).requestShowObject(object);
		((FtsPatcherObject)object.getParent()).requestStopWaiting(ToolsPanel.toolsPanel);
	    }
	};

    listSelListener = new ListSelectionListener(){
	    public void valueChanged(ListSelectionEvent e) {
	    
		if (e.getValueIsAdjusting()) return;	    

		createTreeModelFromSelection((ListSelectionModel)e.getSource());
	    }
	};
    /* ################################################################ */

    /* Error Panel */
    errorTable = new ErrorTablePanel();
    errorTable.setObjectSelectedListener(objSelListener);
    errorTable.setSelectionListener(listSelListener);

    /* RuntimeErrors Panel */
    runErrorTable = new RuntimeErrorsTablePanel();
    runErrorTable.setObjectSelectedListener(objSelListener);
    runErrorTable.setSelectionListener(listSelListener);

    /* Finder Panel */
    finderTable = new FinderTablePanel();
    finderTable.setObjectSelectedListener(objSelListener);
    finderTable.setSelectionListener(listSelListener);

    currentTableModel = errorTable.getToolTableModel();

    tabbedPane = new JTabbedPane();
    tabbedPane.addTab("Errors", errorTable);
    tabbedPane.addTab("Runtime Errors", runErrorTable);
    tabbedPane.addTab("Finder", finderTable);    

    tabbedPane.setSelectedIndex(0);
    tabbedPane.addChangeListener(new ChangeListener(){
	    public void stateChanged(ChangeEvent e)
	    {		
		//set current table model
		currentTableModel = ((JMaxToolPanel)tabbedPane.getSelectedComponent()).getToolTableModel();	    
		//updates JTree model
		createTreeModelFromSelection(((JMaxToolPanel)tabbedPane.getSelectedComponent()).getListSelectionModel());
	    }
	});

    ////////////////////////////////////////////////////////////////

    /* JTREE ####################################### */
    emptyTreeModel = new DefaultTreeModel(new DefaultMutableTreeNode(""));

    tree = new JTree(emptyTreeModel);
    tree.getSelectionModel().setSelectionMode(TreeSelectionModel.SINGLE_TREE_SELECTION);
    tree.setExpandsSelectedPaths(true);
    tree.setToggleClickCount(10);
    
    DefaultTreeCellRenderer renderer = new DefaultTreeCellRenderer();
    renderer.setLeafIcon(null);
    renderer.setOpenIcon(ErrorTablePanel.patcherIcon);    
    renderer.setClosedIcon(ErrorTablePanel.patcherIcon);    
    tree.setCellRenderer(renderer);

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
	      FtsGraphicObject obj = ((FtsMutableTreeNode)path.getLastPathComponent()).getFtsObject();
	      
	      ToolsPanel.this.setCursor( Cursor.getPredefinedCursor( Cursor.WAIT_CURSOR));
	      
	      if(obj instanceof FtsPatcherObject)
		{
		  ((FtsPatcherObject)obj).requestOpenEditor();
		  ((FtsPatcherObject)obj).requestStopWaiting(ToolsPanel.toolsPanel);
		}
	      else
		{
		  ((FtsPatcherObject)obj.getParent()).requestShowObject(obj);
		  ((FtsPatcherObject)obj.getParent()).requestStopWaiting(ToolsPanel.toolsPanel);
		}
	    }
	}
      });

    //Create the scroll pane and add the tree to it. 
    JScrollPane treeView = new JScrollPane(tree);
    treeView.setPreferredSize(new Dimension(400, 100));
    treeView.setMinimumSize(new Dimension(100, 50));
    /////////////////////////////
    
    JSplitPane splitPane = new JSplitPane(JSplitPane.VERTICAL_SPLIT);
    splitPane.setTopComponent(tabbedPane);
    splitPane.setBottomComponent(treeView);
    splitPane.setDividerLocation(200); 
    splitPane.setPreferredSize(new Dimension(400, 300));

    getContentPane().add(splitPane);
    
    pack();
    validate();
  }

  public void ftsActionDone()
  {
      setCursor(Cursor.getDefaultCursor());
  }

  private void createTreeModelFromSelection(ListSelectionModel lsm)
  {
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
	FtsGraphicObject ftsObj;			
	String nodeText;
	////////????????????????? comment eviter ca?????????
	if(currentTableModel instanceof RuntimeErrorsTableModel)
	  ftsObj = ((RuntimeError)currentTableModel.getListModel().getElementAt(selRow)).getObject();
	else
	  ftsObj = (FtsGraphicObject)currentTableModel.getListModel().getElementAt(selRow);
	
	for(Enumeration enum = ftsObj.getGenealogy(); enum.hasMoreElements(); )
	  {
	    ftsObj = (FtsGraphicObject)enum.nextElement();
	    if(top==null)
	      {
		top = new FtsMutableTreeNode(ftsObj, (((FtsPatcherObject)ftsObj).getName()!=null) ? 
						      ((FtsPatcherObject)ftsObj).getName() : ftsObj.getDescription());
		//top = new FtsMutableTreeNode(ftsObj, ftsObj.getDescription());  
		start = top;
	      }				
	    else
	      {
		nodeText = ftsObj.getDescription();
		if(nodeText.equals("")) nodeText = ftsObj.getComment();
		node = new FtsMutableTreeNode(ftsObj, nodeText);
		top.add(node);
		top = node;
	      }
	  }
	DefaultTreeModel treeModel = new DefaultTreeModel(start);
	((DefaultTreeCellRenderer)tree.getCellRenderer()).
	  setLeafIcon(ObjectSetViewer.getObjectIcon(((FtsMutableTreeNode)node).getFtsObject()));
	
	tree.setModel(treeModel);
	
	if(node!=null)
	  tree.setSelectionPath(new TreePath(node.getPath()));
      }
  }

  /*///////////////////////////////////////////////////////////////////*/

  public FinderTablePanel getFinder()
  {
      return finderTable;
  }

  private static ToolsPanel toolsPanel = null;
  private ToolTableModel currentTableModel;
  
  /* JTabbedPane */
  private JTabbedPane tabbedPane;
  private ErrorTablePanel errorTable;
  private RuntimeErrorsTablePanel runErrorTable;
  private FinderTablePanel finderTable;

  /* JTree  */
  private JTree tree;
  private DefaultTreeModel emptyTreeModel;

  /* listeners */
  private ListSelectionListener listSelListener;
  private ObjectSelectedListener objSelListener;
}



