function file_info()
	local stat = vim.uv.fs_stat(vim.fn.expand("%"))
	if not stat then
		return ""
	end
	-- vim.print("HHHHHHHHHHHHHHHHHHHHHHHHHH")
	-- vim.print(stat)
	local hmode = string.format("%o", stat.mode % 4096)

	local size = stat.size
	if size > 1073741824 then
		hsize = string.format("%.2fG",
		size / 1073741824)
	elseif size > 1048576 then
		hsize = string.format("%.2fM",
		size / 1048576)
	elseif size > 1024 then
		hsize = string.format("%.2fK",
		size / 1024)
	else
		hsize = string.format("%.2fB",
		size)
	end

	local hmtime = os.date("%Y-%m-%d %H:%M:%S", stat.mtime.sec)

	return string.format("[%d] [%s] [%s]",
		hmode, hsize, hmtime)
end

function get_buf_count()
	return #vim.fn.getbufinfo({buflisted = 1})
end

vim.opt.statusline = "%{v:lua.file_info()}" ..
	" [%Y] [%n/%{v:lua.get_buf_count()}" ..
	"]%=[%p%%] [%l/%L] %#ErrorMsg#%m%r%w%h%q"
