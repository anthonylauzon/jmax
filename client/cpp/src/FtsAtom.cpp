//
// FTS client library
// Copyright (C) 2001 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// See file COPYING.LIB for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 

#include <fts/client/ftsclient.h>

namespace ircam {
namespace fts {
namespace client {

  std::ostream &operator<<( std::ostream &os, const FtsAtom &a)
  {
    if ( a.isVoid())
      os << "void";
    else if ( a.isInt())
      os << a.value.intValue;
    else if ( a.isDouble())
      os << a.value.doubleValue;
    else if ( a.isSymbol())
      os << (const char *)a.value.stringValue;
    else if ( a.isString() || a.isRawString())
      os << a.value.stringValue;
    //   else if ( a.isObject())
    //     os << a.objectValue();

    return os;
  }

}
}
}
