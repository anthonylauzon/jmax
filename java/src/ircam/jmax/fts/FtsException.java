//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// See file COPYING.LIB for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 

package ircam.jmax.fts;

/** 
 * The generic exception for fts.
 * Very primitive at the moment, just keep a string explaining the detail
 * of the exception; should at least embed the native exception generating
 * the original error (if any), and should may be code also the 
 * gravity of the situation (style, should the server be just closed, should
 * the user be warned, can we safely abort the current operation but continue
 * to use the server, and so on).
 */

public class FtsException extends Exception
{
  FtsException(String detail)
  {
    super(detail);
  }
}
