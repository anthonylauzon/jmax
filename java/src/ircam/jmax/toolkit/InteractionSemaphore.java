
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






