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

package ircam.jmax.editors.table;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;

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
 * (the graphic representation).
 * This class takes care of creating all the components of an editing session
 * (tools, Renderer, ...) and link them togheter.
 */
public class TablePanel extends JPanel implements TableDataListener, Editor{
	
  //--- Fields  
  final static int SCROLLBAR_SIZE = 30;
  final static int PANEL_WIDTH = 500;
  static int PANEL_HEIGHT;
  final static int IVEC_PANEL_HEIGHT = 270;
  final static int FVEC_PANEL_HEIGHT = /*200*/ 400;
	
  public JScrollBar itsHorizontalControl;
	
  Dimension size;
	
  public EditorToolbar toolbar;
  TableGraphicContext gc;
  TableRenderer itsTableRenderer;
  TableDataModel tableData;
  TableDisplay itsCenterPanel;
  TableToolManager toolManager;
  FtsObjectWithEditor ftsObj;
	boolean tableShown = false;
  int oldVisibleScope = 0;
  
  TableVerticalRuler verticalRuler;
  
  ActionListener getValuesPerformer; 
  javax.swing.Timer getValuesTimer;
  /**
		* Constructor. */
  public TablePanel(EditorContainer container, FtsObjectWithEditor ftsObj, TableDataModel tm) {
    super();
		
    this.ftsObj = ftsObj;
    tableData = tm;
    itsEditorContainer = container;
		
    PANEL_HEIGHT = (ftsObj instanceof FtsIvecObject) ? IVEC_PANEL_HEIGHT : FVEC_PANEL_HEIGHT;
    size = new Dimension(PANEL_WIDTH, PANEL_HEIGHT);
		
    setLayout(new BorderLayout());
		
    prepareToolbarPanel();
    
    //... the center panel
    prepareCenterPanel();
		
    //... the Graphic context
    prepareGraphicContext();
    itsCenterPanel.setGraphicContext(gc);
		
    prepareVerticalRuler();
      
    //... the renderer
    itsTableRenderer = new TableRenderer(gc);
    itsCenterPanel.setRenderer(itsTableRenderer);
    gc.setRenderManager(itsTableRenderer);
    
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
    
    getValuesPerformer = new ActionListener() {
      public void actionPerformed(ActionEvent evt) {                
        if(gc.getAdapter().getXZoom() > 0.5)		    
          gc.getFtsObject().requestGetValues();
        else
          gc.getFtsObject().requestGetPixels(0, 0);        
      }
    };
    
    addComponentListener( new ComponentAdapter() {
			public void componentResized(ComponentEvent e)
		  {        
        if(tableShown)
        {          
          updateHorizontalScrollbar();//????
          int visibleScope = gc.getVisibleHorizontalScope();
          
          gc.getFtsObject().requestSetVisibleWindow( visibleScope, gc.getFirstVisibleIndex(), 
                                                     gc.getWindowHorizontalScope(), ((TableAdapter)gc.getAdapter()).getXZoom(), 
                                                     gc.getVisiblePixelsSize());
          
          if((visibleScope > oldVisibleScope /*&& !gc.isCompletelyUpdated()*/) || (gc.getAdapter().getXZoom() <= 0.5))
          {
            if(getValuesTimer == null)
            {
              getValuesTimer = new javax.swing.Timer(300, getValuesPerformer);
              getValuesTimer.setRepeats(false);
              getValuesTimer.start();
            }
            else
              getValuesTimer.restart();

            oldVisibleScope = visibleScope;
          }
        }        
      }
      public void componentMoved(ComponentEvent e)
		  {
        if(!tableShown)
        {
          gc.getFtsObject().requestSetVisibleWindow( gc.getVisibleHorizontalScope(), gc.getFirstVisibleIndex(), 
                                                     gc.getWindowHorizontalScope(), ((TableAdapter)gc.getAdapter()).getXZoom(), 
                                                     gc.getVisiblePixelsSize());
          if(gc.getAdapter().getXZoom() > 0.5)		    
            gc.getFtsObject().requestGetValues();
          else
            gc.getFtsObject().requestGetPixels(0, 0);
          
          tableShown = true;
        }
      }
		});
    //470 is the default size of the TableDisplay .......
    /*gc.getFtsObject().requestSetVisibleWindow( 470, 0, gc.getWindowHorizontalScope(), (double)1.0, gc.getVisiblePixelsSize());
    gc.getFtsObject().requestGetValues();*/
    
    setSize(PANEL_WIDTH, PANEL_HEIGHT);
    setPreferredSize(size);
  }
  
