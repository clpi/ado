" Ado file type plugin - Enhanced
setlocal commentstring=#\ %s
setlocal formatoptions+=croql
setlocal tabstop=2 shiftwidth=2 expandtab
setlocal matchpairs+=(:)
setlocal suffixesadd=.do

" Ado-specific abbreviations
iabbrev <buffer> fn fn 
iabbrev <buffer> let let 

" Snippets for Ado language features
inoremap <buffer> <silent> [[for  [for  in .. ]<Left><Left><Left><Left><Left><Left><Left><Left><Left><Left><Left><Left>

" Code lens toggle
nnoremap <buffer> <leader>cl :lua print("Code lens not available in plain Vim")<CR>

" Auto-format on save (optional)
" autocmd BufWritePre <buffer> lua vim.lsp.buf.format()