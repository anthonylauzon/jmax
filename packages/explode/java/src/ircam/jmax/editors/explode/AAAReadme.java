
package ircam.jmax.editors.explode;

/**
 * <b>this is not a real interface, just a placeholder for documentation</b>
 * <p>
 * some note about this package:<p>
 * The class <a href="explode.ScrTest.html">ScrTest</a>  (score test) contains the main method;its sole function
 * is to create a window with a <a href="explode.ScrPanel.html">ScrPanel</a>  inside.<p>
 * This panel has its database (an object of the <a href="explode.Explode.html">Explode</a> class, containing the score's data, that is, objects of type <a href="explode.ScrEvent.html">ScrEvent</a>),
 * its <a href="explode.Renderer.html">Renderer</a> component (the default is an object of the <a href="explode.ScoreRenderer.html">ScoreRenderer</a> class), and an <a href="explode.Accessor.html">Accessor</a>, by default the specialized accessor <a href="explode.PartitionAccessor.html">PartitionAccessor</a>, that maps the graphic parameter (x, y, lenght, color...) to the Score event's field (time, pitch, duration...)<p>
 * A Renderer paints the background, and the events in the score using  objects that implements the <a href="explode.EventRenderer.html">EventRenderer</a> interface. The eventRenderer for the ScoreRenderer
 * class is the <a href="explode.PartitionEventRenderer.html">PartitionEventRenderer</a>, that draws an
 * event in the partition style (black note on a pentagram, no lenght, no labels).<p>
 * The ScrPanel creates a toolbar (<a href="explode.ScrToolbar.html">ScrToolbar</a>), that is a container of tools (<a href="explode.ScrTool.html">ScrTool</a>)
 *
 * The interaction between the panel and the toolbar is as follows:
 * the panel creates the tool it will need during the interaction, that is 
 * 1) an adding tool (<a href="explode.ScrAddingTool.html">ScrAddingTool</a>) and 
 * 2) a selecting tool (<a href="explode.ScrSelectingTool.html">ScrSelectingTool</a>)
 * It then calls the toolbar.addTool() method passing it an icon and a tool.
 * The icon will determine the graphic appearence of the tool in the toolbar.
 * After that, the panel become a listener of the toolbar (<a href="explode.ToolListener.html">ToolListener</a>) and will
 * be called when the userer selects another tool.
 * The tools simply handle the user interaction, communicating with the panel.
 */ 
public interface AAAReadme {
}
