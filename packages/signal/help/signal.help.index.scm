(help-summary "Signal summary" (file-cat dir "signal.summary.jmax"))
(data-directory dir)

;; generators
(help-patch "osc~" (file-cat dir "osc.help.jmax"))
(help-patch "wave~" (file-cat dir "wave.help.jmax"))
(help-patch "white~" (file-cat dir "white.help.jmax"))
(help-patch "pink~" (file-cat dir "pink.help.jmax"))
(help-patch "~" (file-cat dir "tilda.help.jmax"))
(help-patch "bpf~" (file-cat dir "bpf.help.jmax"))

;; binops
(help-patch "*~" (file-cat dir "binop.help.jmax"))
(help-patch "+~" (file-cat dir "binop.help.jmax"))
(help-patch "-~" (file-cat dir "binop.help.jmax"))
(help-patch "/~" (file-cat dir "binop.help.jmax"))
(help-patch "/*~" (file-cat dir "binop.help.jmax"))
(help-patch "-+~" (file-cat dir "binop.help.jmax"))
