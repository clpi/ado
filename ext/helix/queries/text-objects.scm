; Helix text objects for Ado language

; Function text object
( function_declaration
  name: (identifier) @function.around )

; Block text object
( block "{" @block.start "}" @block.end )

; Array literal text object
( array_expression "[" @array.start "]" @array.end )

; List comprehension text object
( list_comprehension "[" @listcomp.start "]" @listcomp.end )

; Slice expression
( slice_expression 
  array: _ @slice.target
  start: _ @slice.start
  end: _ @slice.end )

; Index expression
( index_expression
  array: _ @index.target
  index: _ @index.index )