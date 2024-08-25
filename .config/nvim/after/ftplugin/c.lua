vim.o.equalprg = "clang-format --fno-color-diagnostics --style='{UseTab: Always, IndentWidth: 8, AlwaysBreakAfterReturnType: All, AlignAfterOpenBracket: DontAlign, IndentPPDirectives: AfterHash, BreakBeforeBraces: Linux}'"

vim.api.nvim_create_user_command('Indent',
	"silent execute 'silent! undojoin | normal mqHm`gg=G``zt`q'", {})
local cgroup = vim.api.nvim_create_augroup('vimrc', { clear = true })
vim.api.nvim_create_autocmd({'BufWritePre'}, {
	pattern = '*',
	group = cgroup,
	command = "Indent"
})
vim.api.nvim_create_autocmd({'BufWritePost'}, {
	pattern = '*',
	group = cgroup,
	command = 'call feedkeys("<CR>")'
})

-------------------------------------------------------------------------------

-- clang-check -extra-arg=-Weverything hello.c

local function mylinter()
	vim.api.nvim_out_write("Hello from Lua!\n")
	vim.print("WWWWW")
end

vim.api.nvim_create_autocmd({'BufWritePost'}, {
	group = cgroup,
	pattern = '*',
	callback = mylinter
})
