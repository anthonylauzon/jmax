//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 
// Based on Max/ISPW by Miller Puckette.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 


package ircam.jmax.editors.explode;

/**
 * <p>
 * <b>Some note about this package</b>
 *<p>
 * The main class (the editor) is <a href="ircam.jmax.editors.explode.Explode.html">Explode</a>, wich is created, as usual, with a MaxData instance.
 * This class then get the associated <a href="ircam.jmax.editors.explode.ExplodeRemoteData.html">ExplodeRemoteData</a>, that is a class that implements the <a href="ircam.jmax.editors.explode.ExplodeDataModel.html">ExplodeDataModel</a> interface.
 * The editor then creates a panel in wich to display this data (<a href="ircam.jmax.editors.explode.ScrPanel.html">ScrPanel</a>)
 * <p>
 * <b> The model of data: ExplodeDataModel.</b>
 *<p>
 * The interface <a href="ircam.jmax.editors.explode.ExplodeDataModel.html">ExplodeDataModel</a> defines the methods needed to access a generic explode-like database. It is currently implemented just by <a href="ircam.jmax.editors.explode.ExplodeRemoteData.html">ExplodeRemoteData</a>, but could be implemented by other classes (ex. a MIDI file reader). The ExplodeRemoteData class actually represents the data contained in a FTS explode object.
 * The data is supposed to be an sequence of  <a href="ircam.jmax.editors.explode.ScrEvent.html">ScrEvent</a>, order in increasing starting time. 
 * The model offers the usual adding, removing functions, as well as direct access to the events via index, and some utility functions (es. indexOfFirstEventStartingAfter(time), used during editing).
 * Modification to the data base are communicated by the means of the <a href="ircam.jmax.editors.explode.ExplodeDataListener.html">ExplodeDataListener</a> interface.
 * The single <a href="ircam.jmax.editors.explode.ScrEvent.html">ScrEvent</a> record contains the score data (time, pitch, velocity, channel..) as well as a set of accessors. 
 * The ScrEvent class contains no graphic informations.
 * <p>
 * <b> The corrispondence between score parameters and graphic parameters: the Adapters </b>
 * <p>
 * An <a href="ircam.jmax.editors.explode.Adapter.html">Adapter</a> is essentially a converter between Score and graphic events. It implements a mapping between the explode parameters (pitch, duration...) and visual parameters (x, y, width...).
 * This allows the separation of an "explode event" from its graphic representation. The Adapter class uses a set of objects of type <a href="ircam.jmax.editors.explode.Mapper.html">Mapper</a> each one representing a given graphic parameter. 
 * So, for example, the <a href="ircam.jmax.editors.explode.PartitionAdapter.html">PartitionAdapter</a> class uses a <a href="ircam.jmax.editors.explode.TimeMapper.html">TimeMapper</a> for the X coordinates, a <a href="ircam.jmax.editors.explode.PitchMapper.html">PitchMapper</a> for the Y coordinates, and a <a href="ircam.jmax.editors.explode.DurationMapper.html">DurationMapper</a> for the Lenght. This is the typical piano-roll association.
 * An adapter gives also the inverse conversion, that is: starting from a graphic value, it returns the value of the score parameter associated with it.
 * The <a href="ircam.jmax.editors.explode.PartitionAdapter.html">PartitionAdapter</a> class offers also services to make a cordinate conversion on the event's representation. It allows infact zooming, transposition, inversion of coordinates.
 *<p>
 *<b> The representation of the score: Renderers, EventRenderers and their services</b>
 *<p>
 * A <a href="ircam.jmax.editors.explode.Renderer.html">Renderer</a> is the abstract interface implemented by the objects that represent graphically a sequence of ScrEvents. Its render() method is called when the score needs to be repainted. A concrete implementation of this interface is <a href="ircam.jmax.editors.explode.ScoreRenderer.html">ScoreRenderer</a>
 * A Renderer have the responsibility of draw the background (if any) and drawing the needed subset of events. Each object is drawn by the mean of an <a href="ircam.jmax.editors.explode.EventRenderer.html">EventRenderer</a>.
 * An EventRender knows the exact representation of an event, and then it offers services like contains(event, x, y), a method that returns true if the given event contains the given (graphic) point. The EventRenderer used by the <a href="ircam.jmax.editors.explode.ScoreRenderer.html">ScoreRenderer</a> is the class <a href="ircam.jmax.editors.explode.PartitionEventRenderer.html">PartitionEventRenderer</a>
 * The Renderer offers also services like eventsIntersecting(int, int, int, int) that returns an enumeration of all the events whose graphic representation intersects the given rectangle.
 * <p>
 * <b> Handling the interaction with the user: Tools and InterfaceModules </b>
 * <p>
 * The operations in the editor are performed by tools (<a href="ircam.jmax.editors.explode.ScrTool.html">ScrTool</a>): only one tool can be active at a time, and during its acivity a tool receives all the events coming from the user. A tool is a simple object that is associated to a name, an ImageIcon (for toolbar representations), and that has method for its activation and the deactivation. A tool can be roughly seen as the set of functionalities offered by a button in the toolbar.
 * Example of tools are <a href="ircam.jmax.editors.explode.ScrAddingTool.html">ScrAddingTool</a>, a tool used to add notes in the score, <a href="ircam.jmax.editors.explode.ArrowTool.html">ArrowTool</a>, the tool that implements the set of operation represented by the "arrow icon" in the palette,
 * <a href="ircam.jmax.editors.explode.DeleteTool.html">DeleteTool</a>, the tool used to delete notes, and <a href="ircam.jmax.editors.explode.MoverTool.html">MoverTool</a>, used to move notes around.
 * Actually, the Mover tool is a unique tool that is instanciated twice in the toolbar, with different creation parameters (VERTICAL, HORIZONTAL) and different icons.
 * <p>
 * The interaction with the user is handled by the tool's current <a href="ircam.jmax.editors.explode.InteractionModule.html">InteractionModule</a>. A tool can mount different <a href="ircam.jmax.editors.explode.InteractionModule.html">InteractionModules</a> in order to accomplish its task. An Interaction module is the only component that actually receives the awt events, and that actually draws.
 * The <a href="ircam.jmax.editors.explode.ScrAddingTool.html">ScrAddingTool</a>, for example, mounts its default tool, a <a href="ircam.jmax.editors.explode.MouseTracker.html">MouseTracker</a> and just waits to be notified when the user chooses a point on the screen. 
 * In an analogous way, the Arrow tool mounts its <a href="ircam.jmax.editors.explode.Selecter.html">Selecter</a> interface module in order to draw the rubber-banding rectangle on the screen, being informed at the end of the interaction on the rectangle selected by the user.
 * An interface module communicates just the result of an interaction to the tool (its listener). The <a href="ircam.jmax.editors.explode.MouseTracker.html">MouseTracker</a> for example, communicates the point choosen to its <a href="ircam.jmax.editors.explode.PositionListener.html">PositionListener</a>
 * The <a href="ircam.jmax.editors.explode.Selecter.html">Selecter</a> draws the rubber band, and communicates the rectangle choosen to its <a href="ircam.jmax.editors.explode.SelectionListener.html">SelectionListener</a>
 * <p>
 * <b> The toolbar </b>
 * <p> A Toolbar (<a href="ircam.jmax.editors.explode.ScrToolbar.html">ScrToolbar</a>), is a mean for the user to select the tools.
 * A toolbar uses  a <a href="ircam.jmax.editors.explode.ToolbarProvider.html">ToolbarProvider</a>, its creation argument, in order to fill the buttons. This simple interface returns an enumeration of tools via its getTools() method. The functionalities offered by a toolbar are the insertion of a tool, and the handling of the listeners (<a href="ircam.jmax.editors.explode.ToolListener.html">ToolListener</a>) for the <a href="ircam.jmax.editors.explode.ToolChangeEvent.html">ToolChangeEvent</a>s produced by the toolbar.
 * The only ToolbarProvider in the current system is the ScrPanel.
 * 
 */ 
public interface AAAReadme {
}

