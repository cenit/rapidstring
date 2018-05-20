from collections import OrderedDict

header = """/*
 * rapidstring - A fast string library.
 * version 1.0.0
 * https://github.com/boyerjohn/rapidstring
 *
 * Licensed under the MIT License <http://opensource.org/licenses/MIT>.
 * Copyright (c) 2018 John Boyer <john.boyer@tutanota.com>.
 */\n
"""

file = open('include/rapidstring.h', 'r+')
lines = [line for line in file]
elems = OrderedDict()

for i in range(3, len(lines)):
	if lines[i].startswith(' * =') and not lines[i + 1].startswith(' */'):
		name = lines[i + 2].replace('*', ' ').strip()

		if name in elems:
			elems[name].append(i + 8)
		else:
			elems[name] = [i + 8]

toc = '/*\n *       TABLE OF CONTENTS\n *\n'
pos = 1

for key, value in elems.items():
	toc += ' * ' + str(pos) + '. ' + key + '\n'
	toc += ' * - Declarations:	line ' + str(value[0]) + '\n'
	toc += ' * - Defintions:	line ' + str(value[1]) + '\n *\n'
	pos += 1

toc = toc[:-3]
toc += ' */\n\n'

file.seek(0)
contents = file.read()
contents = header + toc + contents[contents.find('#ifndef'):]
file.seek(0)
file.write(contents)
