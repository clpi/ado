; Helix queries for Ado language

; Keywords
( "fn" ) @keyword.function
( "let" ) @keyword
( "if" ) @keyword.control
( "else" ) @keyword.control
( "return" ) @keyword.control
( "for" ) @keyword.control
( "while" ) @keyword.control
( "in" ) @keyword.control.conditional

; Functions
(function_declaration name: (identifier) @function)
(call_expression function: (identifier) @function.call)

; Parameters
(parameter_list (identifier) @variable.parameter)

; Variables
(let_statement name: (identifier) @variable)
(destructure_statement (identifier) @variable)

; Operators
["+" "-" "*" "/" "==" "!=" "<" ">" "<=" ">="] @operator
"=" @operator
".." @operator

; Literals
(number) @number
(string) @string

; Punctuation
["(" ")" "{" "}" "[" "]"] @punctuation.bracket
"," @punctuation.delimiter
"..." @punctuation.special

; Special syntax
(slice_expression ".." @operator)