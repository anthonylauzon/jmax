
package ircam.jmax.editors.sequence.track;

/**
 * The (super)class of the objects used to edit TrackEvents.
 * The paradigm here is like the Renderers: a TrackEvent
 * have a EventValue, who provides a render that knows how to represent
 * it. This EventValue provides also an Editor that can open a 
 * specialized panel, or so, to modify parameters that are not
 * the standard time and duration params. */
public class ValueEditor
{
    public void startEdit(TrackEvent event)
    {
    }
}
