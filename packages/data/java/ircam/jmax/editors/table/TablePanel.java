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
public class TablePanel extends JPanel implements StatusBarClient, TableDataListener, Editor{

  //--- Fields  
  final static int SCROLLBAR_SIZE = 30;
  final static int PANEL_WIDTH = 500;
  final static int PANEL_HEIGHT = 240+SCROLLBAR_SIZE;

  /*InfoPanel itsStatusBar;*/

  JScrollBar itsVerticalControl, itsHorizontalControl;

  Dimension size = new Dimension(PANEL_WIDTH, PANEL_HEIGHT);

  public EditorToolbar toolbar;
  TableGraphicContext gc;
  TableRenderer itsTableRenderer;
  TableDataModel tableData;
  TableDisplay itsCenterPanel;
  ToolManager toolManager;

  /*ScalePanel scalePanel;*/

  //static Dimension toolbarDimension = new Dimension(30, 200);
  /**
   * Constructor. */
  public TablePanel(EditorContainer container, TableDataModel tm) {
    super();
    tableData = tm;
    itsEditorContainer = container;

    setSize(PANEL_WIDTH, PANEL_HEIGHT);
    setLayout(new BorderLayout());

    //make the NORTH Status bar 
    //prepareStatusBar();

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
	    gc.getFtsObject().requestSetVisibleWindow(gc.getVisibleHorizontalScope(), gc.getFirstVisibleIndex(), 
						      ((TableAdapter)gc.getAdapter()).getXZoom(), 
						      gc.getVisiblePixelsSize());
	    if(gc.getAdapter().getXZoom() > 0.5)		    
		gc.getFtsObject().requestGetValues();
	    else
		gc.getFtsObject().requestGetPixels(0, 0);
	}
    });
    //470 is the default size of the TableDisplay .......
    gc.getFtsObject().requestSetVisibleWindow(/*gc.getVisibleHorizontalScope()*/470, /*gc.getFirstVisibleIndex()*/0, 
					      (float)1.0, gc.getVisiblePixelsSize());
    gc.getFtsObject().requestGetValues();
  }

  void frameAvailable()
  {
    gc.setFrame(GraphicContext.getFrame(this));
    toolManager.addContextSwitcher(new WindowContextSwitcher(gc.getFrame(), gc));
    toolManager.activate(TableTools.getDefaultTool(), gc);
  }

  /*private void prepareStatusBar()
    {
    JPanel northSection = new JPanel();
    northSection.setLayout(new BoxLayout(northSection, BoxLayout.Y_AXIS));    
    itsStatusBar = new InfoPanel();    
    itsStatusBar.setSize(300, 20);
    northSection.add(itsStatusBar);
    add(northSection, BorderLayout.NORTH);
    }*/
  
  private void prepareToolbarPanel()
  {
    toolManager = new ToolManager(TableTools.instance);    
    /*toolManager.addToolListener(new ToolListener(){
      public void toolChanged(ToolChangeEvent e) 
      {
      if (e.getTool() != null) 
      {
      itsStatusBar.post(e.getTool(), "");			
      }	    
      }
      });*/
    toolbar = new EditorToolbar(toolManager, EditorToolbar.HORIZONTAL);
    /*toolbar.setSize(60, 25);    
      toolbar.setPreferredSize(new Dimension(60, 25));    
      
      JPanel toolbarPanel = new JPanel();
      toolbarPanel.setSize(108, 25);
      toolbarPanel.setPreferredSize(new Dimension(108, 25));
      toolbarPanel.setLayout(new BorderLayout());
      toolbarPanel.add(toolbar, BorderLayout.CENTER);
      itsStatusBar.addWidgetAt(toolbarPanel, 2);
      itsStatusBar.validate();*/
  }

  private void prepareCenterPanel()
  {      
    itsCenterPanel = new TableDisplay(this); 
    itsCenterPanel.setBackground(Color.white);
    itsCenterPanel.setBorder(new EtchedBorder());

    itsCenterPanel.setBounds(/*toolbarDimension.width+ScalePanel.scaleDimension.width, 
			       InfoPanel.INFO_WIDTH,*/ 
			     0,0, 
			     getSize().width/*-toolbarDimension.width-ScalePanel.scaleDimension.width*/-SCROLLBAR_SIZE, 
			     getSize().height/*-InfoPanel.INFO_HEIGHT*/-SCROLLBAR_SIZE);

    add(itsCenterPanel, BorderLayout.CENTER);
    validate();
  }

  private void prepareGraphicContext()
  { 
    //prepares the graphic context
    gc = new TableGraphicContext(tableData);
    gc.setGraphicSource(itsCenterPanel);
    gc.setGraphicDestination(itsCenterPanel);
    gc.setCoordWriter(new CoordinateWriter(gc));
    gc.setToolManager(toolManager);
    gc.setSelection(new TableSelection(tableData));
    TableAdapter ta = new TableAdapter(tableData, itsCenterPanel.getSize(), gc.getVerticalMaximum());

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
	}
      });
    ta.addYZoomListener(new ZoomListener() {
	public void zoomChanged(float zoom, float old)
	{
	  updateVerticalScrollbar();
	  repaint();
	}
      });

    gc.setAdapter(ta);
    //gc.setStatusBar(itsStatusBar);
  }

  private void prepareVerticalScrollbar()
  {
    itsVerticalControl = new JScrollBar(Scrollbar.VERTICAL, 0, 100, -gc.getVerticalMaximum(), gc.getVerticalMaximum());

    itsVerticalControl.addAdjustmentListener( new AdjustmentListener() {
      public void adjustmentValueChanged( AdjustmentEvent e)
	{
	  //gc.getAdapter().setOY(-e.getValue());	    
	  gc.getAdapter().setYTransposition( e.getValue());
	  /*scalePanel.updateScale();*/
	  repaint();
	}
    });

    add(itsVerticalControl, BorderLayout.EAST);
    start0 = gc.getAdapter().getY(0);
    updateVerticalScrollbar();
  }

  int start0;
  void updateVerticalScrollbar()
  {
    int verticalScope = gc.getVisibleVerticalScope();

    if(verticalScope >= gc.getVerticalMaximum()*2)
      {
	if(itsVerticalControl.isVisible())
	  {
	    itsVerticalControl.setValue(-verticalScope/2);
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
	itsVerticalControl.setVisibleAmount(verticalScope);
	//itsVerticalControl.setValue(-Math.round(start0/gc.getAdapter().getYZoom()));
	itsVerticalControl.setValue( oldValue*verticalScope/oldAmount);
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
	itsVerticalControl.setMaximum(value);
	itsVerticalControl.setMinimum(-value);
	updateVerticalScrollbar();
    }

  /**
   * from the StatusBarClient interface
   */
  public String getName() 
  {
    return "";
  }


  /**
   * from the StatusBarClient interface
   */
  public ImageIcon getIcon() 
  {
    return null;
  }

  /**
   * TableDataListener interface */
  public void valueChanged(int index1, int index2)
  {
     itsCenterPanel.rangePaint(index1, index2);
  }
  public void valueChanged(int index)
  {
      itsCenterPanel.singlePaint(index);
  }
  public void tableSetted()
  {
    itsCenterPanel.repaint();
  }
  public void tableCleared()
  {
      itsCenterPanel.repaint();
  }
  public void sizeChanged(int size)
  {
    updateHorizontalScrollbar();
    updateVerticalScrollbar();
    if(!itsHorizontalControl.isVisible())
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

  public void Close(boolean doCancel){
    ((Component)itsEditorContainer).setVisible(false);
    getData().requestDestroyEditor();
    MaxWindowManager.getWindowManager().removeWindow((Frame)itsEditorContainer);
  }
}







