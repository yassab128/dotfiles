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

	if #filepath < 5 then
		return
	end

	mvpause()

	local extension = filepath:sub(-3)

	local media_extensions = {
		"mp3", "wav", "flac", "aac", "ogg", "m4a",
		"avi", "mp4", "mkv", "mov", "wmv", "mpg", "mpeg", "flv",
	}

	if not isInList(extension, media_extensions) then
		return
	end

	-- cmd = vim.system({'mpv', '--loop', '--no-video', filepath}, {}):wait()
	cmd = vim.system({'mpv', '--loop', '--no-video', filepath}, {}, on_exit)
end