  public void setContainer(EditorContainer container)
  {
    itsEditorContainer = container;
    gc.setFrame(GraphicContext.getFrame(this));
    toolManager.addContextSwitcher(new WindowContextSwitcher(gc.getFrame(), gc));
    toolManager.activate(TableTools.getDefaultTool(), gc);
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
	
  JPanel firstCenterPanel;
  private void prepareCenterPanel()
  {      
    itsCenterPanel = new TableDisplay(this); 
    itsCenterPanel.setBackground(Color.white);
    itsCenterPanel.setBorder(new EtchedBorder());

    /*add(itsCenterPanel, BorderLayout.CENTER);
    validate();*/

    firstCenterPanel = new JPanel();
    firstCenterPanel.setLayout(new BoxLayout(firstCenterPanel, BoxLayout.X_AXIS));
    //aPanel.add(Box.createHorizontalStrut(2));
    firstCenterPanel.add(itsCenterPanel);
    firstCenterPanel.add(Box.createHorizontalStrut(2));
    
    add(firstCenterPanel, BorderLayout.CENTER);
    validate();
  }
  
  private void prepareVerticalRuler()
  {
    verticalRuler = new TableVerticalRuler(gc);
    firstCenterPanel.add(verticalRuler, 0);
  }
  
	private void prepareGraphicContext()
  { 
    //prepares the graphic context
    gc = new TableGraphicContext( tableData);
    gc.setGraphicSource( itsCenterPanel);
    gc.setGraphicDestination( itsCenterPanel);
    gc.setToolManager( toolManager);
    gc.setSelection( new TableSelection( tableData));
    TableAdapter ta = new TableAdapter( tableData, gc);
		
    ta.addXZoomListener( new ZoomListener() {
			public void zoomChanged(float zoom, float oldZoom)
		  {
        int lastId = gc.getFtsObject().getLastVisibleIndex();
        int first = gc.getFtsObject().getFirstVisibleIndex();
        int size = gc.getFtsObject().getSize();   
        
				gc.getFtsObject().requestSetVisibleWindow(gc.getVisibleHorizontalScope(), gc.getFirstVisibleIndex(), 
																									gc.getWindowHorizontalScope(), zoom, gc.getVisiblePixelsSize());
				updateHorizontalScrollbar();
				
				if(zoom > 0.5)
				{          
          if((oldZoom-zoom>0) || (oldZoom-zoom<0 && lastId == 0))
            gc.getFtsObject().requestGetValues(first, gc.getFtsObject().getLastVisibleIndex(), false);
					else 
						repaint();
				}
				else
					gc.getFtsObject().requestGetPixels(0, 0);
				
				gc.display("X : "+(int)(gc.getAdapter().getXZoom()*100)+" %   ");
		  }
		});
		
    gc.setAdapter(ta);
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
        int last = gc.getFtsObject().getLastVisibleIndex();
        int first = gc.getFtsObject().getFirstVisibleIndex();
        int size = gc.getFtsObject().getSize();
                
				gc.getAdapter().setXTransposition(hScrollVal);
				gc.getFtsObject().requestSetVisibleWindow(gc.getVisibleHorizontalScope(), gc.getFirstVisibleIndex(), 
																									gc.getWindowHorizontalScope(), gc.getAdapter().getXZoom(), 
                                                  gc.getVisiblePixelsSize());
        
				if(gc.getAdapter().getXZoom() > 0.5)
					if(hDelta<0)
					{
            int end = gc.getFtsObject().getLastVisibleIndex();
            gc.getFtsObject().requestGetValues(last, end, true);            
					}	    
          else
          {
            int end = gc.getFtsObject().getFirstVisibleIndex();
            gc.getFtsObject().requestGetValues(first, end, true);/* INVERTED! to see negative scroll */
          }
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
      itsHorizontalControl.setValue(0);
      if(tableData.getSize()>0)
      {
        itsHorizontalControl.setMaximum(tableData.getSize()-1);
        itsHorizontalControl.setVisibleAmount(tableData.getSize()-1);
      }
      itsHorizontalControl.setEnabled(false);    
    }
    else
		{
			int last = gc.getLastVisibleIndex();
			int first = gc.getFirstVisibleIndex();

      itsHorizontalControl.setEnabled(true);			
		
      if(tableData.getSize() > 0)
				itsHorizontalControl.setMaximum(tableData.getSize()-1);
      itsHorizontalControl.setVisibleAmount(extent);
      
			if(last < first+extent) 
			{
				itsHorizontalControl.setValue(itsHorizontalControl.getValue()-(first+extent-last));
				itsHorizontalControl.setVisibleAmount(gc.getVisibleHorizontalScope());
      }
		}    
  }
	
