package ircam.jmax.editors.table;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;

import java.awt.*;
import java.util.*;
import java.awt.event.*;
import java.io.*;
import tcl.lang.*;
import javax.swing.*;

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

    instance = this;
    setSize(PANEL_WIDTH, PANEL_HEIGHT);
    setLayout(new BorderLayout());
    setBackground(Color.white);
    setDoubleBuffered(true);


    //make the NORTH Status bar 
    prepareStatusBar();

    //... the center panel (the sensible area)
    prepareCenterPanel();

    //... the Graphic context (and the Renderer)
    prepareGraphicContext(tm);
    
    //... the vertical position controller
    prepareVerticalScrollbar();

    //... the widgets in the statusBar
    addWidgets();

    tm.addListener(this);

    initTools();
  }


  /** the panel containing the table representation */
  class CenterPanel extends JPanel 
  {
    /**
     * The table's representation panel uses the TableRenderManager to
     * represent its data.*/
    public void paint(Graphics g) 
    {
      Rectangle r = g.getClipBounds();
      gc.getRenderManager().render(g, r); //et c'est tout	
    }
    
    /**
     * paints all the points in a given (closed) range */
    public void rangePaint(int index1, int index2)
    {
      Graphics g = getGraphics();
      TableRenderer tr = (TableRenderer) gc.getRenderManager();
      for (int i = index1; i<= index2; i++)
	tr.renderPoint(g, i);
      
      g.dispose();
    }
    
    /**
     * paints a single point */
    public void singlePaint(int index)
    {
      Graphics g = getGraphics();
      TableRenderer tr = (TableRenderer) gc.getRenderManager();
      tr.renderPoint(g, index);
      g.dispose();
    }

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
    
  }
  

  /** 
   * Set the initial parameters of the Adapter */
  private void setOriginAndZoom(TableAdapter ta)
  {
    ta.setYZoom(INITIAL_Y_ZOOM);
    ta.setXZoom(INITIAL_X_ZOOM);
    
    ta.setOY(INITIAL_Y_ORIGIN);

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
    
    toolbarPanel = new JPanel() {
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
    toolbarPanel.setLayout(new BorderLayout());
    toolbarPanel.add(c, BorderLayout.CENTER);

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
  

  private void addWidgets()
  {
    itsXZoom = new ZoomWidget("x Zoom", (int)(gc.getAdapter().getXZoom()*100), ZoomWidget.HORIZONTAL);
    itsYZoom = new ZoomWidget("y Zoom", (int)(gc.getAdapter().getYZoom()*100), ZoomWidget.VERTICAL);

    itsXZoom.setSize(150, InfoPanel.INFO_HEIGHT);
    itsYZoom.setSize(150, InfoPanel.INFO_HEIGHT);

    itsYZoom.getLessButton().addActionListener( new ActionListener() {
      public void actionPerformed(ActionEvent e)
	{
	  float  newValue = gc.getAdapter().getYZoom()/2;
	  gc.getAdapter().setYZoom(newValue);
	  itsYZoom.setValue((int)(newValue*100));
	  repaint();
	}
    });

    itsYZoom.getMoreButton().addActionListener( new ActionListener() {
      public void actionPerformed(ActionEvent e)
	{
	  float  newValue = gc.getAdapter().getYZoom()*2;
		  
	  gc.getAdapter().setYZoom(newValue);
	  itsYZoom.setValue((int) (newValue*100));
	  repaint();
	}
    });

    itsXZoom.getLessButton().addActionListener( new ActionListener() {
      public void actionPerformed(ActionEvent e)
	{
	  float newValue = gc.getAdapter().getXZoom()/2;

	  gc.getAdapter().setXZoom(newValue);
	  itsXZoom.setValue((int)(newValue*100));
	  repaint();
	}
    });

    itsXZoom.getMoreButton().addActionListener( new ActionListener() {
      public void actionPerformed(ActionEvent e)
	{
	  float newValue = gc.getAdapter().getXZoom()*2;
	  gc.getAdapter().setXZoom(newValue);
	  
	  itsXZoom.setValue((int)(newValue*100));
	  repaint();
	}
    });

    itsStatusBar.addWidget(itsYZoom);
    itsStatusBar.addWidget(itsXZoom);


  }

  private void prepareCenterPanel()
  {
    itsCenterPanel = new CenterPanel();
 
    itsCenterPanel.setBounds(10, itsStatusBar.getSize().height+10, PANEL_WIDTH, PANEL_HEIGHT);

    itsCenterPanel.setBackground(Color.white);
    itsCenterPanel.setDoubleBuffered(true);

    add(itsCenterPanel, BorderLayout.CENTER);
  }


  private void prepareGraphicContext(TableDataModel tm)
    { //prepares the graphic context
      gc = new TableGraphicContext(tm);
      gc.setGraphicSource(itsCenterPanel);
      gc.setGraphicDestination(itsCenterPanel);

      TableSelection.createSelection(tm);
      gc.setRenderManager(new TableRenderer(gc));
      gc.setStatusBar(itsStatusBar);
      TableAdapter ta = new TableAdapter();
      setOriginAndZoom(ta);
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
    ((TableRenderer)gc.getRenderManager()).setMode(TableRenderer.HOLLOW);
    repaint();
  }

  /** 
   * Sets the "solid" representation mode */
  void solid()
  {
    ((TableRenderer)gc.getRenderManager()).setMode(TableRenderer.SOLID);
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
  CenterPanel itsCenterPanel;

  static boolean toolbarAnchored = true;
  JPanel toolbarPanel;
  static Dimension toolbarDimension = new Dimension(40, 200);

  ZoomWidget itsXZoom;
  ZoomWidget itsYZoom;
  static TablePanel instance;

  public static int INITIAL_X_ZOOM = 2;
  public static int INITIAL_Y_ZOOM = 1;
  public static int INITIAL_Y_ORIGIN = 128;
}







