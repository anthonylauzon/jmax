//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//
package ircam.jmax.toolkit;

/**
 * an utility class to handle a generic graphic XOR movement.
 * Instructions:
 * create a new XORHandler passing it a client (the painter) and eventually
 * a form of updating (default: RELATIVE).
 * start it with the beginAt() method.
 * call moveTo() each time the position changes.
 * The form parameter identify the type of informations passed back to the
 * painter: RELATIVE: the cient's XORDraw will be called with
 * the offset relative to the last moveTo call.
 * ABSOLUTE: the offset is relative to the interaction's starting point.
 * Call end() when you've done with the movements.
 * This class actually takes care of calling the XORErase() and XORPaint()
 * methods of its client when needed.
 * Just one client is handled.
 */
public class XORHandler {

  /** 
   * Constructor.
   */
  public XORHandler(XORPainter theClient) 
  {
    itsClient = theClient;
    form = RELATIVE;
  }

  /** 
   * Constructor.
   */
  public XORHandler(XORPainter theClient, int theForm) 
  {
    itsClient = theClient;
    form = theForm;
  }

  /**
   * set the starting point of the movements
   */
  public void beginAt(int x, int y) 
  {
    firstX = x;
    firstY = y;
    oldX=x;
    oldY = y;
    updated = true;
  }

  /**
   * communicates that the point has moved
   */
  public void moveTo(int x, int y) 
  {
    if (!updated) itsClient.XORErase();
    if (form == RELATIVE)
      itsClient.XORDraw(x-oldX, y-oldY);
    else itsClient.XORDraw(x-firstX, y-firstY);
    
    updated = false;
    oldX = x;
    oldY = y;
  }

  /**
   * stop the interaction
   */
  public void end() 
  {
    itsClient.XORErase();
  }

  //--- Fields
  int oldX;
  int oldY;
  int firstX;
  int firstY;

  XORPainter itsClient;

  boolean updated;

  int form;
  public static final int ABSOLUTE = 0;
  public static final int RELATIVE = 1;
}

