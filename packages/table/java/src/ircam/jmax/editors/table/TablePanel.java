//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//
package ircam.jmax.editors.table;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.widgets.*;

import java.awt.*;
import java.util.*;
import java.awt.event.*;
import java.io.*;
import tcl.lang.*;
import javax.swing.*;
import javax.swing.border.*;
import javax.swing.event.*;

/**
 * The panel in the Table editor's window, containing the toolbar, the CenterPanel 
 * (the graphic representation), the vertical scrollbar.
 * This class takes care of creating all the components of an editing session
 * (tools, Renderer, ...) and link them togheter.
 */
public class TablePanel extends JPanel implements ToolbarProvider, ToolListener, StatusBarClient, TableDataListener{
  
  /**
   * Constructor. */
  public TablePanel(TableDataModel tm) {
    super();

    this.tm = tm;
    instance = this;
    setSize(PANEL_WIDTH, PANEL_HEIGHT);
    setLayout(new BorderLayout());
    setBackground(Color.white);
    setDoubleBuffered(true);


    //make the NORTH Status bar 
    prepareStatusBar();

    //... the panel that will contain the toolbar
    prepareToolbarPanel();

    //... the center panel
    prepareCenterPanel();

    //... the Graphic context (and the selection)
    gc = new TableGraphicContext(tm);
    prepareGraphicContext();

    itsTableRenderer = new TableRenderer(gc);
    itsCenterPanel.setRenderer(itsTableRenderer);
    gc.setRenderManager(itsTableRenderer);
    

    //... the vertical position controller
    prepareVerticalScrollbar();


    //... the widgets in the statusBar
    addWidgets();

    // Add listeners for data model changes
    tm.addListener(this);

    // Add a listener for selection content changes
    // (should this be in the SelectionLayer?)
    gc.getSelection().addListSelectionListener( new ListSelectionListener() {
      public void valueChanged(ListSelectionEvent e)
	{
	  if (e.getValueIsAdjusting())
	      return;

	  itsCenterPanel.repaint();
	}
    });

    //the repaints due to selection's state change are
    //handled in the SelectionLayer level of the RenderManager
 
    initTools();
  }



  private void prepareToolbarPanel()
  {
    toolbarPanel = new Box(BoxLayout.Y_AXIS) {
      public Dimension getMinimumSize()
	{
	  return toolbarDimension;
	}
      
      public Dimension getPreferredSize()
	{
	  return toolbarDimension;
	}
    };
    toolbarPanel.setSize(toolbarDimension.width, toolbarDimension.height);
    
  }
  
  /**
   * note: can't create the toolbar in the constructor,
   * because the Frame is not available yet.
   * The EditorToolbar class uses the Frame information
   * in order to correctly map graphic contexts on windows.
   */
  public EditorToolbar prepareToolbar() 
  {

    gc.setFrame(GraphicContext.getFrame(this));

    tb = new EditorToolbar(this, EditorToolbar.VERTICAL);
    tb.setFloatable(false);
    
    gc.setToolbar(tb);

    // prepare the JPanel containing the toolbar when anchored
    JPanel c = new JPanel() {

      // this callback is called when the user anchors the tooolbar
      protected void addImpl(Component comp,
			     Object constraints,
			     int index)
	{
	  if (! toolbarAnchored && GraphicContext.getFrame(tb) != null)
	    {
	      GraphicContext.getFrame(tb).setVisible(false);
	      GraphicContext.getFrame(tb).dispose();
	    }

	  super.addImpl(comp, constraints, index);
	  TablePanel.toolbarAnchored = true;
	}

      // the tolbar is going to be unanchored
      public void remove(Component c)
	{
	  super.remove(c);
	  TablePanel.toolbarAnchored = false;
	  doLayout();
	}
      
    };

    c.setLayout(new BorderLayout());
    c.setOpaque(false);
    
    c.setSize(30, 200);
    
    toolbarPanel.add(c);

    add(toolbarPanel, BorderLayout.WEST);

    
    if ( toolbarAnchored)
      {
	tb.setSize(30, 200);
	c.add(tb, BorderLayout.CENTER);
      }

    tb.addToolListener(this);
    itsStatusBar.post(tb.getTool(), "");

    return tb;
  }

  private void prepareStatusBar()
  {
    // WARNING:
    // make a superclass! Explode.ScrPanel is very similar
    JPanel northSection = new JPanel();
    northSection.setLayout(new BoxLayout(northSection, BoxLayout.Y_AXIS));
    
    itsStatusBar = new InfoPanel();
    
    itsStatusBar.setSize(300, 20);

    northSection.add(itsStatusBar);

    add(northSection, BorderLayout.NORTH);

  }
  

