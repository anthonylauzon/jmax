package ircam.jmax.editors.table;
import ircam.jmax.*;
import ircam.jmax.fts.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.AWTEvent.*;
import java.io.*;
import tcl.lang.*;
import com.sun.java.swing.*;

public class TablePanel extends JPanel implements MouseMotionListener, MouseListener, ActionListener{
  
  int x_scale_factor = 4;
  int N_POINTS = 128;
  int MAX_VALUE = 300;
  int old_dragx = 0;
  int old_dragy = 0;
  int values[];
  Tabler itsTabler;
  
  public TablePanel(Tabler theTabler) {
    super();
    itsTabler = theTabler;
    values = new int[N_POINTS];
    addMouseMotionListener(this);
    addMouseListener(this);
  }

  public void initValues(int[] vector, int size) {
    if (size != 0) {
      values = vector;
      N_POINTS = size;
    }
  }

  public void paint(Graphics g) {
    for(int i = 0; i<N_POINTS;i++){
      PaintSingle(i, g);
    }
  }

  public void fillTable(FtsIntegerVector aIntV) {
    if(aIntV.getSize()!=0) initValues(aIntV.getValues(),aIntV.getSize());
  }

  public void actionPerformed(ActionEvent e) {
    //the values[] pointer is still there with another content, right?
    paint(getGraphics());
  }
  

   //////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////mouseMotionListener--inizio
  public void mouseMoved(MouseEvent e){
    int x = e.getX()/x_scale_factor;
    int y = e.getY();
    itsTabler.setCoordinates(x, getSize().height-y);
  }
  
  public void mouseDragged(MouseEvent e){
    int x = e.getX()/x_scale_factor;
    int y = e.getY();
    y = getSize().height-y;
    if (x<0 || x> N_POINTS-2) return;
    values[x] = y;
    Interpolate(old_dragx, old_dragy, x, y);
    old_dragx = x; old_dragy = y;
    PaintSingle(x, getGraphics());
    itsTabler.setCoordinates(x, y);
  }
  //////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////mouseMotionListenr--fine

  void Interpolate(int x1, int y1, int x2, int y2) {
    Dimension d = size();
    Graphics temp = getGraphics();
    
    if (x1 == x2) return;	//nothing to do
    
    float factor = (y2-y1)/(x2-x1);
    if (x2>x1) for (int i=x1+1; i<x2; i++) {
      values[i] = (int) (values[i-1]+factor);
      PaintSingle(i, getGraphics());
    }
    else for (int i=x1-1; i>x2; i--) {
      values[i] = (int) (values[i+1]+factor);
      PaintSingle(i, getGraphics());
    }
  }
  
    /////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////mouseListener--inizio
  public void mouseClicked(MouseEvent e){}
  
  public void mousePressed(MouseEvent e){
    int x = e.getX()/4;
    int y = e.getY();
    y = getSize().height-y;
    if (x<0 || x>N_POINTS-2) return;
    old_dragx = x;
    old_dragy = y;
    values[x] = y;
    PaintSingle(x, getGraphics());
  }
  public void mouseReleased(MouseEvent e){
    ((FtsIntegerVector)(itsTabler.itsData.getContent())).changed();
    ((FtsIntegerVector)(itsTabler.itsData.getContent())).forceUpdate();
  }

  public void mouseEntered(MouseEvent e){}
  public void mouseExited(MouseEvent e){}
  //////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////mouseListener--fine
  void PaintSingle(int index, Graphics g) {
    Dimension d = size();
    index = index*x_scale_factor;
    g.setColor(Color.white);
    for(int i=0;i<x_scale_factor;i++){
      g.drawLine(index+i, d.height, index+i,0);
    }    
    g.setColor(Color.black);
    for(int i=0;i<x_scale_factor;i++){
      g.drawLine(index+i, d.height, index+i, d.height-values[index/x_scale_factor]);
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
	"set z " + ((i == 0) ? values[0]:values[i-1]) + "; "+
	"eval \" expr " + theFormula + "\"\n";
      try {
	MaxApplication.getTclInterp().eval(s);
	values[i] = Integer.parseInt(MaxApplication.getTclInterp().getResult().toString());
      } 	catch (TclException e) {
	itsTabler.itsFormula.setText("TCL error: " + e);
	return;
      }
      catch(NumberFormatException e1) {
	itsTabler.itsFormula.setText(e1.toString());
	return;
      }
      PaintSingle(i, getGraphics());
    }
  }

  public Dimension preferredSize() {
    Dimension d = new Dimension();
    d.width = N_POINTS*x_scale_factor;
    d.height = MAX_VALUE;
    return d;
  }

  public Dimension minimumSize() {
    return preferredSize();
  }
}







