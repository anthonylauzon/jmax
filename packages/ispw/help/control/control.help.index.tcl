##
## help declarations for the ispw control classes
##

## logic operators (4)
helpPatch | $dir/logic.help.jmax
helpPatch || $dir/logic.help.jmax
helpPatch && $dir/logic.help.jmax
helpPatch & $dir/logic.help.jmax

## comparison operators (6)
helpPatch <= $dir/compare.help.jmax
helpPatch == $dir/compare.help.jmax
helpPatch > $dir/compare.help.jmax
helpPatch >= $dir/compare.help.jmax
helpPatch != $dir/compare.help.jmax
helpPatch < $dir/compare.help.jmax

## arithmetics (6)
helpPatch % $dir/arith.help.jmax
helpPatch * $dir/arith.help.jmax
helpPatch + $dir/arith.help.jmax
helpPatch - $dir/arith.help.jmax
helpPatch inv* $dir/arith.help.jmax
helpPatch inv+ $dir/arith.help.jmax

##shifting operators (2)
helpPatch << $dir/shift.help.jmax
helpPatch >> $dir/shift.help.jmax

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
helpPatch value $dir/value.help.jmax

## lists (3)
helpPatch pack $dir/pack.help.jmax
helpPatch prepend $dir/prepend.help.jmax
helpPatch unpack $dir/unpack.help.jmax

## timing (9)
helpPatch at $dir/at.help.jmax
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
helpPatch bpe $dir/bpe.help.jmax
helpPatch line $dir/line.help.jmax
helpPatch drunk $dir/drunk.help.jmax
helpPatch random $dir/random.help.jmax

## midi support (19)
helpPatch Rsend $dir/Rsend.help.jmax
helpPatch Rreceive $dir/Rsend.help.jmax
helpPatch bendin $dir/bendin.help.jmax
helpPatch bendout $dir/bendout.help.jmax
helpPatch ctlin $dir/ctlin.help.jmax
helpPatch ctlout $dir/ctlout.help.jmax
helpPatch makenote $dir/makenote.help.jmax
helpPatch midiformat $dir/midiformat.help.jmax
helpPatch midiin $dir/midiin.help.jmax
helpPatch midiout $dir/midiout.help.jmax
helpPatch midiparse $dir/midiparse.help.jmax
helpPatch notein $dir/notein.help.jmax
helpPatch noteout $dir/noteout.help.jmax
helpPatch pgmin $dir/pgmin.help.jmax
helpPatch pgmout $dir/pgmout.help.jmax
helpPatch stripnote $dir/stripnote.help.jmax
helpPatch sysexin $dir/sysexin.help.jmax
helpPatch touchin $dir/touchin.help.jmax
helpPatch touchout $dir/touchout.help.jmax

# polyphonic (2)
helpPatch poly $dir/poly.help.jmax
helpPatch voxalloc $dir/voxalloc.help.jmax

## print (1)
helpPatch print $dir/print.help.jmax

## Summary
helpSummary "ISPW control" $dir/ispw.control.summary.jmax
