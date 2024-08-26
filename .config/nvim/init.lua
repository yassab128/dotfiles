-- function my_func(x)
-- 	return #x
-- end

-- vim.cmd("runtime vimrc")

-- vim.opt = {
-- 	tabstop = 8
-- 	shiftwidth = 8
-- }

require('mystatusline')
require('mynetrw')

vim.api.nvim_set_hl(0, 'WinBar', {bg = 'White', fg = 'Black'})

-- Remember that vim.o > vim.opt
vim.o.tabstop = 8
-- vim.o.hidden = true
vim.o.shiftwidth = 8
vim.o.number = true
vim.o.clipboard = "unnamedplus"
vim.o.ignorecase = true
vim.o.smartcase = true
vim.o.showmatch = true
vim.o.textwidth = 80
vim.o.tabpagemax = 255
vim.o.shortmess = "a"
vim.o.mouse = "a"
vim.o.report = 0
vim.o.showbreak = "=>"
vim.o.list = true
vim.o.swapfile = false
vim.o.backup = false
vim.o.writebackup = false
vim.o.winbar = '%#WinBar#--%f'
vim.o.listchars = [[multispace:+-,tab:< >,trail:$]]

-- local let = vim.g
vim.g.mapleader = " "

vim.api.nvim_set_hl(0, "CursorLine", {cterm=NONE, ctermbg=DarkGray})
vim.api.nvim_set_hl(0, "CursorColumn", {cterm=NONE, ctermbg=DarkGray})
vim.keymap.set("n", "<Leader>w",
	":set cursorline! cursorcolumn!<CR>", {silent=true})
vim.keymap.set("n", "<CR>", ":noh<CR>", {silent=true})

-- local mygroup = vim.api.nvim_create_augroup('vimrc', { clear = true })
vim.api.nvim_create_autocmd({'BufWritePre'}, {
	-- group = mygroup,
	pattern = '*',
	-- delete unnecessary EOL spaces and EOF blank lines.
	command = "silent! %s/\\s\\+$//e | silent! %s/\\n\\+\\%$//e"
})
-- vim.api.nvim_create_autocmd({ 'BufNewFile', 'BufRead' }, {
-- 	pattern = '*.html',
-- 	group = 'vimrc', -- equivalent to group=mygroup
-- 	command = 'set expandtab',
-- })

vim.api.nvim_set_keymap('n', '<leader>r', ':luafile $MYVIMRC<CR>', {noremap = true, silent = true })
