/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * See file COPYING.LIB for further informations on licensing terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#ifndef DATA_PACKAGE_FLOAT_FUNCTIONS_H
#define DATA_PACKAGE_FLOAT_FUNCTIONS_H

#ifndef HAVE_SINF
float sinf(float f)
{
  return (float)sin(f);
}
#endif

#ifndef HAVE_COSF
float cosf(float f)
{
  return (float)cos(f);
}
#endif

#ifndef HAVE_ATAN2F
float atan2f(float x, float y)
{
  return (float)atan2(x,y);
}
#endif

#ifndef HAVE_SQRTF
float sqrtf(float f)
{
  return (float)sqrt(f);
}
#endif

#ifndef HAVE_FABSF
float fabsf(float f)
{
  return (float)fabs(f);
}
#endif

#ifndef HAVE_LOGF
float logf(float f)
{
  return (float)log(f);
}
#endif

#ifndef HAVE_EXPF
float expf(float f)
{
  return (float)exp(f);
}
#endif


#endif