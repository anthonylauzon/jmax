helpSummary "Signal" $dir/signal.summary.jmax
dataDirectory $dir

## generators
helpPatch osc~ $dir/osc.help.jmax
helpPatch wave~ $dir/wave.help.jmax
helpPatch white~ $dir/white.help.jmax
helpPatch pink~ $dir/pink.help.jmax
helpPatch ~ $dir/tilda.help.jmax
helpPatch bpf~ $dir/bpf.help.jmax

## binops
helpPatch *~ $dir/binop.help.jmax
helpPatch +~ $dir/binop.help.jmax
helpPatch -~ $dir/binop.help.jmax
helpPatch /~ $dir/binop.help.jmax
helpPatch /*~ $dir/binop.help.jmax
helpPatch -+~ $dir/binop.help.jmax
