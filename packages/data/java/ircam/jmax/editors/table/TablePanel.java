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

package ircam.jmax.editors.table;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.widgets.*;

import ircam.jmax.editors.table.tools.*;
import ircam.jmax.editors.table.renderers.*;

import java.awt.*;
import java.util.*;
import java.awt.event.*;
import java.io.*;
import javax.swing.*;
import javax.swing.border.*;
import javax.swing.event.*;
import javax.swing.undo.*;

/**
 * The panel in the Table editor's window, containing the toolbar, the CenterPanel 
 * (the graphic representation), the vertical scrollbar.
 * This class takes care of creating all the components of an editing session
 * (tools, Renderer, ...) and link them togheter.
 */
public class TablePanel extends JPanel implements TableDataListener, Editor{

  //--- Fields  
  final static int SCROLLBAR_SIZE = 30;
  final static int PANEL_WIDTH = 500;
  static int PANEL_HEIGHT;
  final static int IVEC_PANEL_HEIGHT = 270;
  final static int FVEC_PANEL_HEIGHT = 200;

  public JScrollBar itsVerticalControl, itsHorizontalControl;

  Dimension size;

  public EditorToolbar toolbar;
  TableGraphicContext gc;
  TableRenderer itsTableRenderer;
  TableDataModel tableData;
  TableDisplay itsCenterPanel;
  TableToolManager toolManager;
  FtsObjectWithEditor ftsObj;

  /**
   * Constructor. */
  public TablePanel(EditorContainer container, FtsObjectWithEditor ftsObj, TableDataModel tm) {
    super();

    this.ftsObj = ftsObj;
    tableData = tm;
    itsEditorContainer = container;

    PANEL_HEIGHT = (ftsObj instanceof FtsIvecObject) ? IVEC_PANEL_HEIGHT : FVEC_PANEL_HEIGHT;
    size = new Dimension(PANEL_WIDTH, PANEL_HEIGHT);

    setSize(PANEL_WIDTH, PANEL_HEIGHT);
    setLayout(new BorderLayout());

    prepareToolbarPanel();
    
    //... the center panel
    prepareCenterPanel();

    //... the Graphic context
    prepareGraphicContext();
    itsCenterPanel.setGraphicContext(gc);

    //... the renderer
    itsTableRenderer = new TableRenderer(gc);
    itsCenterPanel.setRenderer(itsTableRenderer);
    gc.setRenderManager(itsTableRenderer);
    
    //... the vertical position controller
    prepareVerticalScrollbar();    
    prepareHorizontalScrollbar();
   
    // Add listeners for data model changes
    tableData.addListener(this);

    // Add a listener for selection content changes
    gc.getSelection().addListSelectionListener( new ListSelectionListener() {
      public void valueChanged(ListSelectionEvent e)
	{
	  if (e.getValueIsAdjusting())
	    return;
	  itsCenterPanel.repaint();
	}
      });

    addComponentListener( new ComponentAdapter() {
	public void componentResized(ComponentEvent e)
	{
	  updateHorizontalScrollbar();
	  updateVerticalScrollbar();
	  gc.getFtsObject().requestSetVisibleWindow( gc.getVisibleHorizontalScope(), gc.getFirstVisibleIndex(), 
						    ((TableAdapter)gc.getAdapter()).getXZoom(), 
						    gc.getVisiblePixelsSize());
	  if(gc.getAdapter().getXZoom() > 0.5)		    
	    gc.getFtsObject().requestGetValues();
	  else
	    gc.getFtsObject().requestGetPixels(0, 0);
	}
      });
    //470 is the default size of the TableDisplay .......
    gc.getFtsObject().requestSetVisibleWindow( 470, 0, (double)1.0, gc.getVisiblePixelsSize());
    gc.getFtsObject().requestGetValues();
  }
  
  void frameAvailable()
  {
    gc.setFrame(GraphicContext.getFrame(this));
    toolManager.addContextSwitcher(new WindowContextSwitcher(gc.getFrame(), gc));
    toolManager.activate(TableTools.getDefaultTool(), gc);

    toolManager.addToolListener(new ToolListener() {
	    public void toolChanged(ToolChangeEvent e) 
	    {		    
		if (e.getTool() != null) 
		    itsCenterPanel.setCursor(e.getTool().getCursor());
	    }
	});
  }

