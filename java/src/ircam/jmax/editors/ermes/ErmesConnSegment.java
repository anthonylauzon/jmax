package ircam.jmax.editors.ermes;

import java.awt.*;
import java.util.*;

/**
 * The single segment in an autorouted connection.
 * 
 */
public class ErmesConnSegment{

	Point itsStart, itsEnd;
	ErmesConnection itsConnection;
	Dimension preferredSize;
	int itsX, itsY;
	boolean laidOut;
	static int debug_countp = 1;
	static int debug_countP = 1;
	
	//--------------------------------------------------------
    //	CONSTRUCTOR
    //--------------------------------------------------------
	public ErmesConnSegment(){}
	
	//--------------------------------------------------------
    //	CONSTRUCTOR
    //--------------------------------------------------------
	public ErmesConnSegment(Point theStart, Point theEnd, ErmesConnection theConnection){
		int width, height;
		itsStart = new Point(theStart.x, theStart.y);
		itsEnd = new Point(theEnd.x, theEnd.y);
		itsConnection = theConnection;
		laidOut = false;
		
		if(!itsConnection.GetErrorState()){
			if(itsStart.x != itsEnd.x){
				width = ((itsStart.x - itsEnd.x)>0? (itsStart.x - itsEnd.x+2):(-(itsStart.x - itsEnd.x)+2));
				height = 2;
			}			
			else{
				height = ((itsStart.y-itsEnd.y)>0? (itsStart.y-itsEnd.y+2):(-(itsStart.y-itsEnd.y)+2));
				width = 2;
			}
			//ovviamente fare tutte le correzioni sulla grandezza delle regioni e sulla selezione delle linee
			
		}
		else{
			width = ((itsStart.x - itsEnd.x)>0? (itsStart.x - itsEnd.x+2):(-(itsStart.x - itsEnd.x)+2));
			height = ((itsStart.y-itsEnd.y)>0? (itsStart.y-itsEnd.y+2):(-(itsStart.y-itsEnd.y)+2));
		}
		preferredSize = new Dimension(width, height);
		itsX = ((itsStart.x < itsEnd.x)? (itsStart.x-1): (itsEnd.x-1));
		itsY = ((itsStart.y < itsEnd.y)? (itsStart.y-1): (itsEnd.y-1));
	}
	
	//--------------------------------------------------------
    //	GetStartPoint
    //--------------------------------------------------------
	public Point GetStartPoint() { 
		return (itsStart);
	}
	
	//--------------------------------------------------------
    //	GetEndPoint
    //--------------------------------------------------------
	public Point GetEndPoint() { 
		return (itsEnd);
	}
	
	//--------------------------------------------------------
    //	GetConnection
    //--------------------------------------------------------
	public ErmesConnection GetConnection() {
		return itsConnection;
	}
	
	//--------------------------------------------------------
    //	paint
    //--------------------------------------------------------
	public void Paint(Graphics g)
	{
		debug_countP += 1;
		if (debug_countP % ErmesSketchPad.DEBUG_COUNT == 0) {
		  /* System.out.print("uscito in JSegement.Paint ");
			System.out.println(debug_countP); */
		}
		if(itsConnection.GetErrorState()) g.setColor(Color.darkGray);
		else g.setColor(Color.black);
		
		if(itsConnection.GetSelected())
			g.fillRect(itsX, itsY, preferredSize.width, preferredSize.height);
		else
			g.drawLine(itsStart.x, itsStart.y, itsEnd.x, itsEnd.y);
	}

	//--------------------------------------------------------
    //	NewPoints
    //--------------------------------------------------------
	public void NewPoints(Point theStart, Point theEnd)
	{
		itsStart.x = theStart.x;
		itsStart.y = theStart.y;
		itsEnd.x = theEnd.x;
		itsEnd.y = theEnd.y;
	}
	
