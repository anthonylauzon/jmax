package ircam.jmax.editors.ermes;

import java.awt.*;
import java.util.*;

import ircam.jmax.*;

/**
 * The Toolbar contained in the sketch window.
 * Loads the .gif files, creates the toggle buttons,
 * and handles the communication with the corresponding
 * sketch.
 */
public class ErmesToolBar extends Panel {
  ErmesSketchPad itsSketchPad;
  int itsPressedButton = -1;
  boolean locked = false;
  final static int NUM_BUTTONS = 11;
  static Image aImage1;
  static Image aImage2;
  static Image aImage3;
  static Image aImage4;
  static Image aImage5;
  static Image aImage6;
  static Image aImage7;
  static Image aImage8;
  static Image aImage9;
  static Image aImage10;
  static Image aImage11;
  static Image aImage12;
  static Image aImage13;
  static Image aImage14;
  static Image aImage15;
  static Image aImage16;
  static Image aImage17;
  static Image aImage18;
  static Image aImage19;
  static Image aImage20;
  static Image aImage21;
  static Image aImage22;
	
  //--------------------------------------------------------
  //	CONSTRUCTOR
  //
  //--------------------------------------------------------
  public ErmesToolBar(ErmesSketchPad theSketchPad) {
    itsSketchPad = theSketchPad;
    setBackground(Color.lightGray);
    setLayout(new GridLayout(1,NUM_BUTTONS));
    InsertButton();
  }
	
  //--------------------------------------------------------
  //	paint
  //--------------------------------------------------------
  public void paint (Graphics g) {
    g.setColor(getBackground());
    paintComponents(g);
  }
  
  //--------------------------------------------------------
  //	RunModeSetted
  //--------------------------------------------------------
  public void RunModeSetted(boolean theRunMode) {
    ErmesToggleButton aToggleButton;
    if(theRunMode) setBackground(Color.white);
    else setBackground(Color.lightGray);
    for (int i=0; i<NUM_BUTTONS; i++) {
      aToggleButton = (ErmesToggleButton) getComponent(i);
      if (theRunMode) aToggleButton.hide();
      else aToggleButton.show();
    }
  }
	
  //--------------------------------------------------------
  //	ButtonPressed
  //--------------------------------------------------------
  // we make the ipothesys that all the components inside a toolbar are ErmesToggleButtons
  boolean ButtonPressed(int theButton){
    int i;
    int nComponents = countComponents();

    if(theButton==10) {//ErmesObjPatcher for now is not implemented....
      ErmesToggleButton aToggleButton = (ErmesToggleButton) getComponent(theButton);
      aToggleButton.Toggle();
      //itsPressedButton = -1;
      return true;
    }
    if (itsPressedButton != -1) {	// if a button was pressed 
      ErmesToggleButton aToggleButton = (ErmesToggleButton) getComponent(itsPressedButton);
      aToggleButton.Toggle();
      locked = false;
    }

    itsPressedButton = theButton;
    
    //tell the sketchpad that a button was pressed in its ToolBar
    itsSketchPad.StartAdd(itsPressedButton);
    return true;
  }
  

  public void Lock(){
    locked = true;
  }
  
  //--------------------------------------------------------
  //	ButtonDepressed
  // click on a toolbar button already pressed
  //--------------------------------------------------------
  boolean ButtonDepressed(int theButton){
    itsPressedButton = -1;
    itsSketchPad.DoNothing();
    locked = false;
    return true;
  }
  
  //--------------------------------------------------------
  //	Deselect
  //  deselect the current selected button (if any)
  //--------------------------------------------------------
  public void Deselect() {
    if (itsPressedButton == -1) return;
    ErmesToggleButton aToggleButton = (ErmesToggleButton) getComponent(itsPressedButton);
    if (aToggleButton.pressed) {
      aToggleButton.Toggle();
      itsPressedButton = -1;
    }
  }
	
  //--------------------------------------------------------
  //	InsertButton
  //--------------------------------------------------------
  private void InsertButton() {
    //charge images if not already charged
    String fs = MaxApplication.ermesProperties.getProperty("file.separator");
    String path = MaxApplication.ermesProperties.getProperty("root")+fs+"ermes"+fs+"images"+fs;
    if (aImage1 == null) {
      Toolkit toolkit = Toolkit.getDefaultToolkit();
      aImage1 = toolkit.getImage(path+"extUp.gif");
      aImage2 = toolkit.getImage(path+"extDown.gif");
      aImage3 = toolkit.getImage(path+"messUp.gif");
      aImage4 = toolkit.getImage(path+"messDown.gif");
      aImage5 = toolkit.getImage(path+"bangUp.gif");
      aImage6 = toolkit.getImage(path+"bangDown.gif");
      aImage7 = toolkit.getImage(path+"toggleUp.gif");
      aImage8 = toolkit.getImage(path+"toggleDown.gif");
      aImage9 = toolkit.getImage(path+"intUp.gif");
      aImage10 = toolkit.getImage(path+"intDown.gif");
      aImage11 = toolkit.getImage(path+"floatUp.gif");
      aImage12 = toolkit.getImage(path+"floatDown.gif");
      aImage13 = toolkit.getImage(path+"textUp.gif");
      aImage14 = toolkit.getImage(path+"textDown.gif");
      aImage15 = toolkit.getImage(path+"sliderUp.gif");
      aImage16 = toolkit.getImage(path+"sliderDown.gif");
      aImage17 = toolkit.getImage(path+"inUp.gif");
      aImage18 = toolkit.getImage(path+"inDown.gif");
      aImage19 = toolkit.getImage(path+"outUp.gif");
      aImage20 = toolkit.getImage(path+"outDown.gif");
      aImage21 = toolkit.getImage(path+"patcherUp.gif");
      aImage22 = toolkit.getImage(path+"patcherDown.gif");
    }
    //Put buttons 
       	
    add(new ErmesToggleButton(aImage1, aImage2, this, 0));
    add(new ErmesToggleButton(aImage3, aImage4, this, 1));
    add(new ErmesToggleButton(aImage5, aImage6, this, 2));
    add(new ErmesToggleButton(aImage7, aImage8, this, 3));
    add(new ErmesToggleButton(aImage9, aImage10, this, 4));
    add(new ErmesToggleButton(aImage11, aImage12, this, 5));
    add(new ErmesToggleButton(aImage13, aImage14, this, 6));
    add(new ErmesToggleButton(aImage15, aImage16, this, 7));
    add(new ErmesToggleButton(aImage17, aImage18, this, 8));
    add(new ErmesToggleButton(aImage19, aImage20, this, 9));
    add(new ErmesToggleButton(aImage21, aImage22, this, 10));
  }
}

