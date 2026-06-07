" Ado Language Syntax - Enhanced
if exists("b:current_syntax")
    finish
endif

" Keywords
syn keyword adoKeyword fn let if else while for return in true false and or not print len push

" Builtins
syn keyword adoBuiltin abs min max pow clamp sign is_even is_odd gcd lcm factorial fib sum avg take drop concat fill

" Numbers
syn match adoNumber /\d\+/

" Operators
syn match adoOperator /[-+*/%<>=!]=\|[<>]\|[-+*/]/

" Special operators - slice (..) and spread (...)
syn match adoOperator /\.\./
syn match adoOperator /\.\.\./

" Strings
syn region adoString start=/"/ skip=/\\"/ end=/"/

" Comments
syn region adoComment start=/#/ end=/$/

" List comprehensions - highlight the [for ... ] pattern
syn region adoListComp start=/\[\s*for/ end=/]/ contains=adoKeyword,adoIdentifier,adoNumber,adoOperator

" Function calls
syn match adoFuncCall /\w\+\ze\s*(/

" Identifiers
syn match adoIdentifier /\w\+/

" Highlight links
hi def link adoKeyword Keyword
hi def link adoBuiltin Function
hi def link adoNumber Number
hi def link adoString String
hi def link adoComment Comment
hi def link adoOperator Operator
hi def link adoFuncCall Function
hi def link adoIdentifier Identifier

let b:current_syntax = "ado"