	//--------------------------------------------------------
    //	NearToPoint
    //--------------------------------------------------------
	boolean NearToPoint(int x, int y){
		int dx = java.lang.Math.abs(itsEnd.x - itsStart.x);
		int dy = java.lang.Math.abs(itsEnd.y - itsStart.y);
		
		if (dx > dy) return (SegmentIntersect(x, y-3, x, y+3, itsStart.x,
		 									 itsStart.y,itsEnd.x, itsEnd.y));
		else return (SegmentIntersect(x-3, y, x+3, y, itsStart.x, itsStart.y,
							   		  itsEnd.x, itsEnd.y));
	}
	
	//--------------------------------------------------------
    //	SegmentIntersect
    //--------------------------------------------------------
	boolean SegmentIntersect(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4){
		int dx21 = x2 - x1;
		int dx31 = x3 - x1;
		int dx41 = x4 - x1;
		int dy21 = y2 - y1;
		int dy31 = y3 - y1;
		int dy41 = y4 - y1;
		int det1 = dx31*dy21 - dy31*dx21;
		int det2 = dx41*dy21 - dy41*dx21;
		
		if ((det1 == 0)&&(det2 == 0)){
			if ((y1 == y2)&&(y1 == y3)&&(y1 == y4))
				return (Scambio(x1, x2, x3, x4));
			else return (Scambio(y1, y2, y3, y4));
		}
		else {
			int dx32 = x3 - x2;
			int dx34 = x3 - x4;
			int dy32 = y3 - y2;
			int dy34 = y3 - y4;
			int det3 = dx34*dy31 - dy34*dx31;
			int det4 = dx34*dy32 - dy34*dx32;
			return ((((det1<=0)&&(det2>=0))||((det1>=0)&&(det2<=0)))&&
					(((det3<=0)&&(det4>=0))||((det3>=0)&&(det4<=0))));
		}
	}
	
	//--------------------------------------------------------
    //	Scambio
    //--------------------------------------------------------
	boolean Scambio(int x1, int x2, int x3, int x4){
		int temp;
		if (x2 < x1){
			temp = x2;
			x2 = x1; 
			x1 = temp;
		}
		if (x4 < x3){
			temp = x4; 
			x4 = x3; 
			x3 = temp;
		}
		return ((x2 > x3)&&(x4 > x1));
	}
	
	
	//--------------------------------------------------------
    //	UpdateStartPt
    //--------------------------------------------------------
	public void UpdateStartPt(int theDeltaH, int theDeltaV){
		itsStart.y+=theDeltaV;
		itsStart.x+=theDeltaH;
	}
	
	//--------------------------------------------------------
    //	UpdateEndPt
    //--------------------------------------------------------
	public void UpdateEndPt(int theDeltaH, int theDeltaV){
		itsEnd.y+=theDeltaV;
		itsEnd.x+=theDeltaH;
	}
	
	//--------------------------------------------------------
    //	VersoAlto
    //--------------------------------------------------------
	public boolean VersoAlto(){
		if(itsEnd.y < itsStart.y)
			return true;
		else return false;
	}
	
	//--------------------------------------------------------
    //	VersoDestra
    //--------------------------------------------------------
	public boolean VersoDestra(){
		if(itsEnd.x > itsStart.x)
			return true;
		else return false;
	}
	
	//--------------------------------------------------------
    //	UpdatePreferredSize
    //--------------------------------------------------------
	public void UpdatePreferredSize(int theDeltaX, int theDeltaY){
		preferredSize.width += theDeltaX;
		preferredSize.height += theDeltaY;
	}
	
