cmd = {}

local function isInList(str, list)
	for _, v in ipairs(list) do
		if v == str then
			return true
		end
	end
	return false
end

function media_halt()
	if next(cmd) ~= nil then
		cmd:kill('sigint')
		cmd = {}
	end
end

function media_play()
	-- Get the path of the current file under the cursor in netrw
	local filepath = vim.fn.expand("<cfile>")
	local filepath = vim.fn.expand("<cfile>")
	vim.print(filepath)
	filepath = vim.fn.fnameescape(filepath)

	vim.print(filepath)
	if #filepath < 5 then
		vim.print(filepath)
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

	vim.print(vim.b.netrw_curdir .. '/' .. filepath)
	-- cmd = vim.system({'mpv', '--loop', '--no-video', filepath}, {}):wait()
	cmd = vim.system({'mpv', '--loop', '--no-video', '--',
		vim.b.netrw_curdir .. '/' .. filepath}, {}, on_exit)
end

-- Disable mouse clicks (because they interfere with files navigation sometimes)
vim.g.netrw_mousemaps = 0

vim.api.nvim_buf_set_keymap(0, 'n', '.', '', {
	noremap = true,
	silent = true,
	callback = media_play
})
-- vim.api.nvim_buf_set_keymap(0, 'n', '<LeftMouse>', '', {
-- 	noremap = true,
-- 	silent = true,
-- 	callback = media_play
-- })
vim.api.nvim_buf_set_keymap(0, 'n', ',', '', {
	noremap = true,
	silent = true,
	callback = media_halt
})
