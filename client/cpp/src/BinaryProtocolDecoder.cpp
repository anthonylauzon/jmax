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

#include <fts/ftsclient.h>

#include "BinaryProtocol.h"
#include "BinaryProtocolDecoder.h"

using namespace ircam::fts::client;

#define qInitial 1

#define qInt0 10
#define qInt1 11
#define qInt2 12
#define qInt3 13

#define qFloat0 20
#define qFloat1 21
#define qFloat2 22
#define qFloat3 23
#define qFloat4 24
#define qFloat5 25
#define qFloat6 26
#define qFloat7 27

#define qString 30

#define qObject0 40
#define qObject1 41
#define qObject2 42
#define qObject3 43

#define qSymbolIndex0 50
#define qSymbolIndex1 51
#define qSymbolIndex2 52
#define qSymbolIndex3 53

#define qSymbolCache0 60
#define qSymbolCache1 61
#define qSymbolCache2 62
#define qSymbolCache3 63
#define qSymbolCache4 64

void BinaryProtocolDecoder::clearAction( int input)
{
  _lval = 0;
}

void BinaryProtocolDecoder::shiftAction( int input)
{
  _lval = _lval << 8 | input;
}

void BinaryProtocolDecoder::shiftLongAction( int input)
{
  _lval = _lval << 8 | input;
}

void BinaryProtocolDecoder::bufferClearAction( int input)
{
  _buffer->clear();
}

void BinaryProtocolDecoder::clearAllAction( int input)
{
  _lval = 0;
  _buffer->clear();
}

void BinaryProtocolDecoder::bufferShiftAction( int input)
{
  _buffer->append( (char)input);
}

void BinaryProtocolDecoder::endIntAction( int input)
{
  _lval = _lval << 8 | input;

  if (_argsCount >= 2)
    _args->addInt( (int)_lval);

  _argsCount++;
}

void BinaryProtocolDecoder::endFloatAction( int input)
{
  _lval = _lval << 8 | input;

  // FIXME
  if (_argsCount >= 2)
    _args->addDouble( 3.14);

  _argsCount++;
}

void BinaryProtocolDecoder::endSymbolIndexAction( int input)
{
  _lval = _lval << 8 | input;

  const FtsSymbol *s = _symbolCache->get( (int)_lval);

  if (_argsCount == 1)
    _selector = s;
  else
    _args->addSymbol( s);

  _argsCount++;
}

void BinaryProtocolDecoder::endSymbolCacheAction( int input)
{
  const FtsSymbol *s = FtsSymbol::get( (char *)_buffer->getBytes());

  _symbolCache->put( (int)_lval, s);

  if (_argsCount == 1)
    _selector = s;
  else
    _args->addSymbol( s);

  _argsCount++;
}

void BinaryProtocolDecoder::endStringAction( int input)
{
  if (_argsCount >= 2)
    _args->addString( (char *)_buffer->getBytes());

  _argsCount++;
}

void BinaryProtocolDecoder::endObjectAction( int input)
{
  _lval = _lval << 8 | input;

  // FIXME
  // FtsObject *obj = server.getObject( (int)_lval);
  FtsObject *obj = NULL;

  if (_argsCount == 0)
    _target = obj;
  else
    _args->addObject( obj);

  _argsCount++;
}

void BinaryProtocolDecoder::endMessageAction( int input)
{
  //  FtsObject.invokeMessageHandler( target, selector, _args);
  _args->clear();
  _argsCount = 0;
}

