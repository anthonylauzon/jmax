package ircam.jmax.editors.explode;

import com.sun.java.swing.*;
import com.sun.java.swing.plaf.*;
import com.sun.java.swing.undo.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.io.*;

import ircam.jmax.*;
import ircam.jmax.toolkit.*;

  /**
   * The actual panel of a score editor. It contains the "sensible" part.
   * The graphic representation is handled by 
   * a RendererManager that has the responsability to actually draw the score.
   * The simplest renderer is "ScoreRenderer", a piano-roll component.
   * The user interaction is handled by the tools.
   * The panel builds also the Graphic context to be used during edit, 
   * and the Toolbar. 
   */
public class ScrPanel extends JPanel implements ExplodeDataListener, ToolbarProvider, ToolListener, SelectionListener, StatusBarClient{
  
  /**
   * Constructor based on a ExplodeDataModel. 
   */
  public ScrPanel(ExplodeDataModel ep) 
  {  
    setSize(PANEL_WIDTH, PANEL_HEIGHT);
    setLayout(new BorderLayout());
    setBackground(Color.white);
    setDoubleBuffered(false);

    tools = new Vector();

    //-- prepares the NORTH Status bar
    
    JPanel northSection = new JPanel();
    northSection.setLayout(new BoxLayout(northSection, BoxLayout.Y_AXIS));
    
    itsStatusBar = new InfoPanel();
    itsStatusBar.setSize(300, 20);

    northSection.add(itsStatusBar);

    add(northSection, BorderLayout.NORTH);

    //-- prepares the CENTER panel (the score)
    // a simple panel, that just uses a Renderer as its paint method...
    // it takes care of the popup showing.
    // NOTE: the popup cannot be shown from a simple listener, since
    // being a MouseListener for this panel does not prevent the panel
    // itself (and then the current tool) to receive the same events.
    itsScore = new JPanel() {
      //public void update(Graphics g) {}

      public void paint(Graphics g) 
	{
	  Rectangle r = g.getClipBounds();
	  gc.getRenderManager().render(g, r); //et c'est tout	
	}

       protected void processMouseEvent(MouseEvent e)
	{
	  if (e.isPopupTrigger()) 
	    {
	      Explode.toolbar.itsPopupMenu.show (e.getComponent(), e.getX()-10, e.getY()-10);
	    }
	  else 
	    super.processMouseEvent(e);
	  
	}
    };

    itsScore.setBounds(0, itsStatusBar.getSize().height, PANEL_WIDTH, PANEL_HEIGHT);
    itsScore.setDoubleBuffered(false);
    add(itsScore, BorderLayout.CENTER);

    { //-- prepares the graphic context
      gc = new ExplodeGraphicContext();
      gc.setGraphicSource(itsScore);
      gc.setGraphicDestination(itsScore);

      gc.setDataModel(ep);
      ExplodeSelection.createSelection(ep);
      gc.setRenderManager(new ScoreRenderer(gc));
      gc.setLogicalTime(0);
      gc.setStatusBar(itsStatusBar);

    }

    ExplodeSelection.getSelection().addSelectionListener(this);
    gc.getDataModel().addListener(this);

    itsStatusBar.addWidget(new ScrEventWidget(gc));

    //-- prepares the zoom scrollbar (time stretching) and its listeners
    itsTimeZoom = new Scrollbar(Scrollbar.HORIZONTAL, INITIAL_ZOOM, 5, 1, 1000);

    itsZoomLabel = new JLabel("Zoom: "+INITIAL_ZOOM+"%");
    
    gc.getAdapter().addZoomListener( new ZoomListener() {
      public void zoomChanged(int zoom)
	{
	  itsZoomLabel.setText("Zoom: "+zoom+"%"); 

	}
    });


    itsTimeZoom.addAdjustmentListener(new AdjustmentListener() {
      
      public void adjustmentValueChanged(AdjustmentEvent e) {
	
	gc.getAdapter().setXZoom(e.getValue());
	itsTimeScrollbar.setVisibleAmount(windowTimeWidth()/2);
	itsScore.repaint();
      }
      
    });


    //-- prepares the SOUTH scrollbar (time scrolling) and its listener
    int totalTime = 1000;
    if (gc.getDataModel().length() != 0)
      {
	totalTime = gc.getDataModel().getEventAt(gc.getDataModel().length()-1).getTime();
      }

    itsTimeScrollbar = new Scrollbar(Scrollbar.HORIZONTAL, 0, 1000, 0, totalTime);
    itsTimeScrollbar.setUnitIncrement(windowTimeWidth()/10);//WARN: setUnitIncrement seems not to work
    itsTimeScrollbar.setBlockIncrement(windowTimeWidth());


    itsTimeScrollbar.addAdjustmentListener(new AdjustmentListener() {
    
      public void adjustmentValueChanged(AdjustmentEvent e) {
	
	int currentTime = e.getValue();
	gc.setLogicalTime(currentTime);
	
	gc.getAdapter().setXTransposition(-currentTime);
	itsScore.repaint();

	itsStatusBar.post(ScrPanel.this, "starting time: "+currentTime+"msec"+" zoomfactor"+itsTimeZoom.getValue()+"%");
	
      }
    });

    JPanel aSliderPanel = new JPanel();
    aSliderPanel.setLayout(new ProportionalLayout(ProportionalLayout.X_AXIS, (float) 0.75));
    aSliderPanel.add("", itsTimeScrollbar);
    Box aZoomBox = new Box(BoxLayout.X_AXIS);
    aZoomBox.add(itsZoomLabel);
    aZoomBox.add(itsTimeZoom);

    aSliderPanel.add("", aZoomBox);

    add(aSliderPanel, BorderLayout.SOUTH);
    

    initTools();
    
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
    if (Explode.toolbar == null) {
      tb = new EditorToolbar(this);
    }
    else tb = Explode.toolbar;
    
    // add itself as a client of the toolbar
    linkToToolbar(tb);

    // prepare the Panel containing the toolbar when anchored
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
	  ScrPanel.toolbarAnchored = true;
	}

