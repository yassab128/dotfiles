vim.api.nvim_set_option_value('formatprg', "clang-format --fno-color-diagnostics --style='{UseTab: Always, IndentWidth: 8, AlwaysBreakAfterReturnType: All, AlignAfterOpenBracket: DontAlign, IndentPPDirectives: AfterHash, BreakBeforeBraces: Linux}'", {})

vim.api.nvim_create_autocmd('BufWritePre', {
	pattern = '*',
	callback = function()
		-- Save cursor position and window view before formatting
		local view = vim.fn.winsaveview()
		-- Perform formatting
		vim.cmd('silent! normal! gggqG')
		-- Restore cursor position and window view
		vim.fn.winrestview(view)
	end,
})

-------------------------------------------------------------------------------
function clint()
	local stdout = vim.uv.new_pipe()

	local handle = vim.uv.spawn("/usr/bin/clang-tidy", {
		args = {'--quiet', '--extra-arg=-Weverything', '--checks=*',
			vim.fn.expand("%"), '--', '-DCMAKE_EXPORT_COMPILE_COMMANDS=ON'},
		stdio = {nil, stdout, nil}
	})

	if handle == nil then
		vim.print("Error encountered while running vim.uv.spawn()")
		return
	end

	vim.uv.read_start(stdout, function(err, data)
		if data then
			vim.print(">> ", data)
		else
			stdout:close()
		end
	end)

	handle:close()
end


-- clang-check -extra-arg=-Weverything hello.c

-- local function run_command_async(cmd, callback)
-- 	local stdout = ""
-- 	local stderr = ""
-- 	local exit_code = nil
--
-- 	-- Create a new process
-- 	local handle
-- 	local pid
-- 	local on_exit = function(code)
-- 		exit_code = code
-- 		if callback then
-- 			callback({ exit_code = exit_code, stdout = stdout, stderr = stderr })
-- 		end
-- 	end
--
-- 	-- Set up stdout and stderr streams
-- 	local function on_stdout(data)
-- 		if data then
-- 			stdout = stdout .. data
-- 		end
-- 	end
--
-- 	local function on_stderr(data)
-- 		if data then
-- 			stderr = stderr .. data
-- 		end
-- 	end
--
-- 	-- Run the command
-- 	handle, pid = vim.loop.spawn(cmd, {
-- 		stdio = {1, 2, 2} -- 1 = stdout, 2 = stderr
-- 	}, function(code)
-- 		on_exit(code)
-- 		handle:close()
-- 	end)
--
-- 	-- Handle stdout and stderr
-- 	vim.loop.read_start(handle:get_stdin(), on_stdout)
-- 	vim.loop.read_start(handle:get_stderr(), on_stderr)
-- end

-- vim.api.nvim_create_autocmd({'BufWritePost'}, {
-- 	group = cgroup,
-- 	pattern = '*',
-- 	callback = run_command_async("date", function(result)
-- 		vim.print(result.stderr)
-- 	end)
-- })

-- function myfunc()
-- 	vim.print("BEGIN!")
-- 	local stdout = vim.uv.new_pipe()
-- 	local stderr = vim.uv.new_pipe()
--
-- 	local handle = vim.uv.spawn("/usr/bin/clang-check", {
-- 		args = {'-extra-arg=-Weverything', vim.fn.expand("%")},
-- 		stdio = {nil, stdout, stderr}
-- 	})
--
-- 	vim.uv.read_start(stdout, function(err, data)
-- 		if err then
-- 			vim.print("stdout end", stdout)
-- 		end
-- 		if data then
-- 			vim.print("stdout chunk", data)
-- 		else
-- 			vim.uv.close(stdout)
-- 		end
-- 	end)
--
-- 	vim.uv.read_start(stderr, function(err, data)
-- 		if err then
-- 			vim.print("stderr end", stderr)
-- 		end
-- 		if data then
-- 			vim.print("stderr chunk", data)
-- 		else
-- 			vim.uv.close(stderr)
-- 		end
-- 	end)
--
-- 	handle:close()
-- 	vim.print("END!")
-- end
