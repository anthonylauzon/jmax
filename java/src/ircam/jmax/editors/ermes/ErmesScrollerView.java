package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;

/**
 * The "view" that contains the scrollers AND the scrollable area.
 */
public class ErmesScrollerView extends ScrollPane {
  Panel view;
  Dimension imageSize;
  
  //--------------------------------------------------------
  //	CONSTRUCTOR
  //--------------------------------------------------------
  public ErmesScrollerView(Frame theSketchWindow, Panel theView) {
    view = theView;
    view.validate();
    imageSize = new Dimension(view.getPreferredSize());
    add(view, 0);
    getHAdjustable().setUnitIncrement(10);
    getVAdjustable().setUnitIncrement(10);
    getHAdjustable().addAdjustmentListener((AdjustmentListener)theView);
    getVAdjustable().addAdjustmentListener((AdjustmentListener)theView);

    addKeyListener((KeyListener)theSketchWindow);
  }
}









