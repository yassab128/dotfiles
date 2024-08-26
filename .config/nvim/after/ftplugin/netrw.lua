if g_in_termux then
	vim.o.mouse = ''
end
vim.g.netrw_keepdir = 0

local function isInList(str, list)
	for _, v in ipairs(list) do
		if v == str then
			return true
		end
	end
	return false
end

local function media_halt()
	if g_netrw_cmd then
		g_netrw_cmd:kill('sigint')
		g_netrw_cmd = nil
	end
end

local function media_play()
	-- This one doesn't work if spaces are involved
	-- local filepath = vim.fn.expand("<cfile>")

	-- Get the path of the current file under the cursor in netrw
	local filepath = vim.fn.getline('.')

	if #filepath < 5 then
		return
	end

	local extension = filepath:sub(-3)

	local media_extensions = {
		"mp3", "wav", "flac", "aac", "ogg", "m4a",
		"avi", "mp4", "mkv", "mov", "wmv", "mpg", "mpeg", "flv",
	}

	if not isInList(extension, media_extensions) then
		vim.print("Not a media file")
		return
	end

	media_halt()

	g_netrw_cmd = vim.system({'mpv', '--loop', '--no-video', '--', filepath}, {}, on_exit)
end

vim.api.nvim_buf_set_keymap(0, 'n', '.', '', {
	noremap = true,
	silent = true,
	callback = media_play
})
vim.api.nvim_buf_set_keymap(0, 'n', ',', '', {
	noremap = true,
	silent = true,
	callback = media_halt
})
-- Disable mouse clicks (because they interfere with files navigation sometimes)
-- vim.g.netrw_mousemaps = 0
-- vim.api.nvim_set_keymap('n', '<LeftMouse>', '', { noremap = true, silent = true })
