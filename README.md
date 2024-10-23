# Logger

This static library allows you easy to keep a logs.
Also, you can use custom asserts with logging (lassert), 
and create own dumps.

## Dependencies

### Make

Ubuntu:

```bash
sudo apt-get update
sudo apt install make
```
Windows:

[Guide](https://stackoverflow.com/questions/32127524/how-to-install-and-use-make-in-windows)

### gcc

Ubuntu:

```bash
sudo apt-get update
sudo apt install gcc
```

Windows:

[Guide](https://dev.to/gamegods3/how-to-install-gcc-in-windows-10-the-easier-way-422j)

## Usage

```bash
cd PATH/TO/DIR/logger/
make build
```
Then, you need to include ```liblogger.h``` and link ```liblogger.a```.

### Make commands
- build
- all
- clean
- clean_SMTH

### Make variables
- **ADD_FLAGS** - your compilation flags will be added to the built-in ones
- **FLAGS** - you can redefine all compile flags
- **DEBUG_** - default value 1 (enable). Switch to 0, for disable