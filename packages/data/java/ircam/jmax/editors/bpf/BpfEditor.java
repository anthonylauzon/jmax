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


package ircam.jmax.editors.bpf;

import ircam.jmax.editors.bpf.renderers.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.*;
import java.awt.*;
import java.awt.event.*;
import java.beans.*;
import java.util.*;
import javax.swing.*;
import javax.swing.border.*;
import javax.swing.event.*;
import java.beans.*;
/**
 * A Monodimensional view for a generic Sequence. 
 * This kind of editor use a MonoDimensionalAdapter
 * to map the y values. The value returned is always half of the panel,
 * and settings of y are simply ignored. */
public class BpfEditor extends PopupToolbarPanel implements ListSelectionListener
{
  public BpfEditor(Geometry g, FtsBpfObject model, BpfToolManager manager)
  {
    super();
    
    this.geometry = g;
    this.model = model;
    
    /******** display labels **********/
    JPanel labelPanel = new JPanel();
    labelPanel.setLayout( new BoxLayout( labelPanel, BoxLayout.X_AXIS));
    labelPanel.setOpaque(false);
	
    displayMouseLabel = new JLabel();
    displayMouseLabel.setFont(BpfPanel.rulerFont);
    displayMouseLabel.setPreferredSize(new Dimension(102, 15));
    displayMouseLabel.setMaximumSize(new Dimension(102, 15));
    displayMouseLabel.setMinimumSize(new Dimension(102, 15));
    
    infoLabel = new JLabel("", JLabel.RIGHT);
    infoLabel.setFont(BpfPanel.rulerFont);
    infoLabel.setPreferredSize(new Dimension(190, 15));
    infoLabel.setMaximumSize(new Dimension(190, 15));
    infoLabel.setMinimumSize(new Dimension(190, 15));
    
    labelPanel.add(displayMouseLabel);
    labelPanel.add(Box.createHorizontalGlue());
    labelPanel.add(infoLabel);
    
    setLayout( new BoxLayout( this, BoxLayout.Y_AXIS));
    add(labelPanel);
    add(Box.createVerticalGlue());
    /*********************************/
	
    model.addBpfListener(new BpfDataListener() {
	public void pointsDeleted(int index, int size){BpfEditor.this.repaint();}
	public void pointAdded(int index) {
	  BpfEditor.this.repaint();			
	}
	public void pointChanged(int oldndex, int newIndex, float newTime, float newValue) {
	  BpfEditor.this.repaint();
	}
	public void pointsChanged() {
	  BpfEditor.this.repaint();
	}
	public void cleared(){BpfEditor.this.repaint();}
      });
    
    geometry.addTranspositionListener(new TranspositionListener() {
	public void transpositionChanged(int newTranspose)
	{
	  repaint();
	}
      });

    createGraphicContext(geometry, model, manager);
    
    gc.getSelection().addListSelectionListener(this);
    
    setBackground(Color.white);
    
    setOpaque(false);
    
    bpfPopupMenu = new BpfPopupMenu(this);
    
    manager.addToolListener(new ToolListener() {
	public void toolChanged(ToolChangeEvent e) 
	{		    
	  if (e.getTool() != null) 
	    setCursor(e.getTool().getCursor());
	}
      });

    addMouseListener(new MouseListener(){
	public void mouseClicked(MouseEvent e){}
	public void mousePressed(MouseEvent e){}
	public void mouseReleased(MouseEvent e){}
	public void mouseEntered(MouseEvent e)
	{
	  requestFocus();
	}
	public void mouseExited(MouseEvent e){
	  gc.display("");
	}
      });
    addMouseMotionListener(new MouseMotionListener(){
	public void mouseMoved(MouseEvent e)
	{
	  /* workaround: right-mouse events used when popup is visible */
	  if(getMenu().isVisible()) return;
	  
	  if(gc.getToolManager().getCurrentTool().getName().equals("edit tool"))
	    {
	      float time = gc.getAdapter().getInvX(e.getX());
	      float maxTime = gc.getMaximumTime();
	      if(time < 0) time = 0;
	      else if(time > maxTime) time = maxTime;
	      
	      float val =  gc.getAdapter().getInvY(e.getY());
	      if(val > gc.getFtsObject().getMaximumValue())
		val = gc.getFtsObject().getMaximumValue();
	      else 
		if(val < gc.getFtsObject().getMinimumValue())
		  val = gc.getFtsObject().getMinimumValue();
	      
	      gc.display("( "+PointRenderer.numberFormat.format(time)+" , "+
			 PointRenderer.numberFormat.format(val)+" )");	
	    }
	}
	public void mouseDragged(MouseEvent e)
	{
	  /* workaround: right-mouse events used when popup is visible */
	  if(getMenu().isVisible()) return;
	  
	  String toolName = gc.getToolManager().getCurrentTool().getName();
	  if(toolName.equals("edit tool"))
	    {
	      float time = gc.getAdapter().getInvX(e.getX());
	      float maxTime = gc.getMaximumTime();
	      if(time < 0) time = 0;
	      else if(time > maxTime) time = maxTime;
	      
	      float val =  gc.getAdapter().getInvY(e.getY());
	      if(val > gc.getFtsObject().getMaximumValue())
		val = gc.getFtsObject().getMaximumValue();
	      else 
		if(val < gc.getFtsObject().getMinimumValue())
		  val = gc.getFtsObject().getMinimumValue();
	      
	      gc.display("( "+PointRenderer.numberFormat.format(time)+" , "+
			 PointRenderer.numberFormat.format(val)+" )");	
	    }
	  else
	    if(toolName.equals("zoom&scroll tool"))
	      {
		int start = gc.getLogicalTime();
		int end = start + gc.getTimeWindow();
		gc.display("[ "+start+" -- "+end+" ]");
	      }
	}
      });

    addKeyListener(new KeyListener(){
	public void keyTyped(KeyEvent e){}
	public void keyPressed(KeyEvent e)
	{
	  if(isDeleteKey(e))
	    {
	      BpfEditor.this.model.beginUpdate();
	      gc.getSelection().deleteAll();
	    }		
	  else if(isArrowKey(e))
	    {
	      consumeArrowKeyEvent(e);
	    }
	}
	public void keyReleased(KeyEvent e){}
      });
  }

