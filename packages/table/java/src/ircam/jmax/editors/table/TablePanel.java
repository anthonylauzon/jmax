//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell
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

    //... the center panel
    prepareCenterPanel();

    //... the Graphic context (and the selection)
    gc = new TableGraphicContext(tm);
    prepareGraphicContext();

    //... the panel that will contain the toolbar
    prepareToolbarPanel();


    //... the renderer
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
    gc.getSelection().addListSelectionListener( new ListSelectionListener() {
      public void valueChanged(ListSelectionEvent e)
	{
	  if (e.getValueIsAdjusting())
	      return;

	  itsCenterPanel.repaint();
	}
    });

    //the repaints due to selection's ownership change are
    //handled in the SelectionLayer level of the RenderManager
 
    initTools();
  }


  /**
   * Prepare the panel containing the toolbar AND the scale */
  private void prepareToolbarPanel()
  {
    // toolbarPanel is a Box containing the toolbar 
    // scalePanel is a Box containing the scale

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
    
    scalePanel = new ScalePanel();

    scalePanelFont = new Font(getFont().getName(), Font.BOLD, 10);
    scalePanel.setBorder(new EtchedBorder());
  }
  
  /** The scale panel on the left */
  class ScalePanel extends JPanel {
      
    ScalePanel()
    {
      addComponentListener( new ComponentAdapter() {
	public void componentResized(ComponentEvent e)
	  {
	    updateScale();
	    //repaint()??
	  }
      });

      gc.getAdapter().addYZoomListener(new ZoomListener() {
	public void zoomChanged(float zoom) 
	  {
	    updateScale();
	  }
      });

      setSize(scaleDimension);
    }

    public void updateScale()
    {
      computeScaleParameters();
      //repaint();??
    }
    public void paint(Graphics g)
    {
      if (step == 0 || Math.abs(startValue) > 36535) 
	return; //"emergency exit!!!
	  
      TableAdapter ta = gc.getAdapter();
      g.setColor(Color.black);

      g.setFont(scalePanelFont);
      
      for (int i = startValue; i > ta.getInvY(gc.getGraphicDestination().getSize().height); i-= step)
	{
	  g.drawString(""+i, 1, ta.getY(i));
	  g.drawLine(getWidth()-5, ta.getY(i), getWidth(), ta.getY(i));
	}
    }
    
    /** The main computing routine.
     * It is called when the window is resized, scrolled or zoomed*/
    void computeScaleParameters()
    {
      TableAdapter ta = gc.getAdapter();
      int range = ta.getInvY(0)-ta.getInvY(gc.getGraphicDestination().getSize().height);
      
      if (range < 10) 
	step = 5;
	  else if (range <50)
	    step = 10;
      else if (range <100)
	step = 25;
      else if (range <500)
	    step = 25;
      else if (range <1000)
	step = 50; 
      else if (range <10000)
	step = 100;
	  else 
	    {
	      startValue = (ta.getInvY(0)/1000)*1000; 
	      step = 500;
	    }
      
      startValue = (ta.getInvY(0)/step)*step;
      
    }

    public Dimension getMinimumSize()
    {
      return scaleDimension;
	}
    public Dimension getPreferredSize()
    {
      return scaleDimension;
    }

    //--- Fields
    int startValue = 0;
    int step = 0;
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

    JPanel mess = new JPanel();
    mess.setLayout(new BorderLayout());
    mess.add(BorderLayout.WEST, toolbarPanel);
    mess.add(BorderLayout.EAST, scalePanel);
    add(mess, BorderLayout.WEST);

    
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

    //**-- zoom increment listeners and controllers.
    // The 0.9, 1.9 constants are used to avoid rounding errors
    // when passing from the '1/n' to the 'n' form of the zoom values. 
    // (i.e. when we reach 100% zoom value coming from 50% or less)
    IncrementListener xil = new IncrementListener() {
      public void increment()
	{
	  TableAdapter a = gc.getAdapter();
	  if (a.getXZoom()>=0.9)
	    a.setXZoom(Math.round(a.getXZoom())+1);
	  else
	    a.setXZoom(a.getXZoom()*(1/(1-a.getXZoom())));

	}

      public void decrement()
	{
	  TableAdapter a = gc.getAdapter();
	  if (a.getXZoom()>1.9) 
	    a.setXZoom(Math.round(a.getXZoom())-1);
	  else
	    a.setXZoom(a.getXZoom()*(1/(1+a.getXZoom())));
	}
    };


    IncrementListener yil = new IncrementListener() {
      public void increment()
	{
	  TableAdapter a = gc.getAdapter();
	  if (a.getYZoom()>0.9)
	    a.setYZoom(Math.round(a.getYZoom())+1);
	  else
	    a.setYZoom(a.getYZoom()*(1/(1-a.getYZoom())));

	}

      public void decrement()
	{
	  TableAdapter a = gc.getAdapter();
	  if (a.getYZoom() > 1.9 )
	    a.setYZoom(Math.round(a.getYZoom())-1);
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
 
    itsCenterPanel.setBackground(Color.white);
    itsCenterPanel.setDoubleBuffered(true);
    itsCenterPanel.setBounds(toolbarDimension.width+scaleDimension.width, 
			     InfoPanel.INFO_WIDTH, 
			     getSize().width-toolbarDimension.width-scaleDimension.width-SCROLLBAR_SIZE, 
			     getSize().height-InfoPanel.INFO_HEIGHT);

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
	  scalePanel.updateScale(); //this adapter informs the scale... (?!)
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
  ScalePanel scalePanel;
  Font scalePanelFont;

  static Dimension toolbarDimension = new Dimension(30, 200);
  static Dimension scaleDimension = new Dimension(30, 200);

  static TablePanel instance;

  private static int SCROLLBAR_SIZE = 30;
 
  JLabel currentXZoom;
  JLabel currentYZoom;
}







