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
#include "Hashtable.h"

using namespace ircam::fts::client;

FtsSymbol *FtsSymbol::get( const char *s)
{
  FtsSymbol *symbol;

  _symbolTable.get( s, symbol);

  if (symbol != NULL)
    return symbol;

  symbol = new FtsSymbol( s);
  _symbolTable.put( s, symbol);
  
  return symbol;
}

std::ostream &operator<<( std::ostream &os, const FtsSymbol &s)
{
  return os << (const char *)s;
}

