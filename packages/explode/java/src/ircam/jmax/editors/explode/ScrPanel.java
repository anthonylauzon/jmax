package ircam.jmax.editors.explode;

import com.sun.java.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;

  /**
   * The actual panel of a score editor. It contains the "sensible" part.
   * In a given moment, the graphic representation is handled by 
   * a "renderer", that has the responsability to actually draw the score.
   * The simplest renderer is "ScoreRenderer", a musical score-looking component.
   * The user interaction is handled by the tools. 
   * 
   */
public class ScrPanel extends JPanel implements ExplodeDataListener, ToolbarProvider, ToolListener, AdapterProvider, RenderProvider{
  
  public final int PANEL_WIDTH = 800;
  public final int PANEL_HEIGHT = 400;
  int oldX = 0;
  int oldY = 0;
  
  Adapter itsAdapter;
  ExplodeDataModel itsExplodeDataModel;
  ExplodeSelection itsSelection;
  Renderer itsRenderer;
  Vector tools = new Vector();

  int logicalTime = 0;

  Dimension size = new Dimension(PANEL_WIDTH, PANEL_HEIGHT);
  ScrTool currentTool = null;
  Scrollbar itsTimeScrollbar;
  Scrollbar itsTimeZoom;
  Label itsLabel;  

  /**
   * Creator. 
   */
  public ScrPanel(ExplodeDataModel ep) {
    
    setSize(PANEL_WIDTH, PANEL_HEIGHT);
    setLayout(new BorderLayout());
    setBackground(Color.white);
    setExplodeDataModel(ep);
    setAdapter(new PartitionAdapter());
    itsSelection = new ExplodeSelection(itsExplodeDataModel, itsAdapter);
    ((PartitionAdapter) itsAdapter).setXZoom(20);// just a try
    ((PartitionAdapter) itsAdapter).setYZoom(300);// just a try
    ((PartitionAdapter) itsAdapter).setYInvertion(true);// just a try
    ((PartitionAdapter) itsAdapter).setYTransposition(115);// just a try
    itsRenderer = new ScoreRenderer(this, ep, this, itsSelection);
    itsExplodeDataModel.addListener(this);
    
    
    itsLabel = new Label();
    itsLabel.setBounds(0, 0, 200, 30);
    add(itsLabel, BorderLayout.NORTH);

    itsTimeZoom = new Scrollbar(Scrollbar.VERTICAL, 20, 40, 10, 1000);
    itsTimeZoom.addAdjustmentListener(new AdjustmentListener() {
      public void adjustmentValueChanged(AdjustmentEvent e) {
	((PartitionAdapter) itsAdapter).setXZoom(e.getValue());// just a try
	repaint();
      }
    });


    int totalTime = 1000;
    if (itsExplodeDataModel.length() != 0){
      totalTime = itsExplodeDataModel.getEventAt(itsExplodeDataModel.length()-1).getTime();
    }
    itsTimeScrollbar = new Scrollbar(Scrollbar.HORIZONTAL, 0, 1000, 0, totalTime);
    
    //WARN: setUnitIncrement seems not to work, while setBlockIncrement do
    itsTimeScrollbar.setUnitIncrement(windowTimeWidth()/10);
    itsTimeScrollbar.setBlockIncrement(windowTimeWidth());

    itsTimeScrollbar.addAdjustmentListener(new AdjustmentListener() {
      public void adjustmentValueChanged(AdjustmentEvent e) {
	
	logicalTime = e.getValue();
	int maxIndex = itsExplodeDataModel.length();
	
	int first = itsExplodeDataModel.indexOfLastEventEndingBefore(logicalTime)+1;
	int last = itsExplodeDataModel.indexOfFirstEventStartingAfter(logicalTime+windowTimeWidth())-1;


	if (first != -1) 
	  {
	    ((PartitionAdapter)itsAdapter).setXTransposition(-logicalTime);
	    itsRenderer.render(getGraphics(), first, last);
	  }
	
	itsLabel.setText("starting time: "+logicalTime+"msec"+"                 zoomfactor"+itsTimeZoom.getValue()+"%");
      }
    });
    add(itsTimeScrollbar, BorderLayout.SOUTH);
    
    add(itsTimeZoom, BorderLayout.EAST);
    
    //----
    //prepare the tools & the toolbar...
    initTools();

    ScrToolbar aToolbar = new ScrToolbar(this);
    aToolbar.addToolListener(this);

    JFrame aFrame = new JFrame("tools");    
    aFrame.getContentPane().add(aToolbar);
    
    aFrame.pack();
    aFrame.setVisible(true);
    
  }
  

  /**
   * returns the current ExplodeDataModel for this panel
   */
  public ExplodeDataModel getExplodeDataModel() {
    return itsExplodeDataModel;
  }

  /**
   * set the ExplodeDataModel
   */
  public void setExplodeDataModel(ExplodeDataModel theProvider) {
    itsExplodeDataModel = theProvider;
  }


  /**
   * returns the current graphic renderer for this score panel.
   */
  public Renderer getRenderer() {
    return itsRenderer;
  }

  /**
   * sets the graphic renderer to be used for this score panel
   */
  public void setRenderer(Renderer theRenderer) {
    itsRenderer = theRenderer;
  }


  /**
   * returns the data accessor (parameter mapping) for this score panel
   */
  public Adapter getAdapter() {
    return itsAdapter;
  }

  /**
   * set the data accessor to be used with this score panel
   */
  public void setAdapter(Adapter theAdapter) {
    itsAdapter = theAdapter;
  }
 

  /**
   * prepares the tools that will be used with this editor,
   * and activate the default tool (the selecter)
   */
  private void initTools() {
    currentTool = new ArrowTool(this, this, itsSelection);
    tools.addElement(new ScrAddingTool(this, this, itsExplodeDataModel));
    tools.addElement(currentTool);

    currentTool.activate();

  }

  public Enumeration getTools() {
    
    return tools.elements();
  }

  /**
   * Callback from the toolbar that communicates that a new tool have been
   * selected by the user
   */ 
  public void toolChanged(ToolChangeEvent e) {

    if (currentTool != null) {
      currentTool.deactivate();
    }
      
    e.getTool().activate();
    
    currentTool = e.getTool();
    repaint();

  }



  public Enumeration getEvents() {
    return itsExplodeDataModel.getEvents();
  }

  /**
   * called when the database is changed
   */
  public void dataChanged(Object spec) {
    repaint();
  }

  
  public void update(Graphics g) {}
  /* avoid to paint gray (and then paint white)*/

  /**
   * The paint method.
   * Delegated to the current Renderer
   */
  public void paint(Graphics g) {
  
    int temp1 = itsExplodeDataModel.indexOfFirstEventStartingAfter(logicalTime);
   
    int temp2 = itsExplodeDataModel.indexOfLastEventEndingBefore(logicalTime+windowTimeWidth());
    itsRenderer.render(g, temp1, temp2);
  }

  public ExplodeSelection getSelection() {
    return itsSelection;
  }

  int windowTimeWidth() {
    //return (int)(PANEL_WIDTH/(((PartitionAdapter)itsAdapter).getXZoom()));
    return (int)(getSize().width/(((PartitionAdapter)itsAdapter).getXZoom()));
  }

  /**
   * from Panel class...
   */
  public Dimension getMinimumSize() {
    
    return size;
    
  }    
  
  /**
   * from Panel class...
   */
  public Dimension getPreferredSize() {
    
    return getMinimumSize();	
    
  }
  
}




