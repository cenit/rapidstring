from collections import OrderedDict

# TODO: allow this method to be run from any working directory.

header = '''/*
 * rapidstring - A fast string library.
 * version 0.1.0
 * https://github.com/boyerjohn/rapidstring
 *
 * Licensed under the MIT License <http://opensource.org/licenses/MIT>.
 * Copyright (c) 2018 John Boyer <john.boyer@tutanota.com>.
 */\n
'''

docs = '''/**
 * @file rapidstring.h
 * @brief The single header of the rapidstring library.
 */

/**
 * @mainpage rapidstring
 * The documentation of the rapidstring library.
 */\n
'''

file = open('include/rapidstring.h', 'r+')
lines = [line for line in file]
elems = OrderedDict()

for i in range(0, len(lines)):
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

	if len(value) == 2:
		toc += ' * - Defintions:	line ' + str(value[1]) + '\n'
	
	toc += ' *\n'

	pos += 1

toc = toc[:-3]
toc += ' */\n\n'

file.seek(0)
contents = file.read()
contents = header + toc + docs + contents[contents.find('#ifndef'):]
file.seek(0)
file.write(contents)