  private void prepareToolbarPanel()
  {
    toolManager = new TableToolManager( TableTools.instance);    
    Tool arrow = toolManager.getToolByName("edit tool");     
    toolManager.activate(arrow, null);

    toolbar = new EditorToolbar(toolManager, EditorToolbar.HORIZONTAL);
  }

  private void prepareCenterPanel()
  {      
    itsCenterPanel = new TableDisplay(this); 
    itsCenterPanel.setBackground(Color.white);
    itsCenterPanel.setBorder(new EtchedBorder());

    itsCenterPanel.setBounds(0,0, getSize().width-SCROLLBAR_SIZE, 
			     getSize().height-SCROLLBAR_SIZE);

    add(itsCenterPanel, BorderLayout.CENTER);
    validate();
  }

  private void prepareGraphicContext()
  { 
    //prepares the graphic context
    gc = new TableGraphicContext(tableData);
    gc.setGraphicSource(itsCenterPanel);
    gc.setGraphicDestination(itsCenterPanel);
    gc.setToolManager( toolManager);
    gc.setSelection(new TableSelection( tableData));
    TableAdapter ta = new TableAdapter( tableData, gc);

    ta.addXZoomListener(new ZoomListener() {
	public void zoomChanged(float zoom, float oldZoom)
	{
	  gc.getFtsObject().requestSetVisibleWindow(gc.getVisibleHorizontalScope(), gc.getFirstVisibleIndex(), 
						    zoom, gc.getVisiblePixelsSize());
	  updateHorizontalScrollbar();
	  
	  if(zoom > 0.5)
	    {
	      if(oldZoom-zoom>0)
		{
		  int lvi = gc.getLastVisibleIndex()+10;
		  int lastId =  gc.getFtsObject().getLastUpdatedIndex();
		  if(lvi > lastId)
		    gc.getFtsObject().requestGetValues(lastId, lvi);
		  else repaint();
		}		
	      else 
		repaint();
	    }
	  else
	    gc.getFtsObject().requestGetPixels(0, 0);
	
	  gc.display("X : "+(int)(gc.getAdapter().getXZoom()*100)+" %   ");
	}
      });
    ta.addYZoomListener(new ZoomListener() {
	public void zoomChanged(float zoom, float old)
	{
	  updateVerticalScrollbar();
	  repaint();	  
	  gc.displayInfo("Y : "+(int)(gc.getAdapter().getYZoom()*100)+" %   ");
	}
      });

    gc.setAdapter(ta);
  }

  private void prepareVerticalScrollbar()
  {
    itsVerticalControl = new JScrollBar(Scrollbar.VERTICAL);
    itsVerticalControl.setMaximum(  gc.getVerticalRange());
    itsVerticalControl.setMinimum(  0);
    itsVerticalControl.setEnabled(false);
    itsVerticalControl.setVisible(false);

    itsVerticalControl.addAdjustmentListener( new AdjustmentListener() {
	public void adjustmentValueChanged( AdjustmentEvent e)
	{
	  int yT = itsVerticalControl.getMaximum() - itsVerticalControl.getModel().getExtent() - e.getValue();
	  gc.getAdapter().setYTransposition( yT);	  
	  repaint();
	}
      });

    add( itsVerticalControl, BorderLayout.EAST);
  }

