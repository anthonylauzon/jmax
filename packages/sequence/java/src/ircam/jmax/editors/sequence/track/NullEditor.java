
package ircam.jmax.editors.sequence.track;

/**
 * An editor that does nothing */
public class NullEditor extends ValueEditor{

    static NullEditor getInstance()
    {
	return instance;
    }

    static NullEditor instance = new NullEditor();
}
