
package ircam.jmax.editors.sequence;

/**
 * An enumeration that returns primitive (positive) int instead of Objects */
public interface IntEnumeration
{
    public abstract boolean hasMoreElements();
    public abstract int nextElement();
}
