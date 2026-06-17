; Helix injections for Ado language
; Nothing special needed right now

; Helix locals for Ado language
; This helps with variable renaming and references

(let_statement name: (identifier) @local)
(destructure_statement (identifier) @local)
(for_statement var: (identifier) @local)