package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import ircam.jmax.*;
import com.sun.java.swing.*;
import com.sun.java.swing.basic.*;
import com.sun.java.swing.plaf.*;

public class ErmesSwToolbar extends JPanel implements /*ActionListener,*/ MouseListener{
  ErmesSketchPad itsSketchPad;
  int itsPressedButton = -1;
  boolean locked = false;
  final static int NUM_BUTTONS = 11;  
  JToolBar itsSwToolbar;
  JToggleButton itsLastPressed;
  // try to minimize the gif file loading...
  static Hashtable itsImages = new Hashtable();

  public ErmesSwToolbar(ErmesSketchPad theSketchPad) {

    setDoubleBuffered(true);
    itsSketchPad = theSketchPad;
    setLayout (new BorderLayout());    
    itsSwToolbar = new JToolBar();
    ToolBarUI ui = itsSwToolbar.getUI();
    if (ui instanceof BasicToolBarUI)
      ((BasicToolBarUI)ui).setFloatable (false);    
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
  
  /*public void paint(Graphics g){
    //    System.err.println("got a paint");
    super.paint(g);
    //CreateaDump();
    }*/
    
    public void update(Graphics g) {
      super.update(g);
      //System.err.println("got an update()");
    }
    
  /*public void repaint() {
    super.repaint(); //fosse la madonna
    // super.repaint();
    //System.err.println("got a repaint()");
    }*/
  
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
  };

  void SelectAButton(ErmesSwToggleButton theButton) {
      theButton.setSelected(true);
      Deselect();
      itsLastPressed = theButton;    
  }
  /**
   * The action taken when a toolBar button is pressed...
   */
  public void mousePressed(MouseEvent e){ 
    ErmesSwToggleButton aTButton = (ErmesSwToggleButton) e.getSource();
       //first: put to null the last, if re-pressed
    if (e.getClickCount() > 1) {
      SelectAButton(aTButton);
      itsSketchPad.startAdd(aTButton.getName());
      locked = true;
      return;
    }
    if (aTButton == itsLastPressed){
      itsLastPressed= null;
      itsSketchPad.DoNothing();
      locked = false;
    }
    else { //deselect the last
      Deselect();
      itsLastPressed = aTButton;
      itsSketchPad.startAdd(aTButton.getName()); //and tell to the sketch
    }
  }

  public void mouseClicked(MouseEvent e){}
  public void mouseReleased(MouseEvent e){}
  public void mouseEntered(MouseEvent e){}
  public void mouseExited(MouseEvent e){}

  public void Deselect() {
      if (itsLastPressed != null)
	itsLastPressed.setSelected(false);
      itsLastPressed = null;
  }

  public void setRunMode(boolean theRunMode) {
    //JComponent aComponent;
    if(theRunMode) setBackground(Color.white);
    else setBackground(itsSwToolbar.getBackground());
    //for(int i=0;i< getComponentCount(); i++) {
    //getComponent(i).setVisible(!theRunMode);
    //}
    itsSwToolbar.setVisible(!theRunMode);
  }
 
  public void Lock(){}//??
  //--------------------------------------------------------
  //	InsertButton
  //--------------------------------------------------------
  private void InsertButtons() {
    //charge images if not already charged
    String fs = MaxApplication.jmaxProperties.getProperty("file.separator");
    String path = MaxApplication.jmaxProperties.getProperty("root")+fs+"images"+fs;
    String p = new String("ircam.jmax.editors.ermes.");

    AddButton(p+"ErmesObjExternal", path+"extUp.gif");
    AddButton(p+"ErmesObjMessage", path+"messUp.gif");
    AddButton(p+"ErmesObjBang", path+"bangUp.gif");
    AddButton(p+"ErmesObjToggle", path+"toggleUp.gif");
    AddButton(p+"ErmesObjInt", path+"intUp.gif");
    AddButton(p+"ErmesObjFloat", path+"floatUp.gif");
    AddButton(p+"ErmesObjComment", path+"textUp.gif");
    AddButton(p+"ErmesObjSlider", path+"sliderUp.gif");
    AddButton(p+"ErmesObjIn", path+"inUp.gif");
    AddButton(p+"ErmesObjOut", path+"outUp.gif");
    AddButton(p+"ErmesObjPatcher", path+"patcherUp.gif");
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

                    







