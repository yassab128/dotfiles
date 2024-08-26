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

function myfunc()
	vim.print("Hello!")
	local stdout = vim.uv.new_pipe()
	local stderr = vim.uv.new_pipe()

	local handle, pid = vim.uv.spawn("date", {
		stdio = {nil, stdout, stderr}
	}, function(code, signal) -- on exit
		vim.print("exit code", code)
		vim.print("exit signal", signal)
	end)

	vim.uv.read_start(stdout, function(err, data)
		assert(not err, err)
		if data then
			vim.print("stdout chunk", stdout, data)
		else
			vim.print("stdout end", stdout)
		end
	end)

	vim.uv.read_start(stderr, function(err, data)
		assert(not err, err)
		if data then
			vim.print("stderr chunk", stderr, data)
		else
			vim.print("stderr end", stderr)
		end
	end)

	vim.uv.close(handle, function()
		vim.print("process closed", handle, pid)
	end)
end
