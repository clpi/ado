(function () {
  function highlightPage() {
    if (!window.hljs) return;

    if (window.hljs.highlightAll) {
      window.hljs.highlightAll();
      return;
    }

    document.querySelectorAll('pre code').forEach(function (block) {
      window.hljs.highlightElement(block);
    });
  }

  function registerAdoHighlighter() {
    if (!window.hljs) return;

    window.hljs.registerLanguage('ado', function (hljs) {
      var ident = '[A-Za-z_][A-Za-z0-9_]*';

      return {
        name: 'Ado',
        aliases: ['do'],
        case_insensitive: false,
        keywords: {
          keyword: 'fn let if else unless while until for in forever return break continue match enum defer guard assert swap try rescue raise once maybe and or not',
          literal: 'true false',
          built_in: 'print trace len push abs min max clamp pow sign is_even is_odd gcd lcm factorial fib sum avg contains count_if find all any pop reverse remove insert take drop concat fill filter sort unique reflect capacity reserve shrink_to_fit http_get http_post http_put http_delete http_status getenv exit read_file write_file file_exists sleep time random'
        },
        contains: [
          hljs.COMMENT('#', '$'),
          {
            className: 'string',
            begin: '"',
            end: '"',
            contains: [hljs.BACKSLASH_ESCAPE]
          },
          {
            className: 'number',
            begin: '\\b(0[xX][0-9A-Fa-f]+|0[bB][01]+|\\d+)\\b'
          },
          {
            className: 'operator',
            begin: '=>|\\|>|\\.\\.\\.|\\.\\.|==|!=|<=|>=|[+\\-*/%<>=!?:]'
          },
          {
            className: 'function',
            beginKeywords: 'fn',
            end: /[{;]/,
            excludeEnd: true,
            contains: [
              {
                className: 'title',
                begin: ident,
                relevance: 0
              },
              {
                className: 'params',
                begin: /\(/,
                end: /\)/,
                excludeBegin: true,
                excludeEnd: true,
                contains: [
                  {
                    className: 'variable',
                    begin: ident
                  }
                ]
              }
            ]
          },
          {
            className: 'variable',
            begin: '\\blet\\s+',
            end: /\s|=|:/,
            excludeBegin: true,
            excludeEnd: true,
            keywords: 'let',
            contains: [
              {
                className: 'variable',
                begin: ident
              }
            ]
          },
          {
            className: 'function.call',
            begin: ident + '(?=\\s*\\()'
          },
          {
            className: 'variable',
            begin: '\\b' + ident + '\\b'
          }
        ]
      };
    });

    if (window.hljs.registerAliases) {
      window.hljs.registerAliases('do', { languageName: 'ado' });
    }

    highlightPage();

    if (window.MutationObserver) {
      var timer = null;
      var observer = new window.MutationObserver(function () {
        window.clearTimeout(timer);
        timer = window.setTimeout(highlightPage, 0);
      });

      observer.observe(document.documentElement, {
        childList: true,
        subtree: true
      });
    }
  }

  if (document.readyState === 'loading') {
    document.addEventListener('DOMContentLoaded', registerAdoHighlighter);
  } else {
    registerAdoHighlighter();
  }
})();
