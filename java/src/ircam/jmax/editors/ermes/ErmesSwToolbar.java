package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import ircam.jmax.utils.*;//probing...
import ircam.jmax.*;
import ircam.jmax.dialogs.*;
import com.sun.java.swing.*;
import com.sun.java.swing.plaf.*;
import com.sun.java.swing.plaf.motif.*;

public class ErmesSwToolbar extends JPanel implements  MouseListener{
  ErmesSketchPad itsSketchPad;
  int itsPressedButton = -1;
  boolean locked = false;
  boolean pressed = false;
  final static int NUM_BUTTONS = 11;  
  JToolBar itsSwToolbar;
  JToggleButton itsLastPressed;
  // try to minimize the gif file loading...
  static Hashtable itsImages = new Hashtable();

  public ErmesSwToolbar(ErmesSketchPad theSketchPad) {

    try{
      UIManager.setLookAndFeel("com.sun.java.swing.plaf.motif.MotifLookAndFeel");
    }catch (Exception exc) {
      ErrorDialog aErr = new ErrorDialog(theSketchPad.itsSketchWindow, "Could not load MotifLookAndFeel");
      aErr.setLocation(100, 100);
      aErr.show();  
    }
    setDoubleBuffered(true);
    itsSketchPad = theSketchPad;
    setLayout (new BorderLayout());    
    itsSwToolbar = new JToolBar();
    itsSwToolbar.setFloatable (false);   
    
    InsertButtons();

    add (itsSwToolbar, BorderLayout.WEST);
    setBackground(itsSwToolbar.getBackground());
    validate();
  }

  void CreateADump(){
    int a = 0;
    try {
      a = 14/a;
    }
    catch (Exception e) {
      e.printStackTrace();
    }
  }
  
  public void update(Graphics g) {
    super.update(g);
    //System.err.println("got an update()");
  }
  
  /**
   * Add a button to the toolbar, trying not to load already loaded
   * gif files (case of multiple windows with same palette).
   */
  public void AddButton(String name, String gifFilePath) {
    ErmesSwToggleButton aToggleButton;
    ImageIcon aImageIcon = (ImageIcon) itsImages.get(gifFilePath);
    if (aImageIcon == null) {
      aImageIcon =  new ImageIcon(gifFilePath);
      itsImages.put(gifFilePath, aImageIcon);
    }
    
    aToggleButton = new ErmesSwToggleButton(name, aImageIcon);
    itsSwToolbar.add(aToggleButton);
    aToggleButton.addMouseListener(this);
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
      itsSketchPad.startAdd(aTButton.getName()); //and tell to the sketch
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
      itsSketchPad.startAdd(aTButton.getName());
      aTButton.setSelected(true);
      locked = true;
      return;
    }
  }
  
  public void mouseReleased(MouseEvent e){
    if(!itsLastPressed.contains(e.getX(), e.getY())) itsSketchPad.DoNothing();
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
    if(pressed) Unlock();
    if(theRunMode) setBackground(Color.white);
    else setBackground(itsSwToolbar.getBackground());
    itsSwToolbar.setVisible(!theRunMode);
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
    //charge images if not already charged
    String fs = MaxApplication.jmaxProperties.getProperty("file.separator");
    String path = MaxApplication.jmaxProperties.getProperty("root")+fs+"images"+fs;
    String p = new String("ircam.jmax.editors.ermes.");

    AddButton(p+"ErmesObjExternal", path+"tool_ext.gif");
    AddButton(p+"ErmesObjMessage", path+"tool_mess.gif");
    AddButton(p+"ErmesObjPatcher", path+"tool_patcher.gif");
    AddButton(p+"ErmesObjIn", path+"tool_in.gif");
    AddButton(p+"ErmesObjOut", path+"tool_out.gif");
    AddButton(p+"ErmesObjComment", path+"tool_text.gif");
    AddButton(p+"ErmesObjBang", path+"tool_bang.gif");
    AddButton(p+"ErmesObjToggle", path+"tool_toggle.gif");
    AddButton(p+"ErmesObjSlider", path+"tool_slider.gif");
    AddButton(p+"ErmesObjInt", path+"tool_int.gif");
    AddButton(p+"ErmesObjFloat", path+"tool_float.gif");
  }

  public Dimension getMinimumSize(){
    return new Dimension(600, 30);
  }
  public Dimension getPreferredSize(){
    return new Dimension(600, 30);
  }
  
  /*public Dimension minimumSize(){
    return new Dimension(500, 50);
    }
    public Dimension preferredSize(){
    return new Dimension(500,50);
    }*/
  
}

                    








