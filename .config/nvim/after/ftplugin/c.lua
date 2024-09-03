vim.opt_local.equalprg = g_init_dir .. "/clang-format.sh"


-- vim.opt_local.makeprg = "clang-tidy --quiet --extra-arg=-Weverything --checks=* % -- -DCMAKE_EXPORT_COMPILE_COMMANDS=ON"
-- vim.opt_local.errorformat = "%A%f:%l:%c: %t%*[^:]: %m,%-G%.%#"

vim.api.nvim_create_autocmd('BufWritePre', {
	-- group = init_group,
	-- buffer and pattern cannot coexist
	-- buffer is needed or else there will be conflicts with
	-- other opened files of other types
	buffer = 0,
	callback = function()
		local view = vim.fn.winsaveview()
		vim.cmd('silent! undojoin | silent! normal! gg=G')
		vim.fn.winrestview(view)
	end,
})

-------------------------------------------------------------------------------
local function clint()
	local stdout = vim.uv.new_pipe()

	-- '--extra-arg=-ansi',
	local handle = vim.uv.spawn("clang-tidy", {
		args = {'--quiet',
			'--extra-arg=-Weverything',
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
						user_data = {
							rule = _user_data,
							pointer_lines = "",
							severity = (_type == "e" and 1) or (_type == "w" and 2) or 3
						}
					}
				else
					quickfix_list[index].user_data.pointer_lines = quickfix_list[index].user_data.pointer_lines ..
						"\n" .. line
				end
			end
		else
			stdout:close()
			-- Sort: error > warning > note
			table.sort(quickfix_list, function(a, b)
				return a.user_data.severity < b.user_data.severity
			end)
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
	buffer = 0,
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
		vim.keymap.set('n', 'q', ":q<CR>", { buffer = true, noremap = true, silent = true })
		vim.keymap.set('n', '<CR>', get_error_info,
			{ buffer = true, noremap = true, silent = true })
	end
})


local function ccompile()
	local line = vim.fn.getline(1)
	local filename = vim.api.nvim_buf_get_name(0)
	local binary_name = filename:sub(1, -3) .. "_debug"
	-- Taken from https://clang.llvm.org/docs/AddressSanitizer.html
	local cc_command = "clang -O0 -g -fsanitize=address,undefined,leak " ..
		"-fno-omit-frame-pointer -fno-optimize-sibling-calls " ..
		"-o" .. binary_name .. ' ' .. filename
	for match in line:gmatch("(-%S+)") do
		cc_command = cc_command .. " " .. match
	end
	cc_command = cc_command .. " && " .. binary_name
	vim.cmd('tab terminal ' .. cc_command)
	-- vim.system({'echo', 'hello'}, { text = true }, on_exit)
end

vim.api.nvim_create_autocmd("TermOpen", {
	-- pattern = '*',
	buffer = 0,
	callback = function()
		vim.keymap.set('n', 'q', "i<C-C>", { buffer = true, noremap = true, silent = true })
	end
})

vim.keymap.set('n', '<leader>k', ccompile,
	{buffer = true, noremap = true, silent = true})
