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


public class FindPanel extends JFrame implements FtsActionListener
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
  
  static void registerFindPanel()
  {
    MaxWindowManager.getWindowManager().addToolFinder( new MaxToolFinder() {
      public String getToolName() { return "Find Panel";}
      public void open() { FindPanel.open(MaxApplication.getFts());}
    });
  }

  public static FindPanel open(Fts fts)
  {
    if (findPanel == null)
      findPanel = new FindPanel(fts);

    findPanel.setVisible(true);

    return findPanel;
  }

  public static FindPanel getInstance(Fts fts)
  {
    if (findPanel == null)
      findPanel = new FindPanel(fts);

    return findPanel;
  }

  protected FindPanel(Fts f)
  {
    super( "Find Panel");

    this.fts = f;

    try
	{
	    set  = (FtsObjectSet) fts.makeFtsObject(fts.getRootObject(), "__objectset");
	}
    catch (FtsException e)
	{
	    System.out.println("System error: cannot get objectSet object");
	}
    
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

    objectSetViewer = new ObjectSetViewer();
    objectSetViewer.setModel( set);

    objectSetViewer.setObjectSelectedListener(new ObjectSelectedListener() {
	public void objectSelected(FtsObject object)
	{
	  FindPanel.this.setCursor( Cursor.getPredefinedCursor( Cursor.WAIT_CURSOR));
	  ((FtsPatcherObject)object.getParent()).requestShowObject(object);
	  ((FtsPatcherObject)object.getParent()).requestStopWaiting(FindPanel.findPanel);
	}
      });

    JPanel findPanel = new JPanel();
    findPanel.setLayout( new BorderLayout());
    findPanel.setBorder( new EmptyBorder( 5, 5, 5, 5) );
    findPanel.setAlignmentX( LEFT_ALIGNMENT);

    ////////////////////////////////////////////////////////////////////////
    objectSetViewer.setSelectionListener(new ListSelectionListener() {
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
		   for(Enumeration enum =((FtsObject)set.getElementAt(selRow)).getGenealogy(); 
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
    set.addListDataListener(new ListDataListener(){
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
    tree.setExpandsSelectedPaths(true);
    tree.setToggleClickCount(10);
    
    DefaultTreeCellRenderer renderer = new DefaultTreeCellRenderer();
    renderer.setOpenIcon(ErrorTablePanel.patcherIcon);    
    renderer.setClosedIcon(ErrorTablePanel.patcherIcon);    
    renderer.setLeafIcon(null);
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

			FindPanel.this.setCursor( Cursor.getPredefinedCursor( Cursor.WAIT_CURSOR));

			if(obj instanceof FtsPatcherObject)
			    {
				obj.sendMessage(FtsObject.systemInlet, "open_editor");
				((FtsPatcherObject)obj).requestStopWaiting(FindPanel.findPanel);
			    }
			else
			    {
				((FtsPatcherObject)obj.getParent()).requestShowObject(obj);
				((FtsPatcherObject)obj.getParent()).requestStopWaiting(FindPanel.findPanel);
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
    splitPane.setTopComponent(objectSetViewer);
    splitPane.setBottomComponent(treeView);
    splitPane.setDividerLocation(100); 
    splitPane.setPreferredSize(new Dimension(400, 200));

    ///////////////////////////////////////////////////////////////////////

    findPanel.add( "North", labelPanel);
    findPanel.add( "Center", splitPane);

    getContentPane().add( findPanel);

    pack();
    validate();
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
	fts.getFinder().find(fts.getRootObject(), set, args);

    setCursor(temp);
  }

  public void findFriends(FtsObject object)
  {
    if(object.isError()) return;

    Cursor temp = getCursor();
    setCursor( Cursor.getPredefinedCursor( Cursor.WAIT_CURSOR));
    textField.setText("");

    fts.getFinder().findFriends(object, set);    

    setCursor(temp);
  }

  public void ftsActionDone()
  {
      setCursor(Cursor.getDefaultCursor());
  }
  private static FindPanel findPanel = null;

  private ObjectSetViewer objectSetViewer;
  private JTextField textField;
  private FtsObjectSet set;
  private JTree tree;
  private DefaultTreeModel emptyTreeModel; 
}