  void updateVerticalScrollbar()
  {
    int verticalScope = (gc.isIvec()) ? (int)gc.getVisibleVerticalScope() : (int)(gc.getVisibleVerticalScope()*100);

    if(verticalScope >= gc.getVerticalRange())
      {
	if(itsVerticalControl.isVisible())
	  {    
	    itsVerticalControl.setEnabled(false);
	    itsVerticalControl.setVisible(false);
	  }
      }
    else
      {
	if(!itsVerticalControl.isVisible())
	  {
	    itsVerticalControl.setEnabled(true);
	    itsVerticalControl.setVisible(true);		      
	  }
	int oldAmount = itsVerticalControl.getVisibleAmount();
	int oldValue = itsVerticalControl.getValue();
	
	itsVerticalControl.setVisibleAmount( verticalScope);
	if( oldAmount != 0)
	  {
	    int val = oldValue*verticalScope/oldAmount;
	    if( val+itsVerticalControl.getModel().getExtent() < itsVerticalControl.getMaximum())
		itsVerticalControl.setValue( oldValue*verticalScope/oldAmount);	  
	  }
      }
  }
  private int hScrollVal = 0;
  private void prepareHorizontalScrollbar()
  {
    itsHorizontalControl = new JScrollBar(Scrollbar.HORIZONTAL, 0, 100, 0, 500);
    
    itsHorizontalControl.addAdjustmentListener( new AdjustmentListener() {
      public void adjustmentValueChanged( AdjustmentEvent e)
	{
	  if(hScrollVal == e.getValue()) return;
	  
	  int hDelta = hScrollVal-e.getValue();	    
	  
	  hScrollVal = e.getValue();
	  int last = gc.getLastVisibleIndex();
	  int first = gc.getFirstVisibleIndex();
	  gc.getAdapter().setXTransposition(hScrollVal);
	  gc.getFtsObject().requestSetVisibleWindow(gc.getVisibleHorizontalScope(), gc.getFirstVisibleIndex(), 
						    gc.getAdapter().getXZoom(), gc.getVisiblePixelsSize());
	  
	  if(gc.getAdapter().getXZoom()>0.5)
	    if(hDelta<0)
	      {
		int lvi = gc.getLastVisibleIndex()+10;
		if(lvi > gc.getFtsObject().getLastUpdatedIndex())
		  gc.getFtsObject().requestGetValues(last+1, lvi);
		else repaint();
	      }	    
	    else
	      repaint();
	  else
	    {
	      int deltax =  gc.getAdapter().getX(0)-gc.getAdapter().getX(hDelta); 
	      gc.getFtsObject().requestGetPixels(deltax, -hDelta);
	    }
	}
      });
    
    add(itsHorizontalControl, BorderLayout.SOUTH);

    updateHorizontalScrollbar();
  }

  void updateHorizontalScrollbar()
  {
    int extent = gc.getWindowHorizontalScope();
    
    if(extent >= tableData.getSize())
      {
	if(itsHorizontalControl.isVisible())
	  {
	    itsHorizontalControl.setValue(0);
	    itsHorizontalControl.setEnabled(false);
	    itsHorizontalControl.setVisible(false);		      
	  }
      }
    else
      {
	int last = gc.getLastVisibleIndex();
	int first = gc.getFirstVisibleIndex();
	
	if(!itsHorizontalControl.isVisible())
	  {
	    itsHorizontalControl.setEnabled(true);
	    itsHorizontalControl.setVisible(true);
	  }
	
	itsHorizontalControl.setVisibleAmount(extent);
	
	if(tableData.getSize() > 0)
	  itsHorizontalControl.setMaximum(tableData.getSize()-1);
	
	if(last < first+extent) 
	  {
	    itsHorizontalControl.setValue(itsHorizontalControl.getValue()-(first+extent-last));
	    itsHorizontalControl.setVisibleAmount(gc.getVisibleHorizontalScope());
	  }
      }    
  }

  public void setMaximumValue(int value)
  {
    gc.setVerticalMaximum(value);
    itsVerticalControl.setMaximum( gc.isIvec() ? value : value*100);
    updateVerticalScrollbar();
    repaint();
  }

  public void setMinimumValue(int value)
  {
    gc.setVerticalMinimum(value);
    itsVerticalControl.setMinimum( gc.isIvec() ? value : value*100);
    updateVerticalScrollbar();
    repaint();
  }

  public TableGraphicContext getGraphicContext()
  {
    return gc;
  }

  /**
   * TableDataListener interface */
  public void valueChanged(int index1, int index2)
  {    
    int x = gc.getAdapter().getX( index1);
    int w = gc.getAdapter().getX( index2+1) - x;
    itsCenterPanel.repaint( new Rectangle( x, 0, w, itsCenterPanel.getSize().height));
  }
  public void pixelsChanged(int index1, int index2)
  {    
    itsCenterPanel.repaint();
  }
  public void tableSetted()
  {
    itsCenterPanel.repaint();
  }
  public void tableCleared()
  {
    itsCenterPanel.repaint();
  }
  public void sizeChanged(int size, int oldSize)
  {
    if( oldSize == 0)
      {
	gc.getFtsObject().requestSetVisibleWindow( gc.getVisibleHorizontalScope(), gc.getFirstVisibleIndex(), 
						   ((TableAdapter)gc.getAdapter()).getXZoom(), 
						   gc.getVisiblePixelsSize());
	gc.getFtsObject().requestGetValues();
      }
    updateHorizontalScrollbar();
    itsCenterPanel.repaint();
  }