  /**
   * Prepare and add the zoom widgets in the toolbar */
  private void addWidgets()
  {

    // ------ the "x" label and zoom factor
    JLabel xLabel = new JLabel("x");
    xLabel.setSize(15,InfoPanel.INFO_HEIGHT );
    xLabel.setVerticalAlignment(JLabel.CENTER);
    
    currentXZoom = new JLabel(((int)(gc.getAdapter().getXZoom()*100))+"%");
    currentXZoom.setFont(new Font(currentXZoom.getFont().getName(), Font.BOLD, 10));
    currentXZoom.setBorder(new EtchedBorder());
    currentXZoom.setSize(40, InfoPanel.INFO_HEIGHT);
    currentXZoom.setOpaque(true);
    currentXZoom.setBackground(Color.white);
    currentXZoom.setForeground(Color.black);
    
    gc.getAdapter().addXZoomListener(new ZoomListener() {
      public void zoomChanged(float zoom)
	{
	  currentXZoom.setText(((int)(zoom*100))+"%");
	}
    });

    // ------ the "y" label and zoom factor
    JLabel yLabel = new JLabel("y");
    yLabel.setSize(15,InfoPanel.INFO_HEIGHT );
    yLabel.setVerticalAlignment(JLabel.CENTER);

    currentYZoom = new JLabel(((int)(gc.getAdapter().getYZoom()*100))+"%");
    currentYZoom.setFont(new Font(currentYZoom.getFont().getName(), Font.BOLD, 10));
    currentYZoom.setBorder(new EtchedBorder());
    currentYZoom.setOpaque(true);
    currentYZoom.setBackground(Color.white);
    currentYZoom.setForeground(Color.black);
    currentYZoom.setSize(40, InfoPanel.INFO_HEIGHT);

    gc.getAdapter().addYZoomListener(new ZoomListener() {
      public void zoomChanged(float zoom)
	{
	  currentYZoom.setText(((int)(zoom*100))+"%");
	}
    });

    //-- increment listeners and controllers
    IncrementListener xil = new IncrementListener() {
      public void increment()
	{
	  TableAdapter a = gc.getAdapter();
	  if (a.getXZoom()>=1)
	    a.setXZoom(a.getXZoom()+1);
	  else
	    a.setXZoom(a.getXZoom()*(1/(1-a.getXZoom())));

	}

      public void decrement()
	{
	  TableAdapter a = gc.getAdapter();
	  if (a.getXZoom()>1)
	    a.setXZoom(a.getXZoom()-1);
	  else
	    a.setXZoom(a.getXZoom()*(1/(1+a.getXZoom())));
	}
    };


    IncrementListener yil = new IncrementListener() {
      public void increment()
	{
	  TableAdapter a = gc.getAdapter();
	  if (a.getYZoom()>=1)
	    a.setYZoom(a.getYZoom()+1);
	  else
	    a.setYZoom(a.getYZoom()*(1/(1-a.getYZoom())));

	}

      public void decrement()
	{
	  TableAdapter a = gc.getAdapter();
	  if (a.getYZoom()>1)
	    a.setYZoom(a.getYZoom()-1);
	  else
	    a.setYZoom(a.getYZoom()*(1/(1+a.getYZoom())));
	}
    };

    IncrementController xic = new IncrementController(xil);
    IncrementController yic = new IncrementController(yil);
    xic.setSize(20, 20);
    yic.setSize(20, 20);
    xic.setOpaque(false);
    yic.setOpaque(false);
    
    // everything in the status bar!
    itsStatusBar.addWidget(xLabel);
    itsStatusBar.addWidget(currentXZoom);
    itsStatusBar.addWidget(xic);
    itsStatusBar.addWidget(yLabel);
    itsStatusBar.addWidget(currentYZoom);
    itsStatusBar.addWidget(yic);

  }

  private void prepareCenterPanel()
  {
    itsCenterPanel = new TableDisplay() {
      /**
       * Filters out the mouse events that trigger the popup menu.
       * Let all the other event pass undisturbed */
      protected void processMouseEvent(MouseEvent e)
	{
	  if (e.isPopupTrigger()) 
	    {
	      if (!InteractionModule.isSuspended())
		{
		  InteractionModule.suspend();
		}
	      Tabler.toolbar.itsPopupMenu.show (e.getComponent(), e.getX()-10, e.getY()-10);
	    }
	  else {
	    if (InteractionModule.isSuspended())
	      {
		InteractionModule.resume();
	      }
	    super.processMouseEvent(e);
	  }
	} 
      
    };
 
    itsCenterPanel.setBounds(toolbarPanel.getSize().width, 
			     InfoPanel.INFO_WIDTH, 
			     getSize().width-toolbarPanel.getSize().width-SCROLLBAR_SIZE, 
			     getSize().height-InfoPanel.INFO_HEIGHT);

    itsCenterPanel.setBackground(Color.white);
    itsCenterPanel.setDoubleBuffered(true);

    add(itsCenterPanel, BorderLayout.CENTER);
  }


