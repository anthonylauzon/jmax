
package ircam.jmax.editors.table;

import ircam.jmax.*;
import ircam.jmax.utils.*;
import ircam.jmax.mda.*;
import java.awt.*;
import java.io.*;
import java.awt.event.*;
import tcl.lang.*;
import com.sun.java.swing.*;

/** 
 A simple table editor
 */

public class Tabler extends MaxEditor implements MouseListener, MouseMotionListener, KeyListener, MaxDataEditor{
  /*  protected String itsFileName = "";
      protected String itsPathName = "";
      protected String itsDocumentType = "";
      protected boolean neverSaved = true;
      protected String itsTitle = "";
      */

  boolean alreadySaved =true;
  int N_POINTS = 300;
  int MAX_VALUE = 300;
  int old_dragx = 0;
  int old_dragy = 0;
  int values[];
 
  TextField	itsFormula = new TextField("", 40);
  Graphics offGraphics = null;
  Dimension offDimension;	   
  Image offImage;			 
  
  // the MaxDataEditor interface

  public void quitEdit() {}
  /** Tell the editor to syncronize, i.e. to store in the
   * data all the information possibly cached in the editor
   * and still not passed to the data instance; this usually
   * happen before saving an instance.
   */
  public void syncData(){}
  /** Tell the editor the data has changed; it pass a sigle Java
   * Object that may code what is changed and where; if the argument
   * is null, means usually that all the data is changed
   */
  public void dataChanged(Object reason){}

  //end of the MaxDataEditor interface
 
  ////// MaxEditor abstract methods
  public boolean CustomMenuActionPerformed(MenuItem theMenuItem, String itemName){return true;}
  public boolean CustomMenuItemStateChanged(CheckboxMenuItem theCheckItem, String itemName){return true;}
  
  public void SetupMenu(){
    GetCloseMenu().setEnabled(false);
    GetSaveMenu().setEnabled(false);
    GetSaveAsMenu().setEnabled(false);
    GetPrintMenu().setEnabled(true);
    GetCutMenu().setEnabled(false);
    GetCopyMenu().setEnabled(true);//clipboard test
    GetPasteMenu().setEnabled(true);
    GetClearMenu().setEnabled(false);
  }
  ///// endo of MaxEditor anstract methods

  public Tabler(String title) {
    super(title);
    getContentPane().setLayout(new BorderLayout());
    setBackground(Color.white);
    values = new int[N_POINTS];
    InitTabler();
    addMouseListener(this);
    addMouseMotionListener(this);
    itsFormula.addKeyListener(this);
  }
  
  
  /*public boolean Save() {
    FileOutputStream fs;
    PrintStream ps;
    String aOldFileName = GetWholeName();
    
    if (itsFileName.equals("") || itsPathName.equals("")) {
    FileDialog fd = new FileDialog(this, "FileDialog", FileDialog.SAVE);
    fd.show();
    
    if((fd.getFile()==null)||(fd.getDirectory()==null)) return false;
    
    itsFileName = fd.getFile(); 
    itsPathName = fd.getDirectory();
    }
    if(itsFileName.equals("")) return false;
    try {
    fs = new FileOutputStream(GetWholeName());
    }
    catch(IOException e) {
    itsProject.itsProjectWindow.GetApp().GetPrintStream().println("ERROR while opening " + GetWholeName());
    return false;
    }
    ps = new PrintStream(fs);
    for(int i=0; i<300; i++){
    ps.println(""+values[i]);
    }
    try{
    fs.flush();
    }
    catch(IOException e) {
    itsProject.itsProjectWindow.GetApp().GetPrintStream().println("ERROR while flushing " + GetWholeName());
    return false;
    }
    try {
    fs.close();
    } catch (IOException e) {
    itsProject.itsProjectWindow.GetApp().GetPrintStream().println("ERROR while closing " + GetWholeName());
    return false;
    }
    alreadySaved = true;
    neverSaved = false;
    
    if(!aOldFileName.equals(GetWholeName())){
    itsProject.itsProjectWindow.GetApp().ChangeWinNameMenus(aOldFileName,itsFileName);
    setTitle(itsFileName);
    if(itsProject.HaveAEntry(aOldFileName)) 
    itsProject.GetTheEntry(aOldFileName).SetFileName(itsFileName, itsPathName);
    } 
    return true;
    }*/
  
  public void paint(Graphics g) {
    CopyTheOffScreen(g);
  }

  public void mouseMoved(MouseEvent e) {
  }
  
  public void mouseDragged(MouseEvent e) {
    int x = e.getX();
    int y = e.getY();

    y = size().height-y;
    if (x<0 || x> N_POINTS-2) return;
    values[x] = y;
    Interpolate(old_dragx, old_dragy, x, y);
    old_dragx = x; old_dragy = y;
    alreadySaved = false;
    DoublePaint(x);
  }
  
