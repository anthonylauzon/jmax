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

helpSummary "ISPW signal summary" $dir/ispw.signal.summary.jmax

## filters (5)
helpPatch 2p2z~ $dir/2p2z~.help.jmax
helpPatch 2pole~ $dir/2pole~.help.jmax
helpPatch biquad~ $dir/biquad~.help.jmax
helpPatch wahwah~ $dir/wahwah~.help.jmax
helpPatch samphold~ $dir/samphold~.help.jmax

## trigonometrics and logarithms (10)
helpPatch sin~ $dir/trigon~.help.jmax
helpPatch cos~ $dir/trigon~.help.jmax
helpPatch tan~ $dir/trigon~.help.jmax
helpPatch asin~ $dir/trigon~.help.jmax
helpPatch acos~ $dir/trigon~.help.jmax
helpPatch atan~ $dir/trigon~.help.jmax
helpPatch arctan2~ $dir/arctan2~.help.jmax
helpPatch sinh~ $dir/trigon~.help.jmax
helpPatch cosh~ $dir/trigon~.help.jmax
helpPatch tanh~ $dir/trigon~.help.jmax

## io (3)
helpPatch adc~ $dir/adc~.help.jmax
helpPatch dac~ $dir/adc~.help.jmax
helpPatch print~ $dir/print~.help.jmax

## misc math (7)
helpPatch abs~ $dir/abs~.help.jmax
helpPatch clip~ $dir/clip~.help.jmax
helpPatch exp~ $dir/exp~.help.jmax
helpPatch log10~ $dir/exp~.help.jmax
helpPatch log~ $dir/exp~.help.jmax
helpPatch rsqrt~ $dir/sqrt~.help.jmax
helpPatch sqrt~ $dir/sqrt~.help.jmax

## signal flow (6)
helpPatch down~ $dir/down~.help.jmax
helpPatch up~ $dir/down~.help.jmax
helpPatch thrxw~ $dir/catch~.help.jmax
helpPatch cxtch~ $dir/catch~.help.jmax
helpPatch switch~ $dir/switch~.help.jmax
helpPatch thru~ $dir/thru~.help.jmax

## sampling (7)
helpPatch table~ $dir/table~.help.jmax
helpPatch samppeek~ $dir/samppeek~.help.jmax
helpPatch sampread~ $dir/sampwrite~.help.jmax
helpPatch sampwrite~ $dir/sampwrite~.help.jmax
helpPatch tabcycle~ $dir/tabcycle~.help.jmax
helpPatch tabpeek~ $dir/tabpeek~.help.jmax
helpPatch tabpoke $dir/tabpoke.help.jmax

## delay lines (3)
helpPatch delwrite~ $dir/delay~.help.jmax
helpPatch delread~ $dir/delay~.help.jmax
helpPatch vd~ $dir/delay~.help.jmax

## fourier transformation (2)
helpPatch fft~ $dir/fft~.help.jmax
helpPatch ifft~ $dir/fft~.help.jmax

## generators (5)
helpPatch sig~ $dir/sig~.help.jmax
helpPatch line~ $dir/line~.help.jmax
helpPatch phasor~ $dir/phasor~.help.jmax
helpPatch osc1~ $dir/osc1~.help.jmax
helpPatch tab1~ $dir/osc1~.help.jmax
helpPatch noise~ $dir/noise~.help.jmax

## signal peep (7)
helpPatch pitch~ $dir/pitch~.help.jmax
helpPatch pt~ $dir/pt~.help.jmax
helpPatch snapshot~ $dir/snapshot~.help.jmax
helpPatch threshold~ $dir/threshold~.help.jmax
helpPatch zerocross~ $dir/zerocross~.help.jmax
helpPatch samplerate~ $dir/samplerate~.help.jmax
helpPatch vectorsize~ $dir/samplerate~.help.jmax
