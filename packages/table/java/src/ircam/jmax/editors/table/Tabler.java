
package ircam.jmax.editors.table;
import ircam.jmax.*;
import ircam.jmax.mda.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.AWTEvent.*;
import java.io.*;
import tcl.lang.*;

/** 
 A simple table editor
 */

public class Tabler extends MaxEditor implements MaxDataEditor,  MouseMotionListener, MouseListener{
  int N_POINTS = 300;
  int MAX_VALUE = 300;
  int old_dragx = 0;
  int old_dragy = 0;
  int values[];
 
  TextField itsFormula = new TextField("", 40);
  Panel itsCoordinates;
  //Graphics holdGraph;
  //Graphics offGraphics = null;
  //Dimension offDimension;	   
  //Image offImage;			 
  public MaxData itsData;
  static int untitledCounter = 1;

  public Tabler(MaxData theData) {
    super(MaxDataType.getTypeByName("Table"));
    if (theData.getName()==null) setTitle(GetNewUntitledName());
    else {
      setTitle(theData.getDataSource().toString()); 
    }

    //if ( (offGraphics == null)){					  
    //Dimension d = preferredSize();
    //offDimension = preferredSize();
    //offImage = createImage(preferredSize().width, preferredSize().height);
    //offGraphics = offImage.getGraphics();
    //  }

    itsData = theData;
    getContentPane().setLayout(new BorderLayout());
    setBackground(Color.white);
    values = new int[N_POINTS];
    itsFormula.resize(300, 20);
    getContentPane().add("South", itsFormula);

    itsCoordinates = new Panel();
    itsCoordinates.resize(300, 20);
    getContentPane().add("North", itsFormula);
    
    validate();
    //holdGraph = getGraphics();
    addMouseMotionListener(this);
    addMouseListener(this);
    itsFormula.addKeyListener(this);
    setBounds(100, 100, 300,300);
    setVisible(true);
  }

 

  public Tabler() {
    super();
  }

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


  public static String GetNewUntitledName() {
    return "untitled"+(untitledCounter++);
  }

  public void SetupMenu(){}

  public void paint(Graphics g) {
    //CopyTheOffScreen(g);
    for(int i = 0; i<N_POINTS;i++){
      PaintSingle(i, g);
    }
  }

  //////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////mouseMotionListener--inizio
  public void mouseMoved(MouseEvent e){
    int x = e.getX();
    int y = e.getY();
    itsFormula.setText("("+x+","+y+")");
  }
  
  public void mouseDragged(MouseEvent e){
    int x = e.getX();
    int y = e.getY();
    y = size().height-y;
    if (x<0 || x> N_POINTS-2) return;
    values[x] = y;
    Interpolate(old_dragx, old_dragy, x, y);
    old_dragx = x; old_dragy = y;
    //DoublePaint(x);
    PaintSingle(x, getGraphics());
  }
  //////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////mouseMotionListenr--fine
    
  /*void DoublePaint(int x) {
    PaintSingle(x, getGraphics());
    PaintSingle(x, offGraphics);
    }*/
  
  void Interpolate(int x1, int y1, int x2, int y2) {
    Dimension d = size();
    Graphics temp = getGraphics();
    
    if (x1 == x2) return;	//nothing to do
    
    float factor = (y2-y1)/(x2-x1);
    if (x2>x1) for (int i=x1+1; i<x2; i++) {
      values[i] = (int) (values[i-1]+factor);
      PaintSingle(i, /*offGraphics*/getGraphics());
    }
    else for (int i=x1-1; i>x2; i--) {
      values[i] = (int) (values[i+1]+factor);
      PaintSingle(i, /*offGraphics*/getGraphics());
    }
    //CopyTheOffScreen(temp);
  }


   /////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////keyListener --inizio  
  public void keyTyped(KeyEvent e){}
  public void keyReleased(KeyEvent e){}
  // Modified to use inheritance and call the MaxEditor method
  // for all the standard key bindings
  public void keyPressed(KeyEvent e){  
    //public boolean keyDown(Event evt,int key){
    int key = e.getKeyCode();
    if(key == ircam.jmax.utils.Platform.RETURN_KEY || key == ircam.jmax.utils.Platform.ENTER_KEY){
      ApplyFormula(itsFormula.getText());
    }
    else super.keyPressed(e);
  }
  ////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////// keyListener --fine

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
	"set z " + ((i == 0) ? values[0]:values[i-1]) + "; "+
	"eval \" expr " + theFormula + "\"\n";
      try {
	MaxApplication.getTclInterp().eval(s);
	values[i] = Integer.parseInt(MaxApplication.getTclInterp().getResult().toString());
      } 	catch (TclException e) {
	itsFormula.setText("TCL error: " + e);
	return;
      }
      catch(NumberFormatException e1) {
	itsFormula.setText(e1.toString());
	return;
      }
      //DoublePaint(i);
      PaintSingle(i, getGraphics());
    }
  }
  /////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////mouseListener--inizio
  public void mouseClicked(MouseEvent e){}

  public void mousePressed(MouseEvent e){
    int x = e.getX();
    int y = e.getY();
    y = size().height-y;
    if (x<0 || x>N_POINTS-2) return;
    old_dragx = x;
    old_dragy = y;
    values[x] = y;
    //DoublePaint(x);
    PaintSingle(x, getGraphics());
  }
  public void mouseReleased(MouseEvent e){}
  public void mouseEntered(MouseEvent e){}
  public void mouseExited(MouseEvent e){}
  //////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////mouseListener--fine
  void PaintSingle(int index, Graphics g) {
    Dimension d = size();
    
    g.setColor(Color.white);
    g.drawLine(index, d.height, index, 0);	//erase old line
    g.setColor(Color.black);
    g.drawLine(index, d.height, index, d.height-values[index]);	//paint new
  }

  /*public void CopyTheOffScreen(Graphics g) {
    g.drawImage(offImage, 0, 0, this);	
    }*/

  public Dimension preferredSize() {
    Dimension d = new Dimension();
    d.width = N_POINTS;
    d.height = MAX_VALUE;
    return d;
  }

  public Dimension minimumSize() {
    return preferredSize();
  }
	
}







