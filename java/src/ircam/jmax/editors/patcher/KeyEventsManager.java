//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//
package ircam.jmax.editors.patcher;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.utils.*;

public class KeyEventsManager implements KeyListener
{
    public static KeyEventsManager keyEventsManager = new KeyEventsManager();  
    private FtsObject ftsKeyServer;
    private boolean noserver = false;

    public static FtsAtom[] sendArgs = new FtsAtom[3];
    static
    {
	for(int i=0; i<3; i++)
	    sendArgs[i]= new FtsAtom();
    }

    public KeyEventsManager()
    {
	try 
	  {
	      Fts fts = MaxApplication.getFts();
	      ftsKeyServer = (FtsObject) fts.makeFtsObject( fts.getRootObject(), "_keyserver");
	  }
	catch (FtsException e) 
	    {
		System.err.println("Error in FtsKeyServer instantiation!");
		noserver = true;
	    }
    }
    
    static public KeyEventsManager getManager()
    {
	return keyEventsManager;
    }

    /*
     * KeyListener interface
     */
    public void keyPressed(KeyEvent e)
    {
	if(noserver) return;
 
	int code = e.getKeyCode();
	switch(code)
	    {
	    case KeyEvent.VK_SHIFT:
		sendArgs[0].setInt(SHIFT_CODE);
		break;
	    case KeyEvent.VK_CONTROL:
		sendArgs[0].setInt(CTRL_CODE);
		break;
	    case KeyEvent.VK_ALT:
		sendArgs[0].setInt(ALT_CODE);
		break;
	    case KeyEvent.VK_F1:
		sendArgs[0].setInt(F1_CODE);
		break;
	    case KeyEvent.VK_F2:
		sendArgs[0].setInt(F2_CODE);
		break;
	    case KeyEvent.VK_F3:
		sendArgs[0].setInt(F3_CODE);
		break;
	    case KeyEvent.VK_F4:
		sendArgs[0].setInt(F4_CODE);
		break;
	    case KeyEvent.VK_F5:
		sendArgs[0].setInt(F5_CODE);
		break;
	    case KeyEvent.VK_F6:
		sendArgs[0].setInt(F6_CODE);
		break;
	    case KeyEvent.VK_F7:
		sendArgs[0].setInt(F7_CODE);
		break;
	    case KeyEvent.VK_F8:
		sendArgs[0].setInt(F8_CODE);
		break;
	    case KeyEvent.VK_F9:
		sendArgs[0].setInt(F9_CODE);
		break;
	    case KeyEvent.VK_F10:
		sendArgs[0].setInt(F10_CODE);
		break;
	    case KeyEvent.VK_F11:
		sendArgs[0].setInt(F11_CODE);
		break;
	    case KeyEvent.VK_F12:
		sendArgs[0].setInt(F12_CODE);
		break;
	    case KeyEvent.VK_ENTER:
		sendArgs[0].setInt(ENTER_CODE);
		break;
	    case KeyEvent.VK_ESCAPE:
		sendArgs[0].setInt(ESC_CODE);
		break;
	    default:
		sendArgs[0].setInt(code);
	    }	

	sendArgs[1].setInt(1);
	ftsKeyServer.sendMessage(FtsObject.systemInlet, "key", 2, sendArgs);
    }    
    public void keyReleased(KeyEvent e)
    {
	if(noserver) return;

	int code = e.getKeyCode();
	switch(code)
	    {
	    case KeyEvent.VK_SHIFT:
		sendArgs[0].setInt(SHIFT_CODE);
		break;
	    case KeyEvent.VK_CONTROL:
		sendArgs[0].setInt(CTRL_CODE);
		break;
	    case KeyEvent.VK_ALT:
		sendArgs[0].setInt(ALT_CODE);
		break;
	    case KeyEvent.VK_F1:
		sendArgs[0].setInt(F1_CODE);
		break;
	    case KeyEvent.VK_F2:
		sendArgs[0].setInt(F2_CODE);
		break;
	    case KeyEvent.VK_F3:
		sendArgs[0].setInt(F3_CODE);
		break;
	    case KeyEvent.VK_F4:
		sendArgs[0].setInt(F4_CODE);
		break;
	    case KeyEvent.VK_F5:
		sendArgs[0].setInt(F5_CODE);
		break;
	    case KeyEvent.VK_F6:
		sendArgs[0].setInt(F6_CODE);
		break;
	    case KeyEvent.VK_F7:
		sendArgs[0].setInt(F7_CODE);
		break;
	    case KeyEvent.VK_F8:
		sendArgs[0].setInt(F8_CODE);
		break;
	    case KeyEvent.VK_F9:
		sendArgs[0].setInt(F9_CODE);
		break;
	    case KeyEvent.VK_F10:
		sendArgs[0].setInt(F10_CODE);
		break;
	    case KeyEvent.VK_F11:
		sendArgs[0].setInt(F11_CODE);
		break;
	    case KeyEvent.VK_F12:
		sendArgs[0].setInt(F12_CODE);
		break;
	    case KeyEvent.VK_ENTER:
		sendArgs[0].setInt(ENTER_CODE);
		break;
	    case KeyEvent.VK_ESCAPE:
		sendArgs[0].setInt(ESC_CODE);
		break;
	    default:
		sendArgs[0].setInt(code);
	    }
	sendArgs[1].setInt(0);
	ftsKeyServer.sendMessage(FtsObject.systemInlet, "key", 2, sendArgs);
    }
    public void keyTyped(KeyEvent e){}
    

    ////////////// Producers /////////////////////
    static public void addProducer(Component producer)
    {
	producer.addKeyListener(keyEventsManager);
    }
    static public void removeProducer(Component producer)
    {
	producer.removeKeyListener(keyEventsManager);
    }

    //////////////////////////////// key codes ////////////////
    static final int F1_CODE   = 128;
    static final int F2_CODE   = 129;
    static final int F3_CODE   = 130;
    static final int F4_CODE   = 131;
    static final int F5_CODE   = 132;
    static final int F6_CODE   = 133;
    static final int F7_CODE   = 134;
    static final int F8_CODE   = 135;
    static final int F9_CODE   = 136;
    static final int F10_CODE  = 137;
    static final int F11_CODE  = 138;
    static final int F12_CODE  = 139;
    static final int ENTER_CODE = 140;
    static final int ESC_CODE   = 141;
    static final int SHIFT_CODE = 142;
    static final int ALT_CODE   = 143;
    static final int CTRL_CODE  = 144;
    
}