  private void prepareGraphicContext()
    { //prepares the graphic context

      gc.setGraphicSource(itsCenterPanel);
      gc.setGraphicDestination(itsCenterPanel);
      gc.setCoordWriter(new CoordinateWriter(gc));

      gc.setSelection(new TableSelection(tm));
      gc.setStatusBar(itsStatusBar);
      TableAdapter ta = new TableAdapter(tm, itsCenterPanel.getSize(), 128);
      
      //do a repaint() when zoom changes 
      ta.addXZoomListener(new ZoomListener() {
	public void zoomChanged(float zoom)
	  {
	    repaint();
	  }
      });
      ta.addYZoomListener(new ZoomListener() {
	public void zoomChanged(float zoom)
	  {
	    repaint();
	  }
      });

      //setOriginAndZoom(ta, tm);
      gc.setAdapter(ta);
      gc.setStatusBar(itsStatusBar);
    }


  private void prepareVerticalScrollbar()
  {
    itsPositionControl = new Scrollbar(Scrollbar.VERTICAL, 128, 100, -2000, 2000);

    itsPositionControl.addAdjustmentListener( new AdjustmentListener() {
      public void adjustmentValueChanged( AdjustmentEvent e)
	{
	  gc.getAdapter().setOY(256-e.getValue());

	  repaint();
	}
    });
    add(itsPositionControl, BorderLayout.EAST);

  }


  /**
   * prepares the tools that will be used with this editor.
   * 
   */
  private void initTools() 
  {

    if (tools != null) return; //someone else created it

    tools = new Vector();
    String fs = File.separator;
    String path = MaxApplication.getProperty("root")+fs+"packages/table/images"+fs;

    itsDefaultTool = new TableSelecter(new ImageIcon(path+"table_selecter.gif"));

    tools.addElement( itsDefaultTool);
    tools.addElement( new PencilTool(new ImageIcon(path+"pencil.gif")));
    tools.addElement( new LinerTool(new ImageIcon(path+"liner.gif")));

  }

  /**
   * ToolbarProvider interface
   */
  public Enumeration getTools() 
  {  
    return tools.elements();
  }

  /**
   * ToolbarProvider interface
   */
  public GraphicContext getGraphicContext() 
  {
    return gc;
  }

  /**
   * ToolbarProvider interface
   */
  public Tool getDefaultTool() 
  {
    return itsDefaultTool;
  }

  /**
   * Callback from the toolbar when a new tool have been
   * selected by the user
   */ 
  public void toolChanged(ToolChangeEvent e) 
  {
    if (e.getTool() != null) 
      {
	itsStatusBar.post(e.getTool(), "");
	
      }
    
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

  /**
   * TableDataListener interface */
  public void valueChanged(int index)
  {
    itsCenterPanel.singlePaint(index);
  }

  /**
   * Sets the "hollow" representation mode */
  void hollow()
  {
    itsTableRenderer.setMode(TableRenderer.HOLLOW);
    repaint();
  }

  /** 
   * Sets the "solid" representation mode */
  void solid()
  {
    itsTableRenderer.setMode(TableRenderer.SOLID);
    repaint();
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



  //--- Fields

  static Vector tools;
  final static int PANEL_WIDTH = 500;
  final static int PANEL_HEIGHT = 300;

  InfoPanel itsStatusBar;

  Scrollbar itsPositionControl;

  static Tool itsDefaultTool;
  Dimension size = new Dimension(PANEL_WIDTH, PANEL_HEIGHT);

  EditorToolbar tb;
  TableGraphicContext gc;
  TableRenderer itsTableRenderer;
  TableDataModel tm;
  TableDisplay itsCenterPanel;

  static boolean toolbarAnchored = true;
  Box toolbarPanel;

  static Dimension toolbarDimension = new Dimension(30, 200);

  static TablePanel instance;

  private static int SCROLLBAR_SIZE = 30;
 
  JLabel currentXZoom;
  JLabel currentYZoom;
}







