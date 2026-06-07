; Helix indents for Ado language

( if_statement "if" @conditional
  (#eq? @conditional "if") )

( while_statement "while" @loop
  (#eq? @loop "while") )

( for_statement "for" @loop
  (#eq? @loop "for") )

( function_declaration "fn" @function
  (#eq? @function "fn") )

(block "{" @start "}" @end) @indent