	//--------------------------------------------------------
    //	IsInverted
    //--------------------------------------------------------
	public boolean IsInverted(boolean theFirst, boolean theHor, int theDelta){
		Point aOldStart, aOldEnd;
		aOldStart = itsStart;
		aOldEnd = itsEnd;
		if(!theHor){
			if((theDelta>0)&&(theFirst)&&(VersoAlto()))//1
				if(itsStart.y<itsEnd.y+theDelta){
					itsEnd.y+=theDelta;
					Reshape(itsX,itsStart.y-1, preferredSize.width,itsEnd.y - itsStart.y + 2);
					return true;
				 }
				else return false;
			else 
				if((theDelta<0)&&(theFirst)&&(!VersoAlto()))//2
					if(itsStart.y>itsEnd.y+theDelta){
						itsEnd.y+=theDelta;
						Reshape(itsX,itsEnd.y-1, preferredSize.width,itsStart.y - itsEnd.y + 2);
						return true;
					}
					else return false;
				else
					if((theDelta>0)&&(!theFirst)&&(!VersoAlto()))//3
						if(itsEnd.y<itsStart.y+theDelta){ 
							itsStart.y+=theDelta;
							Reshape(itsX,itsEnd.y-1, preferredSize.width,itsStart.y - itsEnd.y + 2);
							return true;
						}
						else return false;
					else
						if((theDelta<0)&&(!theFirst)&&(VersoAlto())) //4
							if(itsEnd.y>itsStart.y+theDelta){
								itsStart.y+=theDelta;
								Reshape(itsX,itsStart.y-1, preferredSize.width,itsEnd.y-itsStart.y + 2);
								return true;
							}
							else return false;
		}
		else{
			if((theDelta<0)&&(theFirst)&&(VersoDestra())) 
				if(itsStart.x>itsEnd.x+theDelta){
					itsEnd.x+=theDelta;
					Reshape(itsEnd.x-1,itsY, itsStart.x-itsEnd.x+2,preferredSize.height);
					return true;
				}
				else return false;
			else
				if((theDelta>0)&&(theFirst)&&(!VersoDestra())) 
					if(itsStart.x<itsEnd.x+theDelta){
						itsEnd.x+=theDelta;
						Reshape(itsStart.x-1,itsY, itsEnd.x - itsStart.x+2,preferredSize.height);
						return true;
					}
					else return false;
				else
					if((theDelta>0)&&(!theFirst)&&(VersoDestra())) 
						if(itsEnd.x<itsStart.x+theDelta){
							itsStart.x+=theDelta;
							Reshape(itsEnd.x-1,itsY, itsStart.x - itsEnd.x+2,preferredSize.height);
							return true;
						}
						else return false;
					else
						if((theDelta<0)&&(!theFirst)&&(!VersoDestra())) 
							if(itsEnd.x>itsStart.x+theDelta){
								itsStart.x+=theDelta;
								Reshape(itsStart.x-1,itsY, itsEnd.x - itsStart.x+2,preferredSize.height);
								return true;
							}
							else return false;
		}	
		//maremma itsConnection.GetSketchPad().repaint();
		//itsConnection.Paint(itsConnection.GetSketchPad().getGraphics());	///????????,???????
		return false;
	}
	
	//--------------------------------------------------------
    //	Reshape
    //--------------------------------------------------------
	public void Reshape(int theX, int theY, int theWidth, int theHeight){
		itsX = theX;
		itsY = theY;
		preferredSize.width = theWidth;
		preferredSize.height = theHeight;
	}
	
	//--------------------------------------------------------
    //	Bounds
    //--------------------------------------------------------
	public Rectangle Bounds(){
		return new Rectangle(itsX, itsY, preferredSize.width, preferredSize.height);
	}
	
	//--------------------------------------------------------
    //	minimumSize
    //--------------------------------------------------------
	 public Dimension minimumSize() {
        return preferredSize;
    }
    
    //--------------------------------------------------------
    //	preferredSize
    //--------------------------------------------------------
    public Dimension preferredSize() {
        return preferredSize;
    }
    
    //--------------------------------------------------------
    //	MoveBy
    //--------------------------------------------------------
    public void MoveBy(int theDeltaH, int theDeltaV) {
		int j;
		ErmesObjInlet aInlet;
		ErmesObjOutlet aOutlet;
		
		if (theDeltaH == 0 && theDeltaV == 0) return;
		itsX+=theDeltaH; itsY+=theDeltaV;
		itsStart.x+=theDeltaH; itsStart.y+=theDeltaV;
		itsEnd.x+=theDeltaH; itsEnd.y+=theDeltaV;
		//maremma itsConnection.GetSketchPad().repaint();	
		//itsConnection.Paint(itsConnection.GetSketchPad().getGraphics());	
		//Paint(itsConnection.GetSketchPad().getGraphics());//?????????
	}
}