  void DoublePaint(int x) {
    PaintSingle(x, getGraphics());
    PaintSingle(x, offGraphics);
  }
  
  void Interpolate(int x1, int y1, int x2, int y2) {
    Dimension d = size();
    Graphics temp = getGraphics();
    
    if (x1 == x2) return;	//nothing to do
    
    float factor = (y2-y1)/(x2-x1);
    if (x2>x1) for (int i=x1+1; i<x2; i++) {
      values[i] = (int) (values[i-1]+factor);
      PaintSingle(i, offGraphics);
    }
    else for (int i=x1-1; i>x2; i--) {
      values[i] = (int) (values[i+1]+factor);
      PaintSingle(i, offGraphics);
    }
    CopyTheOffScreen(temp);
  }
  
  //public void InitFromDocument(JDocument theDocument){}
  
  /*Dimension d = preferredSize();				//ooooooooo
    
    //Create the offscreen graphics context, if no good one exists.
    if ( (offGraphics == null)){					  
    offDimension = d;
    offImage = createImage(d.width, d.height);
    offGraphics = offImage.getGraphics();
    }*/
  
  
  /*  public void InitDoc(String theTitle, JProject theProject) {
      itsTitle = theTitle;
      itsFileName = theTitle;
      itsProject = theProject;
      values = new int[N_POINTS];
      values[N_POINTS-1] = 0;
      setTitle("TableEditor: "+theTitle);
      setBackground(Color.white);
      move(400, 0);
      pack();
      Rectangle temp = bounds();
      itsFormula.resize(temp.width, 20);
      add("South", itsFormula);
      validate();
      
      Dimension d = preferredSize();    
      //Create the offscreen graphics context, if no good one exists.
      if ( (offGraphics == null)){					  
      offDimension = d;
      offImage = createImage(d.width, d.height);
      offGraphics = offImage.getGraphics();
      }
      show();
      }*/
  
  
  /*public void InitDoc(String theTitle, String thePath, String thefName, JProject theProject) {
    itsTitle = theTitle;
    itsFileName = thefName;
    itsPathName = thePath;
    itsProject = theProject;
    values = new int[N_POINTS];
    values[N_POINTS-1] = 0;
    setTitle("TableEditor: "+theTitle);
    setBackground(Color.white);
    move(400, 0);
    pack();
    Rectangle temp = bounds();
    itsFormula.resize(temp.width, 20);
    add("South", itsFormula);
    validate();
    
    Dimension d = preferredSize();
    
    //Create the offscreen graphics context, if no good one exists.
    
    if ( (offGraphics == null)){
    offDimension = d;
    offImage = createImage(d.width, d.height);
    offGraphics = offImage.getGraphics();
    }
    Load(thePath+thefName);
    show();
    }*/
  
  //////////////KeyListener interface (inherit from MaxEditor)////
  
  public void keyTyped(KeyEvent e){}
  
  public void keyReleased(KeyEvent e){}
  
  public void keyPressed(KeyEvent e){
    //if(e.controlDown()){
    //  return super.keyDown(evt,key);
    //}
    if(e.getKeyCode() == Platform.RETURN_KEY || e.getKeyCode() == Platform.ENTER_KEY){
      System.err.println("applico la formula");
      ApplyFormula(itsFormula.getText());
      alreadySaved = false;
    }
  }
  
  void ApplyFormula(String theFormula) {
    //we should evaluate something like:
    //"set x <i>; set y values[i]; [eval <theFormula>] (or <theFormula>)
    String s = new String();
    int temp;
    int start=0;
    int end=N_POINTS-1;
    String ps = new String();
    
    //analize string s; if it begins with "[" is a subset operator
    if (theFormula.startsWith("[")) { //parserino al volo
      int p = theFormula.indexOf(']');
      int semic;
      ps = theFormula.substring(1,p);
      semic = ps.indexOf(':');
      if (semic != -1) {
	String primo = ps.substring(0,semic);
	String secondo = ps.substring(semic+1);
	start = Integer.parseInt(primo);
	end = Integer.parseInt(secondo);
      }
      else {
	start = end = Integer.parseInt(ps);
      }
      theFormula = theFormula.substring(p+1);
    }
    
    for (int i=start; i<=end; i++) {
      s = "set x " + i + "; " + "set y " + values[i] + "; " + 
	"set z " + ((i == 0) ? values[0]:values[i-1]) + "; "+ "expr " + theFormula + "\r\n";
      try {
	MaxApplication.getTclInterp().eval(s);
      } 	
      catch (TclException e) {
	itsFormula.setText("TCL error: " + e);
	return;
      }
      catch(NumberFormatException e1) {
	itsFormula.setText(e1.toString());
	return;
      }
      //      try {
	values[i] = Integer.parseInt(MaxApplication.getTclInterp().getResult().toString());
	//values[i] = Integer.parseInt((String) ReflectObject.get(MaxApplication.getTclInterp(), MaxApplication.getTclInterp().getResult()));
	//}      
      //      catch (TclException e2) {
      //	itsFormula.setText("errore 2:" + e2.toString());
      //}
	DoublePaint(i);
    }
  }

