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
package ircam.jmax.editors.ermes;

class InletOutletSensibilityArea extends SensibilityArea {
  static int height = 4;

  void setNumber( int n)
    {
      this.n = n;
    }

  int getNumber()
    {
      return n;
    }

  private int n = 0;
}
