vim.opt.equalprg = "clang-format --fno-color-diagnostics --style='{UseTab: Always, IndentWidth: 8, AlwaysBreakAfterReturnType: All, AlignAfterOpenBracket: DontAlign, IndentPPDirectives: AfterHash, BreakBeforeBraces: Linux}'"

local c_indent = vim.api.nvim_create_augroup('vimrc', { clear = true })
vim.api.nvim_create_autocmd({'BufWritePre'}, {
	pattern = '*',
	group = c_indent,
	command = "normal 1G=G"
})
