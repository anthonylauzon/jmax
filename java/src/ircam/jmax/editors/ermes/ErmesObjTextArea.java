package ircam.jmax.editors.ermes;

import java.awt.*;
import ircam.jmax.utils.*;

/**
 * The text area contained in a "comment" object (ErmesObjComment)
 */
class ErmesObjTextArea extends TextArea {
	static String filler = "                    ";
	ErmesObjComment itsOwner;
	boolean	focused = false;	//just an hack since AWT sends me several 'lostfocus'
	Dimension preferredRCSize;
	int DEFAULT_COLS = 20;

	//--------------------------------------------------------
	// CONSTRUCTOR
    //--------------------------------------------------------
	ErmesObjTextArea(int lines, int rows, ErmesObjComment theOwner) {
		super(" ", lines, rows);
		preferredRCSize = new Dimension(lines, rows);
		itsOwner = theOwner;
		setFont(new Font(ircam.jmax.utils.Platform.FONT_NAME, Font.PLAIN, ircam.jmax.utils.Platform.FONT_SIZE));
		setEditable(true);	//e.m.	18/03
		selectAll();
		focused = true;
	}
	
  	public boolean lostFocus(Event evt, Object what) {
  		/*if (!focused) return false;
  		else focused = false;*/
  		String pippazza = getText();
  		if (pippazza.compareTo("") == 0 || pippazza.compareTo(" ") == 0) return false; //(immediately)
		//if (itsOwner.itsFtsObject != null) { nothing to do for now with fts objects
		//}
  		itsOwner.itsArgs = getText();
		hide();
		move(-2000,-2000);	//go away please
		if (itsOwner.itsSketchPad != null) {
			itsOwner.ReComputeSizes(itsOwner.itsArgs, DEFAULT_COLS);
			itsOwner.Paint(itsOwner.itsSketchPad.GetOffGraphics());
			itsOwner.itsSketchPad.CopyTheOffScreen(itsOwner.itsSketchPad.getGraphics());
		}
		//itsOwner = null;	//seems to be crazy but...
		return false;
  	}

	public void requestFocus() {
		super.requestFocus();
		gotFocus(null, null);	//si cerca di forzare l'assenza di "got focus" sulle text areas
	}
	
  	public boolean gotFocus(Event evt, Object what) {
  		//if (focused) return true;
  		//else focused = true;
  		/*if (getText().compareTo("") == 0) return true; //(immediately) (why?)
  		else*/ return false /*true; why?*/;
  	}

	//--------------------------------------------------------
	// keyDown()
    //--------------------------------------------------------
	/*public boolean keyDown(Event e, int k){
		appendText(getText()+(char) k);
		return false;
	}*/

	//--------------------------------------------------------
	// keyDown()
    //--------------------------------------------------------
	public boolean keyDown(Event e, int k){
		int lenght;
		String s1, s2;
		int start = getSelectionStart();
		int end = getSelectionEnd();
		String s = getText();
		FontMetrics fm = getFontMetrics(getFont());
	
		if (isEditable()) {
			if(k == ircam.jmax.utils.Platform.ENTER_KEY || k == ircam.jmax.utils.Platform.RETURN_KEY){//return
				/*Dimension d2 = itsOwner.size();
				d2.height += fm.getHeight();
				itsOwner.ResizeTo(d2);
				itsOwner.validate();*/
				return true;
			}
			else if(k == Event.LEFT){//freccia a sinistra
					if(start==end){
						if(start>0){
							if(e.shiftDown())
								select(start-1, start);
							else
								select(start-1,start-1);
						}
					}
					else{
						if(e.shiftDown())
							select(start-1, end);
						else
							select(start,start);
					}
				}
			else if(k == Event.RIGHT){//freccia a destra
					if(start==end){
						if(start < s.length()){
							if(e.shiftDown())
								select(start, end+1);
							else
								select(start+1,start+1);
						}
					}
					else{
						if(e.shiftDown())
							select(start, end+1);
						else
							select(end,end);
					}
				}
				else if((k == Event.UP)||(k == Event.DOWN))
						return true;
				else if (k == ircam.jmax.utils.Platform.DELETE_KEY || k == ircam.jmax.utils.Platform.BACKSPACE_KEY) {//cancellazione
						if(start==end){//se non c' testo selezionato
							if(start>0){
								if(start < s.length()){//cancella intermedio
									s1 = s.substring(0, start-1);
									s2 = s.substring(start, s.length());
									s = s1+s2;
								}
								else//cancella in coda
									s = s.substring(0, s.length()-1);
								setText(s);
								select(start-1,start-1);
							}
						}
						else{//se c' testo selezionato
							s1 = s.substring(0, start);
							s2 = s.substring(end, s.length());
							s = s1+s2;
							setText(s);
							select(start,start);
						}
						
						lenght = fm.stringWidth(s);
						if ((lenght< size().width-20)&&(lenght>itsOwner.minimumSize().width-20)){
							Dimension d1 = itsOwner.Size();
							d1.width -= 20;
							itsOwner.Resize1(d1.width, d1.height);
						}
					}
					else{//scrittura
						if(start!=end){//cancella selezione
							s1 = s.substring(0, start);
							s2 = s.substring(end, s.length());
							s = s1+s2;
							select(start,start);
						}
						if(start < s.length()){//inserisce testo intermedio
							s1 = s.substring(0, start);
							s2 = s.substring(start, s.length());
							s = s1+(char)k+s2;
							setText(s);
						}
						else//inserisce testo in coda
							s = s+(char)k;
						setText(s);
						select(start+1,start+1);
													
					}
		}
		return true;
	}

    //--------------------------------------------------------
	// minimumSize()
    //--------------------------------------------------------
    public Dimension minimumSize() {
    	Dimension r = itsOwner.preferredSize();
    	Dimension d = new Dimension(r.width-2, r.height-2);
        return d;
    }

    //--------------------------------------------------------
	// preferredSize()
    //--------------------------------------------------------
    public Dimension preferredSize() {
        return minimumSize();
    }
    
    //--------------------------------------------------------
	// minimumSize(rows, col)
    //--------------------------------------------------------
    public Dimension minimumSize(int row, int col) {
		return preferredRCSize;
    }

    //--------------------------------------------------------
	// preferredSize(rows, col)
    //--------------------------------------------------------
    public Dimension preferredSize(int row, int col) {
        return preferredRCSize;
    }

}