  public void setMaximumValue(float value)
  {
    gc.setVerticalMaximum(value);
    repaint();
  }
	
  public void setMinimumValue(float value)
  {
    gc.setVerticalMinimum(value);
    repaint();
  }
	
  public TableGraphicContext getGraphicContext()
  {
    return gc;
  }
	
  /**
		* TableDataListener interface */
  public void valueChanged(int index1, int index2, boolean fromScroll)
  {    
    if( fromScroll)
			itsCenterPanel.repaint();
    else
    {
			int x = gc.getAdapter().getX((index1 > 2) ? index1-2 : index1);
			int w = gc.getAdapter().getX( (index2 < gc.getDataModel().getSize()-2) ? index2+2 : index2+1) - x;        
			if( gc.getAdapter().getXZoom() < 0.5)
        if( w < 20)
        {		
					w = 20;
					x = (x > 10) ? x-10 : x;
        }
					itsCenterPanel.repaint( new Rectangle( x, 0, w, itsCenterPanel.getSize().height));
    }
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
																								 gc.getWindowHorizontalScope(), ((TableAdapter)gc.getAdapter()).getXZoom(), 
																								 gc.getVisiblePixelsSize());
			gc.getFtsObject().requestGetValues();
		}
    SwingUtilities.invokeLater(new Runnable() {
	    public void run()
      {
        updateHorizontalScrollbar();
        itsCenterPanel.repaint();
      }
    });
  }
  public void tableUpdated()
  {
    gc.getFtsObject().requestSetVisibleWindow( gc.getVisibleHorizontalScope(), gc.getFirstVisibleIndex(), 
                                               gc.getWindowHorizontalScope(), ((TableAdapter)gc.getAdapter()).getXZoom(), 
                                               gc.getVisiblePixelsSize());
    updateHorizontalScrollbar();
    itsCenterPanel.repaint();
  }
  
	public void tableRange(float min_val, float max_val)
  {
    gc.setVerticalMinimum(min_val);
    gc.setVerticalMaximum(max_val);
    repaint();
  }
  
  public void tableReference(int nRowsRef, int nColsRef, String typeRef, int indexRef, int onsetRef, int sizeRef){}
  public void tableNameChanged(String name){}
  /**
		* Sets the "hollow" representation mode */
  public void setPointsView()
  {
    itsTableRenderer.setMode(TableRenderer.POINTS);
    repaint();
  }
	
  /** 
		* Sets the "solid" representation mode */
  public void setFilledView()
  {
    itsTableRenderer.setMode(TableRenderer.FILLED);
    repaint();
  }
	
  public void setLinesView()
  {
    itsTableRenderer.setMode(TableRenderer.LINES);
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
	
  TableInspector currentInspector;
  public void setCurrentInspector(TableInspector inspector)
  {
    currentInspector = inspector;
  }
  public TableInspector getCurrentInspector()
  {
    return currentInspector;
  }
  
  //------------------- Editor interface ---------------
  EditorContainer itsEditorContainer;
	
  public EditorContainer getEditorContainer(){
    return itsEditorContainer;
  }
	
  public void close(boolean doCancel){
    if(currentInspector!=null)
      currentInspector.close();
    ftsObj.closeEditor();
    ftsObj.requestDestroyEditor();
  }
  public void save(){}
  public void saveAs(){}
  public void print(){}
}







