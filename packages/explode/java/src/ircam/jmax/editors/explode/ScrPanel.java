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
public class ScrPanel extends JPanel implements ExplodeDataListener, ToolbarProvider, ToolListener, AdapterProvider{
  
  public final int PANEL_WIDTH = 800;
  public final int PANEL_HEIGHT = 400;
  int oldX = 0;
  int oldY = 0;
  
  Adapter itsAdapter;
  ExplodeDataModel itsExplodeDataModel;
  Renderer itsRenderer;
  Vector tools = new Vector();

  int logicalTime = 0;

  Dimension size = new Dimension(PANEL_WIDTH, PANEL_HEIGHT);
  ScrTool currentTool = null;
  

  /**
   * Creator. 
   */
  public ScrPanel(ExplodeDataModel ep) {
    
    setLayout(new BorderLayout());
    setExplodeDataModel(ep);
    setAdapter(new PartitionAdapter()); 
    ((PartitionAdapter) itsAdapter).setXZoom(20);// just a try
    ((PartitionAdapter) itsAdapter).setYZoom(300);// just a try
    ((PartitionAdapter) itsAdapter).setYInvertion(true);// just a try
    ((PartitionAdapter) itsAdapter).setYTransposition(122);// just a try
    itsRenderer = new ScoreRenderer(this, ep, this);
    itsExplodeDataModel.addListener(this);
    
    Scrollbar aScrollbar = new Scrollbar(Scrollbar.HORIZONTAL, 0, 1000, 0, itsExplodeDataModel.getEventAt(itsExplodeDataModel.length()-1).getTime());
    
    aScrollbar.setUnitIncrement(1000);
    aScrollbar.addAdjustmentListener(new AdjustmentListener() {
      public void adjustmentValueChanged(AdjustmentEvent e) {
	logicalTime = e.getValue();
	System.err.println("moving to ms "+logicalTime);
	int temp = itsExplodeDataModel.indexOfFirstEventAfter(logicalTime);
	((PartitionAdapter)itsAdapter).setXTransposition(-logicalTime);
	itsRenderer.render(getGraphics(), temp, temp+10);
      }
    });
    add(aScrollbar, BorderLayout.SOUTH);

    //----
    //prepare the toolbar...
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
   * prepares the tools that will be used with this editor
   */
  private void initTools() {
    tools.addElement(new ScrAddingTool(this, this, itsExplodeDataModel));
    tools.addElement(new ScrSelectingTool(this));

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

  /**
   * The paint method.
   * Delegated to the current Renderer
   */
  public void paint(Graphics g) {
    int temp = itsExplodeDataModel.indexOfFirstEventAfter(logicalTime);
    itsRenderer.render(g, temp, temp+10);    
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


