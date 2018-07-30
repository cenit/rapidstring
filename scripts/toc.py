from collections import OrderedDict

# TODO: Allow this method to be run from any working directory.
# TODO: Don't require this to be ran twice if a section is removed.
# TODO: Move the data out of this file.

header = '''/*
 * rapidstring - Maybe the fastest string library ever.
 * version 1.0.0
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
 * The documentation of the rapidstring library is grouped by modules. All functions are within a certain module, and a list of all modules may be seen [here](https://boyerjohn.github.io/rapidstring/modules.html).
 *
 * All functions are named based off what state the string must be. For example, `rs_heap_cat_n` may only be used to concatenate strings that are on the heap. This method is intended for internal use. However, if you know for a fact your string is larger than #RS_STACK_CAPACITY, you may use the heap methods directly to avoid a flag check and obtain a minor speedup. The same is true for `rs_stack_cat_n`, and all other stack or heap specific functions. Any functions without either infix simply means it can handle the string in any state. These are the functions intended for the average user. Furthermore, the latter functions will grow the size of the string when required, while the former functions require this to be done manually.
 *
 * There are no error codes returned from this library, as nothing ever fails. The only possible sources of failure are invalid arguments or a failed allocation. The former is undefined behavior (or assert failures in when `NDEBUG` is not defined) and the latter may be verified with `errno`. The simple truth is that nearly all applications brutally fail either way when memory runs out, and `rapidstring` takes advantage of this by never checking whether allocations succeed. If your application must handle allocation failures, you may set `errno` to `0` before calling a function that either intializes or grows the heap buffer, and then check `errno` after this call. All modern compilers will set `errno` if `malloc()` fails.
 *
 * @copyright Copyright © 2018 John Boyer.
 * @author <a href="https://github.com/boyerjohn">John Boyer</a>
 * @version 1.0.0
 */\n
'''

file = open('include/rapidstring.h', 'r+')
lines = [line for line in file]
elems = OrderedDict()

for i, line in enumerate(lines):
	if line.startswith(' * =') and not lines[i + 1].startswith(' */'):
		name = lines[i + 2].replace('*', ' ').strip()

		if name in elems:
			elems[name].append(i + 8)
		else:
			elems[name] = [i + 8]

toc = '/*\n *       TABLE OF CONTENTS\n *\n'
pos = 1

for key, value in elems.items():
	toc += ' * ' + str(pos) + '. ' + key + '\n'
	toc += ' * - Declarations:\tline ' + str(value[0]) + '\n'

	if len(value) == 2:
		toc += ' * - Defintions:\tline ' + str(value[1]) + '\n'

	toc += ' *\n'

	pos += 1

toc = toc[:-3]
toc += ' */\n\n'

file.seek(0)
contents = file.read()
contents = header + toc + docs + contents[contents.find('#ifndef'):]
file.seek(0)
file.write(contents)
file.truncate()
