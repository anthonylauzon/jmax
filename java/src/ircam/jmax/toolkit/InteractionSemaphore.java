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

package ircam.jmax.toolkit;


public class InteractionSemaphore {

    final static boolean locked()
    {
	return contextLocked;
    }


    /** Locks the current graphic context, i.e., no switches can occur until
     * the context is unlocked */ 
    public final static void lock()
    {
	contextLocked = true;
    }
    

    /** UnLocks the current graphic context. The last context switch asked and not served becomes
     * the current one, if any */ 
    public final static void unlock()
    {
	contextLocked = false;
	if (lastCaller != null) lastCaller.unlocked();
	lastCaller = null;
    }    

    final static void setCaller(LockListener l)
    {
	lastCaller = l;
    }

    static LockListener lastCaller;
    static boolean contextLocked = false;
    //static InteractionSemaphore instance = new InteractionSemaphore();

}






