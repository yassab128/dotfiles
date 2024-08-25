-- function my_func(x)
-- 	return #x
-- end

-- vim.cmd("runtime vimrc")

local file_path = vim.fn.expand("%")
-- vim.opt = {
-- 	tabstop = 8
-- 	shiftwidth = 8
-- }

vim.opt.tabstop = 4
-- vim.opt.hidden = true
vim.opt.shiftwidth = 4
vim.opt.number = true
vim.opt.expandtab = false
vim.opt.clipboard = "unnamedplus"
vim.opt.ignorecase = true
vim.opt.smartcase = true
vim.opt.showmatch = true
vim.opt.textwidth = 80
vim.opt.tabpagemax = 255
vim.opt.shortmess = "a"
vim.opt.mouse = "a"
vim.opt.report = 0
vim.opt.showbreak = "=>"
vim.opt.list = true
vim.opt.listchars = {
	multispace = "+-",
	tab = "< >",
	trail = "$"
}

-- local let = vim.g
-- let.mapleader = " "

vim.cmd.highlight({"CursorLine", "cterm=NONE ctermbg=DarkGray"})
vim.cmd.highlight({"CursorColumn", "cterm=NONE ctermbg=DarkGray"})
vim.keymap.set("n", "<Leader>w",
	":set cursorline! cursorcolumn!<CR>", {silent=true})
vim.keymap.set("n", "<CR>", ":noh<CR>", {silent=true})
-- o.statusline = "%f" .. my_func("@%")

local function status_line()
	local get_octal = function(a, b, c)
			local m
			local n
			local o
			if a == 45 then
				m = 0
			else
				m = 4
			end
			if b == 45 then
				n = 0
			else
				n = 2
			end
			if c == 45 then
				o = 0
			else
				o = 1
			end
			return m + n + o
	end

	local file_perm_octal = function()
			local file_perm_string = vim.fn.getfperm(file_path)
			return get_octal(file_perm_string:byte(1),
			file_perm_string:byte(2),
			file_perm_string:byte(3)) ..
			get_octal(file_perm_string:byte(4),
			file_perm_string:byte(5),
			file_perm_string:byte(6)) ..
			get_octal(file_perm_string:byte(7),
			file_perm_string:byte(8),
			file_perm_string:byte(9))
	end

	function get_buf_count()
		return #vim.fn.getbufinfo({buflisted = 1})
	end

	function get_file_size()
		local bytes_number = vim.fn.getfsize(file_path)
		local unit
		if bytes_number > 1073741824 then
			return string.format("%.2f GiB",
			bytes_number / 1073741824)
		elseif bytes_number > 1048576 then
			return string.format("%.2f MiB",
			bytes_number / 1048576)
		elseif bytes_number > 1024 then
			return string.format("%.2f KiB",
			bytes_number / 1024)
		else
			return string.format("%.2f B",
			bytes_number)
		end
	end
	return "[%f] [0" .. file_perm_octal() .. "] [" .. get_file_size() ..
	"] %y [%n/" .. get_buf_count() ..
	"] [%p%%] [%l/%L]%=%#ErrorMsg#%m"
	-- "%=%#ErrorMsg#%m%#Question#%r%#WarningMsg#%h%#MoreMsg#%w"
end

vim.opt.statusline = status_line()

local mygroup = vim.api.nvim_create_augroup('vimrc', { clear = true })
vim.api.nvim_create_autocmd({'BufWritePre'}, {
	pattern = '*',
	group = mygroup,
	-- delete unnecessary EOL spaces and EOF blank lines.
	command = "silent! %s/\\s\\+$//e | silent! %s/\\n\\+\\%$//e"
})
-- vim.api.nvim_create_autocmd({ 'BufNewFile', 'BufRead' }, {
-- 	pattern = '*.html',
-- 	group = 'vimrc', -- equivalent to group=mygroup
-- 	command = 'set expandtab',
-- })
