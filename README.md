# Build

## Requirements

+ Cmake _(>= 3.7)
+ LLVM (>= 5.0)
+ GCC (>= 7.0)

## Developement

### Supported Environments

	- Arch Linux (x86__64)
	- Gentoo Linux (x86__64)

## Build
	'mkdir build'
	'cmake ..'
	'make -j'

# Usages
'USAGE: otterc [options] <input>'

'OPTIONS:'

'Complier Options:'

  '-debug     - Display debug log.'
  '-dump      - Display bitcode.'
  '-o=<file>  - Place the output into <file>.'

'Generic Options:'

  '-help      - Display available options (-help-hidden for more)'
  '-help-list - Display list of available options (-help-list-hidden for more)'
  '-version   - Display the version of this program'
