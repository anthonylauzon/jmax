//
// FTS client library
// Copyright (C) 2001 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
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
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 

package ircam.ftsclient;

interface TransitionAction {
  public void fire( byte input);
}

class StringCache {
  StringCache( int initialCapacity)
  {
    cache = new String[initialCapacity];
  }

  StringCache()
  {
    this( 256);
  }

  void put( int index, String s)
  {
    if ( index >= cache.length)
      {
	int newLength = cache.length;

	while (newLength <= index)
	  newLength *= 2;

	String[] newCache = new String[newLength];
	System.arraycopy( cache, 0, newCache, 0, cache.length);
	cache = newCache;
      }

    cache[index] = s.intern();
  }

  String[] cache;
}

class State {
  class Transition {
    Transition( byte input, State state, TransitionAction action, Transition next)
    {
      this.input = input; 
      this.state = state;
      this.action = action;
      this.next = next;
    }

    byte input;
    State state;
    TransitionAction action;
    Transition next;
  }

  State( String name)
  {
    this.name = name;
  }

  void addTransition( byte input, State state, TransitionAction action)
  {
    transitions = new Transition( input, state, action, transitions);
  }

  void addTransition( State state, TransitionAction action)
  {
    defaultTransition = new Transition( (byte)0, state, action, null);
  }

  final State next( byte input)
  {
    Transition transition = transitions;

    while (transition != null)
      {
	if (transition.input == input)
	  {
	    transition.action.fire( input);
	    return transition.state;
	  }

	transition = transition.next;
      }

    if (defaultTransition != null)
      {
	defaultTransition.action.fire( input);
	return defaultTransition.state;
      }

    return null;
  }

  public String toString()
  {
    return name;
  }

  private String name;
  private Transition transitions;
  private Transition defaultTransition;
}

class FtsProtocolDecoder {

  private void buildStateMachine()
  {
    TransitionAction clearAction = new TransitionAction() {
	public void fire( byte input)
	{
	  ival = 0;
	}
      };

    TransitionAction shiftAction = new TransitionAction() {
	public void fire( byte input)
	{
	  ival = ival << 8 | input;
	}
      };

    TransitionAction bufferClearAction = new TransitionAction() {
	public void fire( byte input)
	{
	  buffer.setLength( 0);
	}
      };

    TransitionAction bufferShiftAction = new TransitionAction() {
	public void fire( byte input)
	{
	  buffer.append( (char)input);
	}
      };

    TransitionAction endTargetAction = new TransitionAction() {
	public void fire( byte input)
	{
	  ival = ival << 8 | input;
	  target = server.getObject( ival);
	}
      };

    TransitionAction endSelectorAction = new TransitionAction() {
	public void fire( byte input)
	{
	  selector = buffer.toString();
	  args.clear();
	}
      };

    TransitionAction endIntArgAction = new TransitionAction() {
	public void fire( byte input)
	{
	  ival = ival << 8 | input;
	  args.add( ival);
	}
      };

    TransitionAction endFloatArgAction = new TransitionAction() {
	public void fire( byte input)
	{
	  ival = ival << 8 | input;
	  args.add( Float.intBitsToFloat(ival) );
	}
      };

    TransitionAction endStringArgAction = new TransitionAction() {
	public void fire( byte input)
	{
	  args.add( buffer.toString());
	}
      };

    TransitionAction endObjectArgAction = new TransitionAction() {
	public void fire( byte input)
	{
	  ival = ival << 8 | input;
	  args.add( server.getObject( ival));
	}
      };

    TransitionAction endMessageAction = new TransitionAction() {
	public void fire( byte input)
	{
  	  FtsObject.invokeCallback( target, selector, args);
	}
      };

    initialState = new State( "initial");

    State qTarget0 = new State( "Target0");
    State qTarget1 = new State( "Target1");
    State qTarget2 = new State( "Target2");
    State qTarget3 = new State( "Target3");

    State qSelectorTag = new State( "SelectorTag");
    State qSelector = new State( "Selector");

    State qArg = new State( "Arg");

    State qInt0 = new State( "Int0");
    State qInt1 = new State( "Int1");
    State qInt2 = new State( "Int2");
    State qInt3 = new State( "Int3");

    State qFloat0 = new State( "Float0");
    State qFloat1 = new State( "Float1");
    State qFloat2 = new State( "Float2");
    State qFloat3 = new State( "Float3");

    State qString = new State( "String");

    State qObject0 = new State( "Object0");
    State qObject1 = new State( "Object1");
    State qObject2 = new State( "Object2");
    State qObject3 = new State( "Object3");

    initialState.addTransition( FtsProtocol.OBJECT, qTarget0, clearAction);
    qTarget0.addTransition( qTarget1, shiftAction);
    qTarget1.addTransition( qTarget2, shiftAction);
    qTarget2.addTransition( qTarget3, shiftAction);
    qTarget3.addTransition( qSelectorTag, endTargetAction);

    qSelectorTag.addTransition( FtsProtocol.STRING, qSelector, bufferClearAction);

    qSelector.addTransition( FtsProtocol.STRING_END, qArg, endSelectorAction);
    qSelector.addTransition( qSelector, bufferShiftAction);

    qArg.addTransition( FtsProtocol.INT, qInt0, clearAction);
    qArg.addTransition( FtsProtocol.FLOAT, qFloat0, clearAction);
    qArg.addTransition( FtsProtocol.STRING, qString, bufferClearAction);
    qArg.addTransition( FtsProtocol.OBJECT, qObject0, clearAction);
    qArg.addTransition( FtsProtocol.END_OF_MESSAGE, initialState, endMessageAction);

    qInt0.addTransition( qInt1, shiftAction);
    qInt1.addTransition( qInt2, shiftAction);
    qInt2.addTransition( qInt3, shiftAction);
    qInt3.addTransition( qArg, endIntArgAction);

    qFloat0.addTransition( qFloat1, shiftAction);
    qFloat1.addTransition( qFloat2, shiftAction);
    qFloat2.addTransition( qFloat3, shiftAction);
    qFloat3.addTransition( qArg, endFloatArgAction);

    qString.addTransition( FtsProtocol.STRING_END, qArg, endStringArgAction);
    qString.addTransition( qString, bufferShiftAction);

    qObject0.addTransition( qObject1, shiftAction);
    qObject1.addTransition( qObject2, shiftAction);
    qObject2.addTransition( qObject3, shiftAction);
    qObject3.addTransition( qArg, endObjectArgAction);
  }

  FtsProtocolDecoder( FtsServer server)
  {
    this.server = server;

    buffer = new StringBuffer();
    args = new FtsArgs();

    buildStateMachine();

    currentState = initialState;
  }

  void decode( byte[] data, int offset, int length) throws FtsClientException
  {
    for ( int i = offset; i < length; i++)
      {
	currentState = currentState.next( data[i]);
	if (currentState == null)
	  throw new FtsClientException( "Invalid data in protocol : state=" + currentState + " input=" + data[i]);
      }
  }

  private FtsServer server;

  private int ival;
  private StringBuffer buffer;

  private FtsObject target;
  private String selector;
  private FtsArgs args;

  private State initialState;
  private State currentState;
}
