package ircam.jmax.editors.table;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;

import java.awt.*;
import java.util.*;
import java.awt.event.*;
import java.io.*;
import tcl.lang.*;
import com.sun.java.swing.*;

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

    tools = new Vector();

    //-- prepares the NORTH Status bar 
    // WARNING:
    // make a superclass! Explode.ScrPanel is very similar
    JPanel northSection = new JPanel();
    northSection.setLayout(new BoxLayout(northSection, BoxLayout.Y_AXIS));
    
    itsStatusBar = new InfoPanel();
    
    itsStatusBar.setSize(300, 20);

    northSection.add(itsStatusBar);

    add(northSection, BorderLayout.NORTH);
    
    //prepares the center panel (the sensible area)
    itsCenterPanel = new CenterPanel();
 
    itsCenterPanel.setBounds(10, itsStatusBar.getSize().height+10, PANEL_WIDTH, PANEL_HEIGHT);

    itsCenterPanel.setBackground(Color.white);
    itsCenterPanel.setDoubleBuffered(true);

    add(itsCenterPanel, BorderLayout.CENTER);

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

    // prepares the vertical position controller
    itsPositionControl = new Scrollbar(Scrollbar.VERTICAL, 128, 100, -2000, 2000);

    itsPositionControl.addAdjustmentListener( new AdjustmentListener() {
      public void adjustmentValueChanged( AdjustmentEvent e)
	{
	  gc.getAdapter().setOY(256-e.getValue());

	  repaint();
	}
    });

    add(itsPositionControl, BorderLayout.EAST);

    tm.addListener(this);

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

    // create the unique instance of toolbar if it does not exist already
    if (Tabler.toolbar == null) {
      tb = new EditorToolbar(this, EditorToolbar.VERTICAL);
    }
    else tb = Tabler.toolbar;
    
    // add itself as a client of the toolbar
    linkToToolbar(tb);

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
	  repaint();
	}
      
    };

    c.setLayout(new BorderLayout());
    c.setOpaque(false);
    
    c.setSize(30, 200);
    
    // idiosynchrasies of the JToolbar class:
    // can't add directly a JPanel (jp) containing the JToolbar in a side
    // of the main container, because unanchor the toolbar will result
    // in emptying jp, resizing the main container, jp will desappear,
    // and the JToolbar cannot be anchored again.
    // must put jp in another panel instead
    JPanel aPanel = new JPanel();
    aPanel.setSize(30, 200);
    aPanel.setLayout(new BorderLayout());
    aPanel.add(c, BorderLayout.CENTER);

    add(aPanel, BorderLayout.WEST);

    
    // in case the toolbar is anchored, a new window "steals" it 
    // from the preceding owner.
    // the c.add() call invokes indirectly the addImpl method of the toolbar's JPanel.
    // PROBLEM for developers: when the Toolbar changes owner, there's
    // no way to anchor it in the preceding owner. Why? Is there a UI method
    // to do this? Note anyway that this is a non standard behaviour..
    if ( toolbarAnchored)
      {
	tb.setSize(30, 200);
	c.add(tb, BorderLayout.CENTER);
      }

 
    return tb;
  }

  /**
   * link this panel to a pre-existing toolbar */
  void linkToToolbar(EditorToolbar t)
  {
    t.addClient(gc);
    t.addToolListener(this);
    itsStatusBar.post(t.getTool(), "");
  } 


  /**
   * prepares the tools that will be used with this editor.
   * 
   */
  private void initTools() 
  {

    String fs = File.separator;
    String path = MaxApplication.getProperty("root")+fs+"packages/table/images"+fs;

    itsDefaultTool = new TableSelecter(new ImageIcon(path+"table_selecter.gif"));

    tools.addElement( itsDefaultTool);
    tools.addElement( new PencilTool(new ImageIcon(path+"pencil.gif")));
    tools.addElement( new LinerTool(new ImageIcon(path+"liner.gif")));
    //tools.addElement( new TableCutter(new ImageIcon(path+"cesors.gif")));

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

  Vector tools;
  final static int PANEL_WIDTH = 500;
  final static int PANEL_HEIGHT = 300;
  InfoPanel itsStatusBar;

  Scrollbar itsPositionControl;

  Tool itsDefaultTool;
  Dimension size = new Dimension(PANEL_WIDTH, PANEL_HEIGHT);

  EditorToolbar tb;
  TableGraphicContext gc;
  CenterPanel itsCenterPanel;
  static boolean toolbarAnchored = true;

  ZoomWidget itsXZoom;
  ZoomWidget itsYZoom;
  static TablePanel instance;

  public static int INITIAL_X_ZOOM = 2;
  public static int INITIAL_Y_ZOOM = 1;
  public static int INITIAL_Y_ORIGIN = 128;
}