  public void reinit(){}
  
  public JMenu getToolsMenu()
  {
    return gc.getToolManager().getMenu();
  }
  
  public JPopupMenu getMenu()
  {
    return bpfPopupMenu;
  }
  
  public void paintComponent(Graphics g) 
  {
    Rectangle r = g.getClipBounds();
    renderer.render(g, r);	
  }
  
  private void createGraphicContext(Geometry geometry, FtsBpfObject model, BpfToolManager manager)
  {
    gc = new BpfGraphicContext(model); //loopback?
    gc.setSelection( new BpfSelection(model, gc));
    gc.setGraphicSource(this);
    gc.setGraphicDestination(this);
    ad = new BpfAdapter(geometry, gc);
    gc.setAdapter(ad);
    
    renderer = new BpfRenderer(gc);
    gc.setRenderManager(renderer);
    gc.setToolManager(manager);
    
    gc.setDisplay(displayMouseLabel, infoLabel);
  }
  
  public void setAdapter(BpfAdapter adapter)
  {
    gc.setAdapter(adapter);	
    ad = adapter;
  }
  
  public void setRenderer(BpfRenderer renderer)
  {
    this.renderer = renderer;
    gc.setRenderManager(renderer);
  }
  
  /**
   * ListSelectionListener interface
   */
  
  public void valueChanged(ListSelectionEvent e)
  {
    repaint();
  }
  
  public void showListDialog()
  {
    if(listDialog==null) 
      createListDialog();
    else
      listDialog.relocate();
    listDialog.setVisible(true);
  }

  private void createListDialog()
  {
    listDialog = new BpfTableDialog(gc.getFrame(), gc);
  }

