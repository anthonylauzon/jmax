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
  
  unsigned int getNextPrime( unsigned int n)
  {
    static const unsigned int primesSuite[] = {
      7,
      17,
      31,
      67,
      127,
      257,
      521,
      1031,
      2053,
      4099,
      8191,
      16411,
      32771,
      65537,
      131071,
      262147,
      524287,
      1048583,
      2097169,
      4194319,
      8388617,
      16777259,
      33554467,
    };
    unsigned int i;

    for ( i = 0; i < sizeof (primesSuite) / sizeof (unsigned int); i++)
      if (n < primesSuite[i])
	return primesSuite[i];

    return primesSuite[i-1];
  }

};
};
};

