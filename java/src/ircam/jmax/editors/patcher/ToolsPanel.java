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

public class ToolsPanel extends JFrame implements FtsActionListener
{
    Fts fts;
    public final static ImageIcon objectIcon = SystemIcons.get("_object_");
    public final static ImageIcon inletIcon = SystemIcons.get("_inlet_");
    public final static ImageIcon outletIcon =  SystemIcons.get("_outlet_");
  
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
  
    static void registerToolsPanel()
    {
	MaxWindowManager.getWindowManager().addToolFinder( new MaxToolFinder() {
		public String getToolName() { return "Tools Panel";}
		public void open() { ToolsPanel.open(MaxApplication.getFts());}
	    });
    }

    public static ToolsPanel open(Fts fts)
    {
	if (toolsPanel == null)
	    toolsPanel = new ToolsPanel(fts);
	
	toolsPanel.setVisible(true);
	
	return toolsPanel;
    }

  public static ToolsPanel getInstance(Fts fts)
  {
    if (toolsPanel == null)
      toolsPanel = new ToolsPanel(fts);

    return toolsPanel;
  }

  protected ToolsPanel(Fts f)
  {
    super( "Tools Panel");

    this.fts = f;

    /* #############    listeners    ######################################## */
    objSelListener = new ObjectSelectedListener(){
	    public void objectSelected(FtsObject object)
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
    errorTable = new ErrorTablePanel(fts);
    errorTable.setObjectSelectedListener(objSelListener);
    errorTable.setSelectionListener(listSelListener);

    /* RuntimeErrors Panel */
    runErrorTable = new RuntimeErrorsTablePanel(fts);
    runErrorTable.setObjectSelectedListener(objSelListener);
    runErrorTable.setSelectionListener(listSelListener);

    currentTableModel = errorTable.getToolTableModel();

    tabbedPane = new JTabbedPane();
    tabbedPane.addTab("Errors", errorTable);
    tabbedPane.setSelectedIndex(0);
    tabbedPane.addTab("Runtime Errors", runErrorTable);
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
			FtsObject obj = (FtsObject)((FtsMutableTreeNode)path.getLastPathComponent()).getFtsObject();

			ToolsPanel.this.setCursor( Cursor.getPredefinedCursor( Cursor.WAIT_CURSOR));

			if(obj instanceof FtsPatcherObject)
			    {
				obj.sendMessage(FtsObject.systemInlet, "open_editor");
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
	      FtsObject ftsObj;			
	      ////////????????????????? comment eviter ca?????????
	      if(currentTableModel instanceof RuntimeErrorsTableModel)
		  ftsObj = ((RuntimeError)currentTableModel.getListModel().getElementAt(selRow)).getObject();
	      else
		  ftsObj = (FtsObject)currentTableModel.getListModel().getElementAt(selRow);
	      
	      for(Enumeration enum = ftsObj.getGenealogy(); enum.hasMoreElements(); )
		  {
		      ftsObj = (FtsObject)enum.nextElement();
		      if(top==null)
			  {
			      top = new FtsMutableTreeNode(ftsObj, (ftsObj.getDocument()!=null) ? 
							   ftsObj.getDocument().getName() : ftsObj.getDescription());
			      start = top;
			  }				
		      else
			  {
			      node = new FtsMutableTreeNode(ftsObj, ftsObj.getDescription());
			      top.add(node);
			      top = node;
			  }
		  }
	      DefaultTreeModel treeModel = new DefaultTreeModel(start);
	      ((DefaultTreeCellRenderer)tree.getCellRenderer()).
		  setLeafIcon(ToolsPanel.getObjectIcon(((FtsMutableTreeNode)node).getFtsObject()));
	      
	      tree.setModel(treeModel);
	      
	      if(node!=null)
		  tree.setSelectionPath(new TreePath(node.getPath()));
	  }
  }

  /*////////////////////// Object Icons //////////////////////////*/
  /* Move this in SystemIcons !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
  public static ImageIcon getObjectIcon(FtsObject obj)
  {
      ImageIcon icon;
      String className = ((FtsObject)obj).getClassName();

      if(obj.isError())
	   icon = ErrorTablePanel.errorIcon;
      else if (obj instanceof FtsTemplateObject)
	  icon = objectIcon;
      else if (obj instanceof FtsPatcherObject)
	  icon = ErrorTablePanel.patcherIcon;
      else if (obj instanceof FtsInletObject)
	  icon = inletIcon;
      else if (obj instanceof FtsOutletObject)
	  icon = outletIcon;
      else if(ObjectCreatorManager.containsClass(className))
	  if(SystemIcons.get(className)!=null)
	      icon = SystemIcons.get(className);
	  else
	      icon = objectIcon;
      else
	  icon = objectIcon;
      return icon;
  }  

  /*///////////////////////////////////////////////////////////////////*/

  private static ToolsPanel toolsPanel = null;
  private ToolTableModel currentTableModel;
  
  private JTabbedPane tabbedPane;
  private ErrorTablePanel errorTable;
  private RuntimeErrorsTablePanel runErrorTable;

  /* JTree variables */
  private JTree tree;
  private DefaultTreeModel emptyTreeModel;

  /* listeners */
  private ListSelectionListener listSelListener;
  private ObjectSelectedListener objSelListener;
}