  public void updateNewObject(Object obj){};
  
  void updateEventProperties(Object whichObject, String propName, Object propValue){}
  
  void updateRange(Object whichObject){}    
  /**
   * Track editor interface */
  
  public Component getComponent()
  {
    return this;
  }
  
  public BpfGraphicContext getGraphicContext()
  {
    return gc;
  }
  
  public int getDefaultHeight()
  {
    return DEFAULT_HEIGHT;
  }
  
  public void dispose()
  {
    if(listDialog != null)
      listDialog.dispose();
  }
  
  public Dimension getPreferredSize()
  {
    return new Dimension(BpfWindow.DEFAULT_WIDTH, DEFAULT_HEIGHT);
  }

    /**
     * Displays (x, y) coord in top-left corner
     **/
    /*public void processMouseMotionEvent(MouseEvent e)
      {
      if(!getMenu().isVisible())
      {
      float time = gc.getAdapter().getInvX(e.getX());
      float val =  gc.getAdapter().getInvY(e.getY());
      gc.display("( "+PointRenderer.numberFormat.format(time)+" , "+
      PointRenderer.numberFormat.format(val)+" )");
      }
      super.processMouseMotionEvent(e);
      }*/

    public void processKeyEvent(KeyEvent e)
    {
      if((e.getKeyCode() == KeyEvent.VK_TAB)&&(e.getID()==KeyEvent.KEY_PRESSED))
	if(e.isControlDown())
	  gc.getSelection().selectPrevious();
	else
	  gc.getSelection().selectNext();
	  
      super.processKeyEvent(e);
      requestFocus();
    }

  void consumeArrowKeyEvent(KeyEvent e)
  {
    if(!e.isControlDown())
      {
	int dx = 0;
	int dy = 0;
	int DXY;
	if(e.isShiftDown())
	  DXY = 5;
	else
	  DXY = 1;
	switch(e.getKeyCode())
	  {
	  case KeyEvent.VK_UP:
	    dy  = -DXY;
	    break;
	  case KeyEvent.VK_DOWN:
	    dy  = DXY;
	    break;
	  case KeyEvent.VK_RIGHT:
	    dx  = DXY;
	    break;
	  case KeyEvent.VK_LEFT:
	    dx  = -DXY;
	  }
	
	gc.getSelection().moveSelection(dx, dy, gc);
      }
    else
      switch(e.getKeyCode())
	{
	case KeyEvent.VK_UP:
	  gc.getSelection().alignTop();
	  break;
	case KeyEvent.VK_DOWN:
	  gc.getSelection().alignBottom();
	  break;
	case KeyEvent.VK_RIGHT:
	  gc.getSelection().alignRight();
	  break;
	case KeyEvent.VK_LEFT:
	  gc.getSelection().alignLeft();
	}
  }
  
  public static boolean isDeleteKey(KeyEvent e)
  {
    return ((e.getKeyCode() == KeyEvent.VK_DELETE)||(e.getKeyCode() == KeyEvent.VK_BACK_SPACE));
  }
  public static boolean isArrowKey(KeyEvent e)
  {
    return ((e.getKeyCode() == KeyEvent.VK_UP)||(e.getKeyCode() == KeyEvent.VK_DOWN) ||(e.getKeyCode() == KeyEvent.VK_LEFT) || (e.getKeyCode() == KeyEvent.VK_RIGHT));
  }
  //--- BpfEditor fields
  Geometry geometry;
  BpfGraphicContext gc;
  FtsBpfObject model;
  BpfPopupMenu bpfPopupMenu;
  static int MONODIMENSIONAL_TRACK_OFFSET = 0;
  static public int DEFAULT_HEIGHT = BpfWindow.DEFAULT_HEIGHT - 30;
  BpfRenderer renderer;
  BpfAdapter ad;

  MaxVector oldElements = new MaxVector();
  BpfTableDialog listDialog = null;
  JLabel displayMouseLabel, infoLabel;
}





