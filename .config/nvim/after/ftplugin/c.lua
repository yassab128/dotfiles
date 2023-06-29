vim.opt.equalprg = "clang-format --fno-color-diagnostics --style='{UseTab: Always, IndentWidth: 8, AlwaysBreakAfterReturnType: All, AlignAfterOpenBracket: DontAlign, IndentPPDirectives: AfterHash, BreakBeforeBraces: Linux}'"

vim.api.nvim_create_user_command('Indent',
	"silent execute 'silent! undojoin | normal mqHm`gg=G``zt`q'", {})
local c_indent = vim.api.nvim_create_augroup('vimrc', { clear = true })
vim.api.nvim_create_autocmd({'BufWritePre'}, {
	pattern = '*',
	group = c_indent,
	command = "Indent"
})
vim.api.nvim_create_autocmd({'BufWritePost'}, {
	pattern = '*',
	group = c_indent,
	command = 'call feedkeys("<CR>")'
})
-- vim.opt.makeprg = "clang-tidy 2>&-"
-- vim.opt.errorformat = "%f:%l:%c: %t: %m"
