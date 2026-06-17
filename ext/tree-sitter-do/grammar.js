module.exports = grammar({
  name: 'do',

  rules: {
    source_file: $ => repeat($._statement),

    _statement: $ => choice(
      $.function_declaration,
      $.let_statement,
      $.destructure_statement,
      $.if_statement,
      $.for_statement,
      $.while_statement,
      $.return_statement,
      $.expression_statement
    ),

    function_declaration: $ => seq(
      'fn',
      field('name', $.identifier),
      '(',
      optional($.parameter_list),
      ')',
      field('body', $.block)
    ),

    parameter_list: $ => sep1($.identifier, ','),

    block: $ => seq(
      '{',
      repeat($._statement),
      '}'
    ),

    let_statement: $ => seq(
      'let',
      field('name', $.identifier),
      '=',
      field('value', $._expression)
    ),

    destructure_statement: $ => seq(
      'let',
      '[',
      field('names', sep1($.identifier, ',')),
      optional(seq(
        '...',
        field('rest', $.identifier)
      )),
      ']',
      '=',
      field('value', $._expression)
    ),

    while_statement: $ => seq(
      'while',
      field('condition', $._expression),
      field('body', $.block)
    ),

    for_statement: $ => seq(
      'for',
      field('var', $.identifier),
      'in',
      field('start', $._expression),
      '..',
      field('end', $._expression),
      field('body', $.block)
    ),

    if_statement: $ => seq(
      'if',
      field('condition', $._expression),
      field('consequence', $.block),
      optional(seq('else', field('alternative', $.block)))
    ),

    return_statement: $ => seq(
      'return',
      $._expression
    ),

    expression_statement: $ => $._expression,

    _expression: $ => choice(
      $.list_comprehension,
      $.binary_expression,
      $.call_expression,
      $.identifier,
      $.number,
      $.string,
      $.array_expression
    ),

    binary_expression: $ => choice(
      prec.left(1, seq($._expression, choice('+', '-'), $._expression)),
      prec.left(2, seq($._expression, choice('*', '/'), $._expression)),
      prec.left(0, seq($._expression, choice('==', '!=', '<', '>', '<=', '>='), $._expression))
    ),

    call_expression: $ => seq(
      field('function', $.identifier),
      '(',
      optional($.argument_list),
      ')'
    ),

    argument_list: $ => sep1($._expression, ','),

    identifier: $ => /[a-zA-Z_][a-zA-Z0-9_]*/,
    number: $ => /\d+/,
    string: $ => /\"[^\"]*\"/,
    boolean: $ => choice('true', 'false'),

    array_expression: $ => seq(
      '[',
      optional(sep1($._expression, ',')),
      ']'
    ),

    list_comprehension: $ => seq(
      '[',
      'for',
      field('var', $.identifier),
      'in',
      field('start', $._expression),
      '..',
      field('end', $._expression),
      field('body', $._expression),
      optional(seq('if', field('filter', $._expression))),
      ']'
    ),

    index_expression: $ => prec(2, seq(
      field('array', $._expression),
      '[',
      field('index', $._expression),
      ']'
    )),

    slice_expression: $ => prec(2, seq(
      field('array', $._expression),
      '[',
      field('start', $._expression),
      '..',
      field('end', $._expression),
      ']'
    ))
  }
});

function sep1(rule, separator) {
  return seq(rule, repeat(seq(separator, rule)));
}