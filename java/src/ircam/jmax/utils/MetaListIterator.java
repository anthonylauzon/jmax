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

package ircam.jmax.utils;

import java.util.*;

public class MetaListIterator extends ListIterator{

  public MetaListIterator(List l) {
    super(l);
  }

  public Object nextElement() {
    ListObject temp = itsPointer;

    if (itsPointer == null) return null;//what's going on here?
    itsPointer = itsPointer.next;
    return ((ListObject)(temp.itsObject)).itsObject;
  }

  public Object previousElement() {

    if (itsPointer == null) return null;//what's going on here?
    itsPointer = itsPointer.previous;
    return ((ListObject)itsPointer.itsObject).itsObject;
  }

}