  /**
   * Sets the "hollow" representation mode */
  public void hollow()
  {
    itsTableRenderer.setMode(TableRenderer.HOLLOW);
    repaint();
  }

  /** 
   * Sets the "solid" representation mode */
  public void solid()
  {
    itsTableRenderer.setMode(TableRenderer.SOLID);
    repaint();
  }

  public void changeBackgroundColor()
  {
    Color newColor = JColorChooser.showDialog(null,"Choose Background Color", itsTableRenderer.getBackColor());
	  
    if(newColor != null){
      itsTableRenderer.setBackColor( newColor);
      repaint();
    }
  }

  public void changeForegroundColor()
  {
    Color newColor = JColorChooser.showDialog(null,"Choose Foreground Color", itsTableRenderer.getForeColor());
	  
    if(newColor != null){
      itsTableRenderer.setForeColor( newColor);
      repaint();
    }
  }

  public void Refresh(){
    repaint();
  }

  public void Undo()
  {
     try 
     {
	 ((UndoableData) gc.getFtsObject()).undo();
     } catch (CannotUndoException e1) {
       System.out.println("can't undo");	
     }
  }

  public void Redo()
  {
    try 
    {
      ((UndoableData) gc.getFtsObject()).redo();
    } catch (CannotRedoException e1) {
      System.out.println("can't redo");
    }
  }

  int lastCopySize = 0;
  public void Copy()
  {
    gc.getFtsObject().requestCopy(gc.getSelection().getFirstSelected(), gc.getSelection().size());
    lastCopySize = gc.getSelection().size();
  }

  public void Cut()
  {
    int vsize, pixsize;
    int selsize = gc.getSelection().size();
    int sizeAfterCut = gc.getFtsObject().getSize()-selsize;

    if((sizeAfterCut >= gc.getLastVisibleIndex(sizeAfterCut))&&(sizeAfterCut> gc.getWindowHorizontalScope()))
      {
	vsize = gc.getWindowHorizontalScope();
	pixsize = gc.getGraphicDestination().getSize().width;
      } 
    else
      {
	vsize = sizeAfterCut-gc.getFirstVisibleIndex();
	pixsize = gc.getAdapter().getX(vsize);
	}

    gc.getFtsObject().requestCut(gc.getSelection().getFirstSelected(), selsize, vsize, pixsize);
    lastCopySize = selsize;
    gc.getSelection().deselectAll();
  }

  public void Paste()
  {
    int first;
    int size = gc.getSelection().size();

    if(size == 0) first = gc.getSelection().getCaretPosition();
    else first = gc.getSelection().getFirstSelected();

    gc.getFtsObject().requestPaste(first, size);
  }

  public void Insert()
  {
    int first;
    int selsize = gc.getSelection().size();

    if(selsize == 0) first = gc.getSelection().getCaretPosition();
    else first = gc.getSelection().getFirstSelected();
    
    int vsize, pixsize;
    int sizeAfterInsert = gc.getFtsObject().getSize()+lastCopySize;
    
    if((sizeAfterInsert >= gc.getLastVisibleIndex(sizeAfterInsert))&&(sizeAfterInsert > gc.getWindowHorizontalScope()))
      {
	vsize = gc.getWindowHorizontalScope();
	pixsize = gc.getGraphicDestination().getSize().width;
      } 
    else
      {
	vsize = sizeAfterInsert-gc.getFirstVisibleIndex();
	pixsize = gc.getAdapter().getX(vsize);
      }

    gc.getFtsObject().requestInsert(first, vsize, pixsize);
    gc.getSelection().deselectAll();
  }
  /**
   * from Panel class...
   */
  public Dimension getMinimumSize() 
  {  
    return size;
  }    
  
  /**
   * from Panel class...
   */
  public Dimension getPreferredSize() 
  {  
    return getMinimumSize();	  
  }

  public FtsTableObject getData(){
      return (FtsTableObject)tableData;
  }

  //------------------- Editor interface ---------------
  EditorContainer itsEditorContainer;

  public EditorContainer getEditorContainer(){
    return itsEditorContainer;
  }

  public void close(boolean doCancel){
    ((Component)itsEditorContainer).setVisible(false);
    ftsObj.requestDestroyEditor();
    MaxWindowManager.getWindowManager().removeWindow((Frame)itsEditorContainer);
  }
  public void save(){}
  public void saveAs(){}
  public void print(){}
}







