package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.io.*;

import com.sun.java.swing.*;
import com.sun.java.swing.plaf.*;
import com.sun.java.swing.plaf.motif.*;

import ircam.jmax.utils.*;//probing...
import ircam.jmax.*;
import ircam.jmax.dialogs.*;


public class ErmesSwToolbar extends JPanel implements  MouseListener
{
  ErmesSketchPad itsSketchPad;
  int itsPressedButton = -1;
  boolean locked = false;
  boolean pressed = false;
  final static int NUM_BUTTONS = 11;  
  CardLayout layout;
  JPanel cards;
  JPanel tools;
  JPanel empty;
  JToolBar itsSwToolbar;
  JToggleButton itsLastPressed;
  // try to minimize the gif file loading...
  static Hashtable itsImages = new Hashtable();

  /** This static method preload all the tipical images in the hashtable, and is called
    during application startup; this will reduce the first patcher opening time,
    while slowing down the application startup */

  static void loadToolBarImages()
  {
    String images[] = { "tool_ext.gif",     "tool_mess.gif", 
			"tool_patcher.gif", "tool_in.gif", 
			"tool_out.gif",     "tool_text.gif", 
			"tool_bang.gif",    "tool_toggle.gif", 
			"tool_slider.gif",  "tool_int.gif", 
			"tool_float.gif" };

    String fs = File.separator;
    String path = MaxApplication.getProperty("root")+fs+"images"+fs;

    for (int i = 0; i < images.length; i++)
      {
	String gifFilePath = path + images[i];

	itsImages.put(gifFilePath, new ImageIcon(gifFilePath));
      }
  }

  public ErmesSwToolbar(ErmesSketchPad theSketchPad) {

    setDoubleBuffered(false);
    itsSketchPad = theSketchPad;
    setLayout (new BorderLayout());    

    cards = new JPanel();
    add(cards,  BorderLayout.WEST);
    layout = new CardLayout();    
    cards.setLayout(layout);    

    tools = new JPanel();
    cards.add("edit", tools);
    tools.setLayout (new BorderLayout());    

    empty = new JPanel();
    cards.add("run", empty);

    itsSwToolbar = new JToolBar();
    itsSwToolbar.setFloatable (false);   
    InsertButtons();
    tools.add(itsSwToolbar, BorderLayout.WEST);

    // layout.first();// 

    validate();
  }

  /**
   * Add a button to the toolbar, trying not to load already loaded
   * gif files (case of multiple windows with same palette).
   */

  void AddButton(Class theClass, String gifFilePath) {
    ErmesSwToggleButton aToggleButton;
    ImageIcon aImageIcon = getIcon(gifFilePath);
    
    aToggleButton = new ErmesSwToggleButton(theClass, aImageIcon);
    itsSwToolbar.add(aToggleButton);
    aToggleButton.addMouseListener(this);
  }

  static ImageIcon getIcon(String gifFilePath) {
    ImageIcon aImageIcon = (ImageIcon) itsImages.get(gifFilePath);
    if (aImageIcon == null)
      {
	aImageIcon =  new ImageIcon(gifFilePath);
	itsImages.put(gifFilePath, aImageIcon);
      }
    //else already loaded
    return aImageIcon;
  }
  
  void SelectAButton(ErmesSwToggleButton theButton) {
    theButton.setSelected(true);
    Deselect();
    itsLastPressed = theButton; 
    pressed = true;
  }
  /**
   * The action taken when a toolBar button is pressed...
   */
  public void mousePressed(MouseEvent e){ 
    ErmesSwToggleButton aTButton = (ErmesSwToggleButton) e.getSource();
    
    if (e.getClickCount() > 1) {
      return;
    }
    if (aTButton == itsLastPressed){
      itsSketchPad.DoNothing();
      locked = false;
      pressed = false;
    }
    else { //deselect the last
      Deselect();
      locked = false;
      itsLastPressed = aTButton;
      pressed = true;
      itsSketchPad.startAdd(aTButton.getNewObjectClass()); //and tell to the sketch
    }
  }

  public void mouseClicked(MouseEvent e){
    ErmesSwToggleButton aTButton = (ErmesSwToggleButton) e.getSource();
    if (e.getClickCount() > 1) {
      pressed = true;
      //aTButton.setSelected(true);
      if(itsLastPressed!=aTButton){
	itsLastPressed.setSelected(false);
	itsLastPressed = aTButton;   
      }
      itsSketchPad.startAdd(aTButton.getNewObjectClass());
      aTButton.setSelected(true);
      locked = true;
      return;
    }
  }
  
  public void mouseReleased(MouseEvent e){
    if(!itsLastPressed.contains(e.getX(), e.getY())) {
      itsLastPressed = null;
      pressed = false;
      itsSketchPad.DoNothing();
    }
  }
  public void mouseEntered(MouseEvent e){}
  public void mouseExited(MouseEvent e){}

  public void Deselect() {
    if (itsLastPressed != null)
      itsLastPressed.setSelected(false);
    itsLastPressed = null;
    pressed = false;
  }

  public void setRunMode(boolean theRunMode) {
    if (pressed)
      Unlock();

    // itsSwToolbar.setVisible(!theRunMode);

    layout.show(cards, theRunMode ? "run" : "edit");
  }
 
  public void Lock(){}//??
  public void Unlock(){
    Deselect();
    itsSketchPad.DoNothing();
    locked = false;
    pressed = false;
  }
  //--------------------------------------------------------
  //	InsertButton
  //--------------------------------------------------------

  private void InsertButtons() {
    String fs = File.separator;
    String path = MaxApplication.getProperty("root")+fs+"images"+fs;

    AddButton(ircam.jmax.editors.ermes.ErmesObjExternal.class, path+"tool_ext.gif");
    AddButton(ircam.jmax.editors.ermes.ErmesObjMessage.class, path+"tool_mess.gif");
    AddButton(ircam.jmax.editors.ermes.ErmesObjPatcher.class, path+"tool_patcher.gif");
    AddButton(ircam.jmax.editors.ermes.ErmesObjIn.class, path +"tool_in.gif");
    AddButton(ircam.jmax.editors.ermes.ErmesObjOut.class, path +"tool_out.gif");
    AddButton(ircam.jmax.editors.ermes.ErmesObjComment.class, path + "tool_text.gif");
    AddButton(ircam.jmax.editors.ermes.ErmesObjBang.class, path+"tool_bang.gif");
    AddButton(ircam.jmax.editors.ermes.ErmesObjToggle.class, path+"tool_toggle.gif");
    AddButton(ircam.jmax.editors.ermes.ErmesObjSlider.class, path+"tool_slider.gif");
    AddButton(ircam.jmax.editors.ermes.ErmesObjInt.class, path+"tool_int.gif");
    AddButton(ircam.jmax.editors.ermes.ErmesObjFloat.class, path+"tool_float.gif");
  }

  public Dimension getMinimumSize(){
    return new Dimension(600, /*150630*/25);
  }
  public Dimension getPreferredSize(){
    return new Dimension(600, /*150630*/25);
  }
  
  /*public Dimension minimumSize(){
    return new Dimension(500, 50);
    }
    public Dimension preferredSize(){
    return new Dimension(500,50);
    }*/
  
}

                    








