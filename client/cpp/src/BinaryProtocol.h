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

namespace ircam {
namespace fts {
namespace client {

  class BinaryProtocol {
  public:
    static const unsigned char INT               = (unsigned char)0x01;
    static const unsigned char FLOAT             = (unsigned char)0x02;
    static const unsigned char SYMBOL_INDEX      = (unsigned char)0x03;
    static const unsigned char SYMBOL_CACHE      = (unsigned char)0x04;
    static const unsigned char STRING            = (unsigned char)0x05;
    static const unsigned char OBJECT            = (unsigned char)0x06;
    static const unsigned char RAW_STRING        = (unsigned char)0x07;
    static const unsigned char END_OF_MESSAGE    = (unsigned char)0x0F;
  };

};
};
};

