vim.opt_local.equalprg = g_init_dir .. "/clang-format.sh"


-- vim.opt_local.makeprg = "clang-tidy --quiet --extra-arg=-Weverything --checks=* % -- -DCMAKE_EXPORT_COMPILE_COMMANDS=ON"
-- vim.opt_local.errorformat = "%A%f:%l:%c: %t%*[^:]: %m,%-G%.%#"

vim.api.nvim_create_autocmd('BufWritePre', {
	group = init_group,
	pattern = '*',
	callback = function()
		local view = vim.fn.winsaveview()
		vim.cmd('silent! undojoin | silent! normal! gg=G')
		vim.fn.winrestview(view)
	end,
})

-------------------------------------------------------------------------------
local function clint()
	local stdout = vim.uv.new_pipe()

	local handle = vim.uv.spawn("clang-tidy", {
		args = {'--quiet',
			'--extra-arg=-Weverything ',
			'--checks=*,-llvmlibc-restrict-system-libc-headers,-readability-avoid-unconditional-preprocessor-if',
			vim.fn.expand("%"), '--', '-DCMAKE_EXPORT_COMPILE_COMMANDS=ON'},
		stdio = {nil, stdout, nil}
	})

	if handle == nil then
		vim.print("Error encountered while running vim.uv.spawn()")
		return
	end

	quickfix_list = {}
	local index = 0
	vim.uv.read_start(stdout, function(err, data)
		if data then
			for line in data:gmatch("[^\n]+") do
				if line:sub(1, 1) == "/" then
					index = index + 1
					local _filename, _lnum, _col, _type, _text,
						-- Don't change the pattern. It works, don't question it.
						_user_data = line:match("(.+):(%d+):(%d+): (%l).+: ([^[]+[^[ ]) *(%[*.*)")
					quickfix_list[index] = {
						filename = _filename,
						lnum = _lnum,
						col = _col,
						type = _type,
						text = _text,
						user_data = {rule = _user_data, pointer_lines = ""}
					}
				else
					quickfix_list[index].user_data.pointer_lines = quickfix_list[index].user_data.pointer_lines ..
						"\n" .. line
				end
			end
		else
			stdout:close()
			vim.schedule(function()
				vim.fn.setqflist(quickfix_list, 'r')
				vim.cmd("copen | wincmd p")
			end)
		end
	end)
	handle:close()
end

vim.api.nvim_set_keymap('n', '<leader>n', ':cnext<CR>', { noremap = true, silent = true })
vim.api.nvim_set_keymap('n', '<leader>p', ':cprevious<CR>', { noremap = true, silent = true })

vim.api.nvim_set_hl(0, "QuickFixLine", {
	bg = "DarkRed",
	bold = true,
})

vim.api.nvim_create_autocmd("BufWritePost", {
	pattern = "*",  -- Apply to all files or specify a filetype
	callback = clint
})

local function get_error_info()
	local line_index = vim.fn.line('.')
	local qflist = vim.fn.getqflist()
	local entry = qflist[line_index]
	-- vim.print() should't be used as it doesn't print the whole message
	vim.api.nvim_echo({{entry.user_data.rule .. "\n" .. entry.user_data.pointer_lines .. "\n"}},
		false, -- Do not show in history (:messages)
		{})
end

vim.api.nvim_create_autocmd('FileType', {
	pattern = 'qf',
	callback = function()
		vim.keymap.set('n', '<CR>', get_error_info,
			{ buffer = true, noremap = true, silent = true })
	end
})

----------------------------------------------------------
-- local lsp = vim.lsp
-- local lspconfig = {
-- 	name = 'clangd',
-- 	cmd = { 'clangd' },  -- Command to start the language server
-- 	filetypes = { 'c', 'cpp', 'objc', 'objcpp' },  -- Filetypes to enable clangd for
-- 	root_dir = vim.fn.getcwd(),  -- Use the current working directory as the root directory
-- 	settings = {},  -- Add any clangd-specific settings here
-- }
--
-- -- Create an autocommand to start the LSP client when opening C/C++ files
-- vim.api.nvim_create_autocmd('FileType', {
-- 	pattern = { 'c', 'cpp', 'objc', 'objcpp' },
-- 	callback = function()
-- 		lsp.start_client(lspconfig)
-- 	end,
-- })
--
-- -- Set up key mappings for LSP functionality
-- vim.api.nvim_set_keymap('n', 'gd', '<cmd>lua vim.lsp.buf.definition()<CR>', { noremap = true, silent = true })
-- vim.api.nvim_set_keymap('n', 'K', '<cmd>lua vim.lsp.buf.hover()<CR>', { noremap = true, silent = true })
-- vim.api.nvim_set_keymap('n', '<leader>rn', '<cmd>lua vim.lsp.buf.rename()<CR>', { noremap = true, silent = true })
-- vim.api.nvim_set_keymap('n', '<leader>ca', '<cmd>lua vim.lsp.buf.code_action()<CR>', { noremap = true, silent = true })
