g_in_termux = (os.getenv("TERMUX_VERSION") ~= nil)
-- g_netrw_cmd has to be defined exactly like this
g_netrw_cmd = nil

g_init_dir = vim.fn.stdpath("config")

require('mystatusline')

-- vim.api.nvim_set_hl(0, 'WinBar', {bg = 'White', fg = "Black"})
vim.api.nvim_set_hl(0, "IsModified", {fg = "Yellow"})
vim.api.nvim_set_hl(0, "IsNotModified", {fg = "White"})

if not g_in_termux then
	vim.o.clipboard = "unnamedplus"
end

-- Remember that vim.o > vim.opt
vim.o.tabstop = 8
-- vim.o.hidden = true
vim.o.shiftwidth = 8
vim.o.number = true
vim.o.ignorecase = true
vim.o.smartcase = true
vim.o.showmatch = true
vim.o.textwidth = 80
vim.o.tabpagemax = 255
vim.o.shortmess = "a"
vim.o.splitbelow = true
vim.o.mouse = "a"
vim.o.report = 0
vim.o.showbreak = "=>"
vim.o.list = true
vim.o.swapfile = false
vim.o.backup = false
vim.o.writebackup = false
-- vim.o.winbar = '%#WinBar#--%f'
vim.o.cursorline = true
vim.o.winbar = "  %#IsModified#%{&mod?expand('%:p'):''}%*%#IsNotModified#%{&mod?'':expand('%:p')}%*"
vim.o.listchars = [[multispace:+-,tab:< >,trail:$]]

-- vim.g.mapleader = " "
-- Better be local, if you want to unset it later.
vim.g.maplocalleader = " "

vim.api.nvim_set_hl(0, "CursorLine", {cterm=NONE, ctermbg=DarkGray})
-- vim.api.nvim_set_hl(0, "CursorColumn", {cterm=NONE, ctermbg=DarkGray})
vim.keymap.set("n", "<Leader>w",
":set cursorline! cursorcolumn!<CR>", {silent=true})
vim.keymap.set("n", "<CR>", ":noh<CR>", {silent=true})

local init_group = vim.api.nvim_create_augroup('vimrc', { clear = true })
vim.api.nvim_create_autocmd({'BufWritePre'}, {
	group = init_group,
	pattern = '*',
	-- delete unnecessary EOL spaces and EOF blank lines.
	command = "silent! %s/\\s\\+$//e | silent! %s/\\n\\+\\%$//e"
})

vim.api.nvim_set_keymap('n', '<leader>r', ':luafile $MYVIMRC<CR>', {noremap = true, silent = true })

-- vim.api.nvim_create_autocmd({ 'BufNewFile', 'BufRead' }, {
	-- 	pattern = '*.html',
	-- 	group = 'vimrc', -- equivalent to group=init_group
	-- 	command = 'set expandtab',
	-- })
