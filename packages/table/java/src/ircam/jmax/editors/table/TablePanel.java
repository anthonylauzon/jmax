package ircam.jmax.editors.table;

import java.lang.*;
import ircam.jmax.*;
import ircam.jmax.fts.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.AWTEvent.*;
import java.io.*;
import tcl.lang.*;
import com.sun.java.swing.*;

public class TablePanel extends JPanel implements MouseMotionListener, MouseListener{
  
  int x_scale_factor = 4;
  int N_POINTS = 128;
  int MAX_VALUE = 300;
  int old_dragx = 0;
  int old_dragy = 0;
  int values[];
  Tabler itsTabler;
  
  Graphics offGraphics = null;
  Dimension offDimension;	   
  Image offImage;	

  public TablePanel(Tabler theTabler) {
    super();
    itsTabler = theTabler;
    setBackground(Color.white);
    values = new int[N_POINTS];
    //InitOffScreen();
    addMouseMotionListener(this);
    addMouseListener(this);
  }

  void InitOffScreen(){
    Dimension d = preferredSize();	    
    if((offGraphics == null)){					  
      offDimension = d;
      offImage = createImage(d.width, d.height);
      offGraphics = offImage.getGraphics();
    }
  }

  public void initValues(int[] vector) {
    if (vector != null && vector.length != 0) {
      values = vector;
      N_POINTS = vector.length;
    }
  }

  public int getValue(int i) {
    if (i > 0 && i<values.length) return values[i];
    else return 0;
  }

  public void update(Graphics g) {
  }

  public void paint(Graphics g) {
    //System.err.println("fava");
    /*for(int i = 0; i<N_POINTS;i++){
      PaintSingle(i, g);
    }*/
    InitOffScreen();
    CopyTheOffScreen(g);
  }
  
  void DoublePaint(int x) {
    PaintSingle(x, getGraphics());
    InitOffScreen();
    PaintSingle(x, offGraphics);
  }

  public void CopyTheOffScreen(Graphics g) {
    InitOffScreen();
    g.drawImage(offImage, 0, 0, this);	
  }

  public void fillTable(FtsIntegerVector aIntV) {
    if(aIntV.getSize()!=0) initValues(aIntV.getValues());
  }

   //////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////mouseMotionListener--inizio
  public void mouseMoved(MouseEvent e){
    int x = e.getX()/x_scale_factor;
    int y = e.getY();
    if (x < values.length) 
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
    //PaintSingle(x, getGraphics());
    DoublePaint(x);
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
      //PaintSingle(i, getGraphics());
      DoublePaint(i);
    }
    else for (int i=x1-1; i>x2; i--) {
      values[i] = (int) (values[i+1]+factor);
      //PaintSingle(i, getGraphics());
      DoublePaint(i);
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
    //PaintSingle(x, getGraphics());
    DoublePaint(x);
  }

  public void mouseReleased(MouseEvent e){
    ((FtsIntegerVector)(itsTabler.itsData.getContent())).changed();
    ((FtsIntegerVector)(itsTabler.itsData.getContent())).forceUpdate();
  }

  public void mouseEntered(MouseEvent e){}
  public void mouseExited(MouseEvent e){}
  //////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////mouseListener--fine

  /*void DrawPoint(int index, Graphics g) {
    Dimension d = size();
     for(int i=0;i<x_scale_factor;i++){
       g.drawLine(index+i, d.height, index+i,0);
     }   
   }
  
   void PaintSingle(int index, Graphics g) {
     Dimension d = size();
     index = index*x_scale_factor;
     g.setColor(Color.white);
     DrawPoint(index, g);
     g.setColor(Color.black);
     DrawPoint(index, g);
     g.setColor(Color.white);
   }*/
  
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
      ((FtsIntegerVector)(itsTabler.itsData.getContent())).changed();
      //PaintSingle(i, getGraphics());
      DoublePaint(i);
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







