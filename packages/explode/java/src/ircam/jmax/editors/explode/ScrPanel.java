package ircam.jmax.editors.explode;

import com.sun.java.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.io.*;

import ircam.jmax.*;
import ircam.jmax.toolkit.*;

  /**
   * The actual panel of a score editor. It contains the "sensible" part.
   * The graphic representation is handled by 
   * a Renderer that has the responsability to actually draw the score.
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

    tools = new Vector();

    //-- prepares the NORTH infoPanel
    
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
      public void update(Graphics g) {}

      public void paint(Graphics g) 
	{
	  
	  int currentTime = gc.getLogicalTime();
	  int temp1 = gc.getDataModel().indexOfFirstEventEndingAfter(currentTime);
	  int temp2 = gc.getDataModel().indexOfLastEventStartingBefore(currentTime+windowTimeWidth());

	  gc.getRenderer().render(g, temp1, temp2);	
	}

      protected void processMouseEvent(MouseEvent e)
	{
	  if (e.isPopupTrigger()) 
	    ScrToolbar.getToolbar().itsPopupMenu.show (e.getComponent(), e.getX(), e.getY());
	  
	  else 
	    super.processMouseEvent(e);
	  
	}
    };

    itsScore.setBounds(0, itsStatusBar.getSize().height, PANEL_WIDTH, PANEL_HEIGHT);
    add(itsScore, BorderLayout.CENTER);

    { //-- prepares the graphic context
      gc = new GraphicContext();
      gc.setGraphicSource(itsScore);
      gc.setGraphicDestination(itsScore);
      //gc.setFrame(GraphicContext.getFrame(this));
      gc.setDataModel(ep);
      ExplodeSelection.createSelection(ep);
      gc.setRenderer(new ScoreRenderer(gc));
      gc.setLogicalTime(0);
      gc.setStatusBar(itsStatusBar);
    }

    ExplodeSelection.getSelection().addSelectionListener(this);
    gc.getDataModel().addListener(this);

    
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

	gc.getStatusBar().post(ScrPanel.this, "starting time: "+currentTime+"msec"+"                 zoomfactor"+itsTimeZoom.getValue()+"%");
	
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
   * The ScrToolbar class uses the Frame information
   * in order to correctly map tools on windows
   */
  public void prepareToolbar() 
  {

    gc.setFrame(GraphicContext.getFrame(this));

    ScrToolbar.createToolbar(this, gc);
    ScrToolbar.getToolbar().addToolListener(this);
  }



  /**
   * prepares the tools that will be used with this editor,
   * and activate the default tool (the selecter)
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
  public ScrTool getDefaultTool() 
  {
    return itsDefaultTool;
  }

  /**
   * Callback from the toolbar when a new tool have been
   * selected by the user
   */ 
  public void toolChanged(ToolChangeEvent e) 
  {
    if (ScrToolbar.getTool() != null) 
      {
	gc.getStatusBar().post(ScrToolbar.getTool(), "");
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


  /* avoid to paint the white background twice */  
  public void update(Graphics g) {}


  /**
   * The paint method.
   * Delegated to the current Renderer
   */
  public void paint(Graphics g) 
  {

    super.paint(g);

  }

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
    return "Main Editor";
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
  public final int PANEL_WIDTH = 800;
  public final int PANEL_HEIGHT = 400;
  
  GraphicContext gc;

  Vector tools;
  ScrToolbar itsToolbar;

  ScrTool itsDefaultTool;

  Dimension size = new Dimension(PANEL_WIDTH, PANEL_HEIGHT);
  
  public final int INITIAL_ZOOM = 20;
  Scrollbar itsTimeScrollbar;
  Scrollbar itsTimeZoom;
  JLabel itsZoomLabel;

  InfoPanel itsStatusBar;
  JPanel itsScore;
}






