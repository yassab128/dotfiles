#!/usr/bin/env python3

from sys import argv
from zipfile import ZipFile
# with zipfile.ZipFile(path_to_zip_file, 'r') as zip_ref:
# 	zip_ref.extractall(directory_to_extract_to)

argc = len(argv)
if argc == 2:
	zip = ZipFile(argv[1])
	zip.extractall()
else:
	print('Usage:', argv[0], '[FILE] ')
