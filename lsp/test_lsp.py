import unittest
import sys
import os

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

try:
    from lsp import do_lsp
except ImportError:
    pass

class TestLSP(unittest.TestCase):
    def test_parse(self):
        # We can't easily test without mock, but just creating the instance works as a smoke test
        server = do_lsp.AdoLSP()
        server.parse_symbols("file:///test.do", "fn main() {\n  let a = 1\n}")
        self.assertIn("main", server.symbols)
        self.assertIn("a", server.symbols)

    def test_missing_let_assignment(self):
        server = do_lsp.AdoLSP()
        server.parse_symbols("file:///test.do", "fn main() {\n  let a\n}")
        diags = server.get_diagnostics("file:///test.do", "fn main() {\n  let a\n}")
        self.assertTrue(any(d['message'] == 'let statement requires assignment' for d in diags))

    def test_undefined_function(self):
        server = do_lsp.AdoLSP()
        server.parse_symbols("file:///test.do", "fn main() {\n  foo()\n}")
        diags = server.get_diagnostics("file:///test.do", "fn main() {\n  foo()\n}")
        self.assertTrue(any('Undefined function: foo' in d['message'] for d in diags))

    def test_semantic_tokens(self):
        server = do_lsp.AdoLSP()
        server.docs["file:///test.do"] = "fn main() {\n  let a = 1\n}"
        server.parse_symbols("file:///test.do", server.docs["file:///test.do"])
        msg = {'params': {'textDocument': {'uri': 'file:///test.do'}}}
        result = server.handle_semantic_tokens_full(msg)
        self.assertIn('data', result)
        self.assertTrue(len(result['data']) > 0)

    def test_inlay_hints(self):
        server = do_lsp.AdoLSP()
        text = "fn add(a, b) {}\nfn main() {\n  add(1, 2)\n}"
        server.docs["file:///test.do"] = text
        server.parse_symbols("file:///test.do", text)
        msg = {'params': {'textDocument': {'uri': 'file:///test.do'}, 'range': {'start': {'line': 0}, 'end': {'line': 3}}}}
        hints = server.handle_inlay_hint(msg)
        self.assertTrue(any('a:' in h['label'] for h in hints))

    def test_document_highlight(self):
        server = do_lsp.AdoLSP()
        text = "fn main() {\n  let a = 1\n  print(a)\n}"
        server.docs["file:///test.do"] = text
        server.parse_symbols("file:///test.do", text)
        msg = {'params': {'textDocument': {'uri': 'file:///test.do'}, 'position': {'line': 1, 'character': 7}}}
        highlights = server.handle_document_highlight(msg)
        self.assertEqual(len(highlights), 2)

    def test_code_action(self):
        server = do_lsp.AdoLSP()
        diag = {'message': 'let statement requires assignment', 'range': {'end': {'line': 1, 'character': 7}}}
        msg = {'params': {'textDocument': {'uri': 'file:///test.do'}, 'context': {'diagnostics': [diag]}}}
        actions = server.handle_code_action(msg)
        self.assertTrue(any(a['title'] == 'Initialize variable' for a in actions))

    def test_folding_range(self):
        server = do_lsp.AdoLSP()
        text = "fn main() {\n  let a = 1\n}"
        server.docs["file:///test.do"] = text
        msg = {'params': {'textDocument': {'uri': 'file:///test.do'}}}
        folds = server.handle_folding_range(msg)
        self.assertEqual(len(folds), 1)
        self.assertEqual(folds[0]['startLine'], 0)
        self.assertEqual(folds[0]['endLine'], 2)

    def test_call_hierarchy(self):
        server = do_lsp.AdoLSP()
        text = "fn foo() {}\nfn main() {\n  foo()\n}"
        server.docs["file:///test.do"] = text
        server.parse_symbols("file:///test.do", text)

        # Prepare
        msg_prep = {'params': {'textDocument': {'uri': 'file:///test.do'}, 'position': {'line': 0, 'character': 4}}}
        prep = server.handle_prepare_call_hierarchy(msg_prep)
        self.assertTrue(len(prep) > 0)

        item = prep[0]

        # Incoming
        msg_in = {'params': {'item': item}}
        incoming = server.handle_call_hierarchy_incoming(msg_in)
        self.assertTrue(any(i['from']['name'] == 'main' for i in incoming))

        # Outgoing
        msg_out = {'params': {'item': prep[0]}}
        msg_out['params']['item'] = server.handle_prepare_call_hierarchy({'params': {'textDocument': {'uri': 'file:///test.do'}, 'position': {'line': 1, 'character': 4}}})[0]
        outgoing = server.handle_call_hierarchy_outgoing(msg_out)
        self.assertTrue(any(o['to']['name'] == 'foo' for o in outgoing))

if __name__ == '__main__':
    unittest.main()