      // the tolbar is going to be unanchored
      public void remove(Component c)
	{
	  super.remove(c);
	  ScrPanel.toolbarAnchored = false;
	  repaint();
	}
      
    };

    c.setLayout(new BorderLayout());
    c.setOpaque(false);
    
    c.setSize(200, 30);
    
    itsStatusBar.addWidgetAt(c, 2);

    // in case the toolbar is anchored, a new window "steals" it 
    // from the preceding owner.
    // the c.add() call invokes indirectly the addImpl method of the toolbar's JPanel.
    // PROBLEM for (future?) developers: when the Toolbar changes owner, there's
    // no way to anchor it in the preceding owner. Why? Is there a UI method
    // to do this? Note anyway that this is a non standard behaviour..
    if ( toolbarAnchored)
      {
	tb.setSize(200, 30);
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
    String path = MaxApplication.getProperty("root")+fs+"packages/explode/images"+fs;

    itsDefaultTool = new ArrowTool(gc, new ImageIcon(path+"selecter.gif"));

    tools.addElement( itsDefaultTool);

    tools.addElement(new ScrAddingTool(gc,  new ImageIcon(path+"adder.gif")));
    tools.addElement(new DeleteTool(gc, new ImageIcon(path+"eraser.gif")));
    tools.addElement(new MoverTool(gc, new ImageIcon(path+"vmover.gif"), 
				   MoverTool.VERTICAL_MOVEMENT));
    tools.addElement(new MoverTool(gc, new ImageIcon(path+"hmover.gif"), 
				   MoverTool.HORIZONTAL_MOVEMENT));
    tools.addElement(new ResizerTool(gc, new ImageIcon(path+"resizer.gif")));
    tools.addElement(new ZoomTool(gc, new ImageIcon(path+"zoomer.gif")));

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
   * called when the database is changed
   */

  public void objectChanged(Object spec) 
  {
    repaint();
  }

  public void objectAdded(Object spec) 
  {
    repaint();
  }

  public void objectDeleted(Object whichObject) 
  {
    repaint();
  }

  /**
   * SelectionListener interface
   */

  public void selectionChanged()
  {  

    repaint();
  }


  /* avoid to paint the white background twice*/   
  public void update(Graphics g) {}


  /**
   * The paint method.
   * Delegated to the current Renderer
   */
  /*public void paint(Graphics g) 
  {
    super.paint(g);
  }*/

  /**
   * get the lenght (in milliseconds) of the window
   */
  int windowTimeWidth() 
  {
    return (int)(itsScore.getSize().width/(gc.getAdapter().getXZoom()));
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
   * invoke the Adapter's editor
   */
  public void settings() {
    gc.getAdapter().edit(gc.getFrame());
  }


  /** Undo support */
  public void undo()
  {
    try 
      {
	gc.getDataModel().undo();
      } catch (CannotUndoException e1) {
	System.out.println("can't undo");
      }
  }

  /** undo support */
  protected void redo()
  {
    try 
      {
	gc.getDataModel().redo();
      } catch (CannotRedoException e1) {
	System.out.println("can't redo");
      }
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
  

  //------------- Fields
  public static final int PANEL_WIDTH = 800;
  public static final int PANEL_HEIGHT = 450;
  
  ExplodeGraphicContext gc;

  Vector tools;
  EditorToolbar itsToolbar;

  Tool itsDefaultTool;

  Dimension size = new Dimension(PANEL_WIDTH, PANEL_HEIGHT);
  
  public final int INITIAL_ZOOM = 20;
  Scrollbar itsTimeScrollbar;
  Scrollbar itsTimeZoom;
  JLabel itsZoomLabel;

  EditorToolbar tb;
  static boolean toolbarAnchored = true;
  
  InfoPanel itsStatusBar;
  JPanel itsScore;

}






