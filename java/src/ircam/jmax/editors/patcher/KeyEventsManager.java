//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 

package ircam.jmax.editors.patcher;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.io.*;

import ircam.jmax.*;
import ircam.fts.client.*;

public class KeyEventsManager implements KeyListener
{
  public static KeyEventsManager keyEventsManager = null;  
  private FtsObject ftsKeyServer;
  protected FtsArgs args = new FtsArgs();

  public static void createManager()
  {
      if(keyEventsManager == null)
	  keyEventsManager = new KeyEventsManager();  
  }

  public KeyEventsManager()
  {
    try
      {
	ftsKeyServer = new FtsObject(JMaxApplication.getFtsServer(), JMaxApplication.getRootPatcher(), FtsSymbol.get("_keyserver"));
      }
    catch(IOException e)
      {
	System.err.println("[KeyEventsManager]: Error in KeyServer creation!");
	e.printStackTrace();
	}      
  }
    
  static public KeyEventsManager getManager()
  {
    if(keyEventsManager == null)
      createManager();
    return keyEventsManager;
  }
  
  private int getCode(KeyEvent e)
  {
    int code = 0;

    switch(e.getKeyCode())
      {
      case KeyEvent.VK_SHIFT:
	code = SHIFT_CODE;
	break;
      case KeyEvent.VK_ALT:
      case KeyEvent.VK_META:
	code = ALT_CODE;
	break;
      case KeyEvent.VK_F1:
	code = F1_CODE;
	break;
      case KeyEvent.VK_F2:
	code = F2_CODE;
	break;
      case KeyEvent.VK_F3:
	code = F3_CODE;
	break;
      case KeyEvent.VK_F4:
	code = F4_CODE;
	break;
      case KeyEvent.VK_F5:
	code = F5_CODE;
	break;
      case KeyEvent.VK_F6:
	code = F6_CODE;
	break;
      case KeyEvent.VK_F7:
	code = F7_CODE;
	break;
      case KeyEvent.VK_F8:
	code = F8_CODE;
	break;
      case KeyEvent.VK_F9:
	code = F9_CODE;
	break;
      case KeyEvent.VK_F10:
	code = F10_CODE;
	break;
      case KeyEvent.VK_F11:
	code = F11_CODE;
	break;
      case KeyEvent.VK_F12:
	code = F12_CODE;
	break;
      case KeyEvent.VK_ENTER:
	code = ENTER_CODE;
	break;
      case KeyEvent.VK_ESCAPE:
	code = ESC_CODE;
	break;
      case KeyEvent.VK_ADD:
      case KeyEvent.VK_SUBTRACT:
      case KeyEvent.VK_MULTIPLY:
      case KeyEvent.VK_DIVIDE:
	break;
      default:
	{
	  int c = (int)e.getKeyChar();

	  if (c >= 32 && c < 127)
	    code = c;
	  break;
	}
      }	

    return code;
  }

  /*
   * KeyListener interface
   */
  public void keyPressed(KeyEvent e)
  {
    if (e.isControlDown()) 
      return;
 
    int code = getCode(e);

    if ( code != 0)
      {
	  args.clear();
	  args.addInt(code);
	  args.addInt(1);
	  
	  try{
	      ftsKeyServer.send( FtsSymbol.get("key"), args);
	  }
	  catch(IOException exc)
	      {
		  System.err.println("KeyEventManager: I/O Error sending key Message!");
		  exc.printStackTrace(); 
	      }
      }
  }   
 
  public void keyReleased(KeyEvent e)
  {
    if (e.isControlDown()) 
      return;

    int code = getCode(e);

    if ( code != 0)
      {
	  args.clear();
	  args.addInt(code);
	  args.addInt(0);
	  
	  try{
	      ftsKeyServer.send(  FtsSymbol.get("key"), args);
	  }
	  catch(IOException exc)
	      {
		  System.err.println("KeyEventManager: I/O Error sending key Message!");
		  exc.printStackTrace(); 
	      }
      }
  }

  public void keyTyped(KeyEvent e){}
    

  ////////////// Producers /////////////////////
  static public void addProducer(Component producer)
  {
    producer.addKeyListener(getManager());
  }
  static public void removeProducer(Component producer)
  {
    producer.removeKeyListener(getManager());
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
