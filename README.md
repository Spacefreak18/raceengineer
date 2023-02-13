# RaceEngineer
```
________                  __________              _____                         
___  __ \_____ ______________  ____/_____________ ___(_)________________________
__  /_/ /  __ `/  ___/  _ \_  __/  __  __ \_  __ `/_  /__  __ \  _ \  _ \_  ___/
_  _, _// /_/ // /__ /  __/  /___  _  / / /  /_/ /_  / _  / / /  __/  __/  /    
/_/ |_| \__,_/ \___/ \___//_____/  /_/ /_/_\__, / /_/  /_/ /_/\___/\___//_/     
                                          /____/                            
```
Cross Platform highly configurable tool for automated communication while playing simulators.

## Features
- Modular design for support with various titles and devices.
- "SingleShot" support for integration with other scripts

## Dependencies
- libao - cross-platform sound file library
- argtable2
- libconfig
- libxdg
- [slog](https://github.com/kala13x/slog) (static)
- [simshmbridge](https://github.com/spacefreak18/simshmbridge)
- [simapi](https://github.com/spacefreak18/simapi)

## Building
This code depends on the shared memory data headers in the simapi [repo](https://github.com/spacefreak18/simapi). When pulling lastest if the submodule does not download run:
```
git submodule sync --recursive
git submodule update --init --recursive
```
Then to compile simply:
```
mkdir build; cd build
cmake ..
make
```

## Setup
be sure to set default_driver=pulse in [libao.conf](https://man.archlinux.org/man/libao.conf.5.en) , i've only tested this using pulseaudio,

## Usage
```
raceengineer monitor --sim=ac -c $HOME/.config/raceengineer.config -p $HOME/.local/share/raceengineer/sounds/
```
one can omit -c and -p, the paths specified above are the default values.
in singelshot mode, one can specify the name of a specific config entry, for immediate response
```
raceengineer singleshot --sim=ac -m Tyre_Wear_All
```

## Config File

## Testing

### Static Analysis
```
    mkdir build; cd build
    make clean
    cmake -Danalyze=on ..
    make
```
### Valgrind
```
    cd build
    valgrind -v --leak-check=full --show-leak-kinds=all --suppressions=../.valgrindrc ./raceengineer monitor --sim=ac
```

## ToDo
 - more testing
 - explore using mqtt for handling a queue of sound files to play
 - much, much more
