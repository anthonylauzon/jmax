# 
# jMax
# Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
# 
# See file LICENSE for further informations on licensing terms.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
# 
# Based on Max/ISPW by Miller Puckette.
#
# Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
#

helpSummary "ISPW control summary" $dir/ispw.control.summary.jmax

## misc math (10)
helpPatch abs $dir/abs.help.jmax
helpPatch accum $dir/accum.help.jmax
helpPatch clip $dir/clip.help.jmax
helpPatch expr $dir/expr.help.jmax
helpPatch exscale $dir/exscale.help.jmax
helpPatch ftom $dir/ftom.help.jmax
helpPatch linedrive $dir/linedrive.help.jmax
helpPatch logscale $dir/exscale.help.jmax
helpPatch mtof $dir/mtof.help.jmax
helpPatch scale $dir/scale.help.jmax

## data (7)
helpPatch bag $dir/bag.help.jmax
helpPatch float $dir/float.help.jmax
helpPatch funbuff $dir/funbuff.help.jmax
helpPatch int $dir/int.help.jmax
helpPatch pbank $dir/pbank.help.jmax
helpPatch table $dir/table.help.jmax
helpPatch v $dir/value.help.jmax

## lists (3)
helpPatch pack $dir/pack.help.jmax
helpPatch prepend $dir/prepend.help.jmax
helpPatch unpack $dir/unpack.help.jmax

## timing (9)
helpPatch clock $dir/clock.help.jmax
helpPatch delay $dir/delay.help.jmax
helpPatch metro $dir/metro.help.jmax
helpPatch pipe $dir/pipe.help.jmax
helpPatch speedlim $dir/speedlim.help.jmax
helpPatch time $dir/time.help.jmax
helpPatch timer $dir/timer.help.jmax

## flow control (11)
helpPatch bangbang $dir/bangbang.help.jmax
helpPatch change $dir/change.help.jmax
helpPatch gate $dir/gate.help.jmax
helpPatch loadbang $dir/loadbang.help.jmax
helpPatch route $dir/route.help.jmax
helpPatch select $dir/select.help.jmax
helpPatch split $dir/split.help.jmax
helpPatch swap $dir/swap.help.jmax
helpPatch send $dir/send.help.jmax
helpPatch receive $dir/receive.help.jmax
helpPatch trigger $dir/trigger.help.jmax
#alias
helpPatch s $dir/send.help.jmax
helpPatch r $dir/receive.help.jmax
helpPatch t $dir/trigger.help.jmax

## genarators (6)
helpPatch line $dir/line.help.jmax
helpPatch drunk $dir/drunk.help.jmax
helpPatch random $dir/random.help.jmax

## midi (4)
helpPatch Rsend $dir/Rsend.help.jmax
helpPatch Rreceive $dir/Rsend.help.jmax
helpPatch makenote $dir/makenote.help.jmax
helpPatch stripnote $dir/stripnote.help.jmax

# polyphonic (2)
helpPatch poly $dir/poly.help.jmax
helpPatch voxalloc $dir/voxalloc.help.jmax
