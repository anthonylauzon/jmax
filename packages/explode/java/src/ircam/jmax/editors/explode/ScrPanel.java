package ircam.jmax.editors.explode;

import com.sun.java.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;

  /**
   * The actual panel of a score editor. It contains the "sensible" part.
   * In a given moment, the graphic representation is handled by 
   * a Renderer that has the responsability to actually draw the score.
   * The simplest renderer is "ScoreRenderer", a musical score-looking component.
   * The user interaction is handled by the tools.
   * The panel builds also the Graphic context to be used during edit, 
   * and the Toolbar. 
   */
public class ScrPanel extends JPanel implements ExplodeDataListener, ToolbarProvider, ToolListener{
  
  /**
   * Constructor based on a ExplodeDataModel. 
   */
  public ScrPanel(ExplodeDataModel ep) 
  {  
    setSize(PANEL_WIDTH, PANEL_HEIGHT);
    setLayout(new BorderLayout());
    setBackground(Color.white);

    tools = new Vector();

    { //-- prepares the graphic context
      gc = new GraphicContext();
      gc.setGraphicEventSource(this);
      gc.setGraphicDestination(this);
      gc.setDataModel(ep);
      gc.setAdapter(new PartitionAdapter());
      gc.setSelection(new ExplodeSelection(ep));
      gc.setRenderer(new ScoreRenderer(gc));
    }

    gc.getDataModel().addListener(this);

    {//-- prepares the parameters for the adapter
      ((PartitionAdapter) gc.getAdapter()).setXZoom(20);// just a try
      ((PartitionAdapter) gc.getAdapter()).setYZoom(300);// just a try
      ((PartitionAdapter) gc.getAdapter()).setYInvertion(true);// just a try
      ((PartitionAdapter) gc.getAdapter()).setYTransposition(115);// just a try
    }
    
    //-- prepares the NORTH label
    itsLabel = new Label();
    itsLabel.setBounds(0, 0, 200, 30);
    add(itsLabel, BorderLayout.NORTH);

    //-- prepares the EAST scrollbar (time stretching) and its listener
    itsTimeZoom = new Scrollbar(Scrollbar.VERTICAL, 20, 40, 10, 1000);
    itsTimeZoom.addAdjustmentListener(new AdjustmentListener() {
      
      public void adjustmentValueChanged(AdjustmentEvent e) {
	((PartitionAdapter) gc.getAdapter()).setXZoom(e.getValue());// just a try
	repaint();
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
	
	logicalTime = e.getValue();
	
	int firstIndex = gc.getDataModel().indexOfLastEventEndingBefore(logicalTime)+1;
	int lastIndex = gc.getDataModel().indexOfFirstEventStartingAfter(logicalTime+windowTimeWidth())-1;


	if (firstIndex != -1) 
	  {
	    ((PartitionAdapter)gc.getAdapter()).setXTransposition(-logicalTime);
	    gc.getRenderer().render(getGraphics(), firstIndex, lastIndex);
	  }
	
	itsLabel.setText("starting time: "+logicalTime+"msec"+"                 zoomfactor"+itsTimeZoom.getValue()+"%");
      }
    });

    add(itsTimeScrollbar, BorderLayout.SOUTH);    
    add(itsTimeZoom, BorderLayout.EAST);
    
    //---- prepare the tools & the toolbar...
    initTools();

    ScrToolbar aToolbar = new ScrToolbar(this);
    aToolbar.addToolListener(this);

    JFrame aFrame = new JFrame("tools");    
    aFrame.getContentPane().add(aToolbar);
    
    aFrame.pack();
    aFrame.setVisible(true);
    
  }
  

  /**
   * prepares the tools that will be used with this editor,
   * and activate the default tool (the selecter)
   */
  private void initTools() 
  {
    gc.setTool( new ArrowTool(gc));
    
    tools.addElement(gc.getTool());
    tools.addElement(new ScrAddingTool(gc));
    tools.addElement(new DeleteTool(gc));
    tools.addElement(new MoverTool(gc, 
				   new ImageIcon("/u/worksta/maggi/projects/max/packages/explode/images/vmover.gif"), 
				   MoverTool.VERTICAL_MOVEMENT));
    tools.addElement(new MoverTool(gc, 
				   new ImageIcon("/u/worksta/maggi/projects/max/packages/explode/images/hmover.gif"), 
				   MoverTool.HORIZONTAL_MOVEMENT));

    gc.getTool().activate();
  }


  /**
   * ToolbarProvider interface
   */
  public Enumeration getTools() 
  {  
    return tools.elements();
  }


  /**
   * Callback from the toolbar when a new tool have been
   * selected by the user
   */ 
  public void toolChanged(ToolChangeEvent e) 
  {
    if (gc.getTool() != null) 
      {
	gc.getTool().deactivate();
      }
    
    e.getTool().activate();
    
    gc.setTool(e.getTool());
    repaint();
  }


  /**
   * called when the database is changed
   */
  public void dataChanged(Object spec) 
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
    int temp1 = gc.getDataModel().indexOfFirstEventStartingAfter(logicalTime);
    int temp2 = gc.getDataModel().indexOfLastEventEndingBefore(logicalTime+windowTimeWidth());
    
    gc.getRenderer().render(g, temp1, temp2);
  }

  /**
   * get the lenght (in milliseconds) of the window
   */
  int windowTimeWidth() 
  {
    return (int)(getSize().width/(((PartitionAdapter)gc.getAdapter()).getXZoom()));
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

  int logicalTime = 0;

  Dimension size = new Dimension(PANEL_WIDTH, PANEL_HEIGHT);
  
  Scrollbar itsTimeScrollbar;
  Scrollbar itsTimeZoom;
  Label itsLabel;  

}






