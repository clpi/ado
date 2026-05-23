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

if __name__ == '__main__':
    unittest.main()
