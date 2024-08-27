function file_perm_octal()
		local function get_octal(a, b, c)
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
		local myfile = vim.fn.expand("%")
		vim.print(myfile)
		local file_perm_string = vim.fn.getfperm(myfile)
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
	local myfile = vim.fn.expand("%")
	local bytes_number = vim.fn.getfsize(myfile)
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

vim.opt.statusline = "[0%{v:lua.file_perm_octal()}] [%{v:lua.get_file_size()}]" ..
	" [%Y] [%n/%{v:lua.get_buf_count()}" ..
	"]%=[%p%%] [%l/%L] %#ErrorMsg#%r%w%h%q"
