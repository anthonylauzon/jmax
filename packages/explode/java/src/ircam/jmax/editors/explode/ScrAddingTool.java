package ircam.jmax.editors.explode;

import java.awt.event.*;
import java.awt.*;
import com.sun.java.swing.ImageIcon;

/**
 * The tool used to add an event in the score.
 * It uses just one user-interaction module:
 * a mouse tracker to choose the position.
 */ 
public class ScrAddingTool extends ScrTool implements PositionListener {

  AdapterProvider itsAdapterProvider;
  ExplodeDataModel itsProvider;
  MouseTracker itsMouseTracker;
  Component itsGraphicSource;

  /**
   * Constructor. It needs to know the graphic source of events,
   * the Adapter, the database.
   */
  public ScrAddingTool(Component graphicSource, AdapterProvider theAdapterProvider, ExplodeDataModel theProvider) {
    super("adder", new ImageIcon("/u/worksta/maggi/projects/max/images/tool_slider.gif"));

    itsMouseTracker = new MouseTracker(this, graphicSource);
    itsAdapterProvider = theAdapterProvider;
    itsGraphicSource = graphicSource;
    itsProvider = theProvider;
  }

  /**
   * called when this tool becomes the active tool
   */
  public void activate() {
    mountIModule(itsMouseTracker);
  }

  public void deactivate() {}

  
  //----------- PositionListener interface ------------
  
  public void positionChoosen(int x, int y) {
    ScrEvent aEvent = new ScrEvent();
    itsAdapterProvider.getAdapter().XMapper.set(aEvent, x);
    itsAdapterProvider.getAdapter().YMapper.set(aEvent, y);

    itsProvider.addEvent(aEvent);
  } 
  
}



