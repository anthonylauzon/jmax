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

class SymbolCache {
  SymbolCache( int initialCapacity)
  {
    cache = new String[initialCapacity];
  }

  SymbolCache()
  {
    this( 512);
  }

  final String put( int index, String s)
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

    return cache[index];
  }

  String[] cache;
}

class State {
  class Transition {
    Transition( byte input, State targetState, TransitionAction action, Transition next)
    {
      this.input = input; 
      this.targetState = targetState;
      this.action = action;
      this.next = next;
    }

    byte input;
    State targetState;
    TransitionAction action;
    Transition next;
  }

  State( String name)
  {
    this.name = name;
  }

  void addTransition( byte input, State targetState, TransitionAction action)
  {
    transitions = new Transition( input, targetState, action, transitions);
  }

  void addTransition( State targetState, TransitionAction action)
  {
    defaultTransition = new Transition( (byte)0, targetState, action, null);
  }

  final State next( byte input)
  {
    Transition transition = transitions;

    while (transition != null)
      {
	if (transition.input == input)
	  {
	    transition.action.fire( input);
	    return transition.targetState;
	  }

	transition = transition.next;
      }

    if (defaultTransition != null)
      {
	defaultTransition.action.fire( input);
	return defaultTransition.targetState;
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

  private State buildStateMachine()
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

    TransitionAction endIntAction = new TransitionAction() {
	public void fire( byte input)
	{
	  ival = ival << 8 | input;
	  if (argsCount >= 2)
	    args.add( ival);
	  argsCount++;
	}
      };

    TransitionAction endFloatAction = new TransitionAction() {
	public void fire( byte input)
	{
	  ival = ival << 8 | input;
	  if (argsCount >= 2)
	    args.add( Float.intBitsToFloat(ival) );
	  argsCount++;
	}
      };

    TransitionAction endStringAction = new TransitionAction() {
	public void fire( byte input)
	{
	  if (argsCount >= 2)
	    args.add( buffer.toString());
	  argsCount++;
	}
      };

    TransitionAction endSymbolIndexAction = new TransitionAction() {
	public void fire( byte input)
	{
	  ival = ival << 8 | input;
	  String s = symbolCache.cache[ival];

	  if (argsCount == 1)
	    selector = s;
	  else
	    args.add( s);

	  argsCount++;
	}
      };

    TransitionAction endSymbolCacheAction = new TransitionAction() {
	public void fire( byte input)
	{
	  String s = symbolCache.put( ival, buffer.toString());

	  if (argsCount == 1)
	    selector = s;
	  else
	    args.add( s);

	  argsCount++;
	}
      };

    TransitionAction endObjectAction = new TransitionAction() {
	public void fire( byte input)
	{
	  ival = ival << 8 | input;

	  FtsObject obj = server.getObject( ival);

	  if (argsCount == 0)
	    args.add( obj);
	  else
	    target = obj;

	  argsCount++;
	}
      };

    TransitionAction endMessageAction = new TransitionAction() {
	public void fire( byte input)
	{
  	  FtsObject.invokeCallback( target, selector, args);
	  args.clear();
	  argsCount = 0;
	}
      };

    State qStart = new State( "Start");

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

    State qSymbolIndex0 = new State( "SymbolIndex0");
    State qSymbolIndex1 = new State( "SymbolIndex1");
    State qSymbolIndex2 = new State( "SymbolIndex2");
    State qSymbolIndex3 = new State( "SymbolIndex3");

    State qSymbolCache0 = new State( "SymbolCache0");
    State qSymbolCache1 = new State( "SymbolCache1");
    State qSymbolCache2 = new State( "SymbolCache2");
    State qSymbolCache3 = new State( "SymbolCache3");
    State qSymbolCache4 = new State( "SymbolCache4");

    qStart.addTransition( FtsProtocol.INT, qInt0, clearAction);
    qStart.addTransition( FtsProtocol.FLOAT, qFloat0, clearAction);
    qStart.addTransition( FtsProtocol.SYMBOL_INDEX, qSymbolIndex0, clearAction);
    qStart.addTransition( FtsProtocol.SYMBOL_CACHE, qSymbolCache0, clearAction);
    qStart.addTransition( FtsProtocol.STRING, qString, bufferClearAction);
    qStart.addTransition( FtsProtocol.OBJECT, qObject0, clearAction);
    qStart.addTransition( FtsProtocol.END_OF_MESSAGE, qStart, endMessageAction);

    qInt0.addTransition( qInt1, shiftAction);
    qInt1.addTransition( qInt2, shiftAction);
    qInt2.addTransition( qInt3, shiftAction);
    qInt3.addTransition( qStart, endIntAction);

    qFloat0.addTransition( qFloat1, shiftAction);
    qFloat1.addTransition( qFloat2, shiftAction);
    qFloat2.addTransition( qFloat3, shiftAction);
    qFloat3.addTransition( qStart, endFloatAction);

    qSymbolIndex0.addTransition( qSymbolIndex1, shiftAction);
    qSymbolIndex1.addTransition( qSymbolIndex2, shiftAction);
    qSymbolIndex2.addTransition( qSymbolIndex3, shiftAction);
    qSymbolIndex3.addTransition( qStart, endSymbolIndexAction);

    qSymbolCache0.addTransition( qSymbolCache1, shiftAction);
    qSymbolCache1.addTransition( qSymbolCache2, shiftAction);
    qSymbolCache2.addTransition( qSymbolCache3, shiftAction);
    qSymbolCache3.addTransition( qSymbolCache4, shiftAction);
    qSymbolCache4.addTransition( FtsProtocol.SYMBOL_CACHE_END, qStart, endSymbolCacheAction);
    qSymbolCache4.addTransition( qSymbolCache4, bufferShiftAction);

    qString.addTransition( FtsProtocol.STRING_END, qStart, endStringAction);
    qString.addTransition( qString, bufferShiftAction);

    qObject0.addTransition( qObject1, shiftAction);
    qObject1.addTransition( qObject2, shiftAction);
    qObject2.addTransition( qObject3, shiftAction);
    qObject3.addTransition( qStart, endObjectAction);

    return qStart;
  }

  FtsProtocolDecoder( FtsServer server)
  {
    this.server = server;

    buffer = new StringBuffer();
    args = new FtsArgs();
    argsCount = 0;

    symbolCache = new SymbolCache();

    currentState = buildStateMachine();
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
  private int argsCount;

  private State currentState;

  private SymbolCache symbolCache;
}