void BinaryProtocolDecoder::nextState( int input)
{
  switch( _currentState) {
  case 0:
    /* try to skip till end of message */
    if ( input == BinaryProtocol::END_OF_MESSAGE)
      _currentState = qInitial;
    break;
  case qInitial:
    if ( input == BinaryProtocol::INT)
      {
	_currentState = qInt0;
	clearAction( input);
      }
    else if ( input == BinaryProtocol::FLOAT)
      {
	_currentState = qFloat0;
	clearAction( input);
      }
    else if ( input == BinaryProtocol::SYMBOL_INDEX)
      {
	_currentState = qSymbolIndex0;
	clearAction( input);
      }
    else if ( input == BinaryProtocol::SYMBOL_CACHE)
      {
	_currentState = qSymbolCache0;
	clearAllAction( input);
      }
    else if ( input == BinaryProtocol::STRING)
      {
	_currentState = qString;
	bufferClearAction( input);
      }
    else if ( input == BinaryProtocol::OBJECT)
      {
	_currentState = qObject0;
	clearAction( input);
      }
    else if ( input == BinaryProtocol::END_OF_MESSAGE)
      endMessageAction( input);
    else
      _currentState = 0;
    break;
  case qInt0:
    _currentState = qInt1;
    shiftAction( input);
    break;
  case qInt1:
    _currentState = qInt2;
    shiftAction( input);
    break;
  case qInt2:
    _currentState = qInt3;
    shiftAction( input);
    break;
  case qInt3:
    _currentState = qInitial;
    endIntAction( input);
    break;
  case qFloat0:
    _currentState = qFloat1;
    shiftAction( input);
    break;
  case qFloat1:
    _currentState = qFloat2;
    shiftAction( input);
    break;
  case qFloat2:
    _currentState = qFloat3;
    shiftAction( input);
    break;
  case qFloat3:
    _currentState = qFloat4;
    shiftAction( input);
    break;
  case qFloat4:
    _currentState = qFloat5;
    shiftAction( input);
    break;
  case qFloat5:
    _currentState = qFloat6;
    shiftAction( input);
    break;
  case qFloat6:
    _currentState = qFloat7;
    shiftAction( input);
    break;
  case qFloat7:
    _currentState = qInitial;
    endFloatAction( input);
    break;
  case qSymbolIndex0:
    _currentState = qSymbolIndex1;
    shiftAction( input);
    break;
  case qSymbolIndex1:
    _currentState = qSymbolIndex2;
    shiftAction( input);
    break;
  case qSymbolIndex2:
    _currentState = qSymbolIndex3;
    shiftAction( input);
    break;
  case qSymbolIndex3:
    _currentState = qInitial;
    endSymbolIndexAction( input);
    break;
  case qSymbolCache0:
    _currentState = qSymbolCache1;
    shiftAction( input);
    break;
  case qSymbolCache1:
    _currentState = qSymbolCache2;
    shiftAction( input);
    break;
  case qSymbolCache2:
    _currentState = qSymbolCache3;
    shiftAction( input);
    break;
  case qSymbolCache3:
    _currentState = qSymbolCache4;
    shiftAction( input);
    break;
  case qSymbolCache4:
    if ( input == 0)
      {
	_currentState = qInitial;
	endSymbolCacheAction( input);
      }
    else
      bufferShiftAction( input);
    break;
  case qString:
    if ( input == 0)
      {
	_currentState = qInitial;
	endStringAction( input);
      }
    else
      bufferShiftAction( input);
    break;
  case qObject0:
    _currentState = qObject1;
    shiftAction( input);
    break;
  case qObject1:
    _currentState = qObject2;
    shiftAction( input);
    break;
  case qObject2:
    _currentState = qObject3;
    shiftAction( input);
    break;
  case qObject3:
    _currentState = qInitial;
    endObjectAction( input);
    break;
  }
}

BinaryProtocolDecoder::BinaryProtocolDecoder( FtsServer *server)
{
  _server = server;
  //  _connection = server.getConnection();

  _buffer = new Buffer();
  _args = new FtsArgs();
  _argsCount = 0;

  _symbolCache = new SymbolCache();

  _currentState = qInitial;
}

void BinaryProtocolDecoder::decode( const unsigned char *data, int length) throw( FtsClientException)
{
  for ( int i = 0; i < length; i++)
    {
      int c = data[i];

      if ( c < 0)
	c += 256;

      nextState( c);

      if (_currentState == 0)
	throw FtsClientException( "Invalid data in protocol");
    }
}