  public void mouseEntered(MouseEvent e){}
  public void mouseExited(MouseEvent e){}
  public void mouseReleased(MouseEvent e){}
  public void mouseClicked(MouseEvent e){}
  
  public void mousePressed (MouseEvent e) { 
    int x = e.getX();
    int y = e.getY();

    y = size().height-y;
    if (x<0 || x>N_POINTS-2) return;
    old_dragx = x;
    old_dragy = y;
    values[x] = y;
    alreadySaved = false;
    DoublePaint(x);
    return;
  }
  
  void PaintSingle(int index, Graphics g) {
    Dimension d = size();
    
    g.setColor(Color.white);
    g.drawLine(index, d.height, index, 0);	//erase old line
    g.setColor(Color.black);
    g.drawLine(index, d.height, index, d.height-values[index]);	//paint new
    //paint(g);
  }

  public void CopyTheOffScreen(Graphics g) {
    if ( (offGraphics == null)){	
      //System.err.println("ancora nullo");
      Dimension d = preferredSize();				
      // || (d.width != offDimension.width)
      // || (d.height != offDimension.height)  
      offDimension = d;
      offImage = createImage(d.width, d.height);
      if (offImage != null)//always the same problem
	offGraphics = offImage.getGraphics();
    }
    g.drawImage(offImage, 0, 0, this);	
  }
  
  public void InitTabler() {
    
    Dimension d = preferredSize();				//ooooooooo
    
    //Create the offscreen graphics context, if no good one exists.
    if ( (offGraphics == null)){					
      // || (d.width != offDimension.width)
      // || (d.height != offDimension.height)  
      offDimension = d;
      offImage = createImage(d.width, d.height);
      if (offImage != null)//always the same problem
	offGraphics = offImage.getGraphics();
    }
    //  itsFileName = theTitle;
    //  itsProject = theProject;
      values = new int[N_POINTS];
      values[N_POINTS-1] = 0;
      setTitle("TableEditor");
      setBackground(Color.white);
      move(400, 0);
      pack();
      Rectangle temp = bounds();
      itsFormula.resize(temp.width, 20);
      getContentPane().add("South", itsFormula);
      validate();
      setVisible(true);
  }	
	
  public Dimension preferredSize() {
    Dimension d = new Dimension();
    d.width = N_POINTS;
    d.height = MAX_VALUE;
    return d;
  }

  public Dimension minimumSize() {
    return preferredSize();
  }
	
  /*  public boolean OpenFile(String theName, String thePath){
      String aOldName = GetName();
      itsTitle = theName;
      itsFileName = theName;
      itsPathName = thePath;
      setTitle("TableEditor: "+itsTitle);
      Load(itsPathName+itsFileName);
      itsProject.itsProjectWindow.GetApp().ChangeWinNameMenus(aOldName, theName);
      return true;
      }*/
  


  /*public boolean Load(String theWholeName) {
    
    FileInputStream fs;
    boolean temp = true;
    
    try {
    fs = new FileInputStream(theWholeName);
    }
    catch(IOException e) {
    itsProject.itsProjectWindow.GetApp().GetPrintStream().println("ERROR while opening " + GetWholeName());
    return false;
    }
    int aBuffer[] = new int[300];
    int c;
    char aTemp;
    String aString = "";
    int i=0;
    try{
    while(((c = fs.read())!= -1)&&(i<300)){
    aTemp = (char) c;
    if((aTemp!='\r')&&(aTemp!='\n')){
    aString = aString+aTemp;
    }
    else{
    c = (Integer.valueOf(aString)).intValue();
    values[i++] = c;
    aString = "";
    }
    }
    }
    catch(IOException e){
    itsProject.itsProjectWindow.GetApp().GetPrintStream().println("ERROR while reading " + GetWholeName());
    return false;
    }
    try {
    fs.close();
    } catch (IOException e) {
    itsProject.itsProjectWindow.GetApp().GetPrintStream().println("ERROR while closing " + GetWholeName());
    return false;
    }
    for(int j=0;j<300;j++){
    DoublePaint(j);
    }
    neverSaved = false;
    alreadySaved  = true;
    return false;
    }*/
  
  /*public boolean SaveAs(String theWholeName) {
    return false;	
    }*/

}







