# UPDATE 1.4

This repo is now deprecated. Please DO NOT USE.

Download directly from HorizonMW, and everything should just work!

# Introduction

This copy of HorizonMW's source code, has been altered to run under a WINE environment on Linux. Do NOT attempt to run this binary from Windows, you WILL have problems. While this is for Linux only, some users have reported success with use on MacOS. I will not provide any support outside of Linux.

For pre-compiled WINE-compliant binaries please see [releases](https://github.com/MichaelDeets/HorizonMW-Client/releases) for more information.


## Installation

Download `hmw-mod.exe` from [releases](https://github.com/MichaelDeets/HorizonMW-Client/releases), place this file inside your HorizonMW root directory.

Note: Using HMW Launcher to verify the files will replace `hmw-mod.exe` with the original copy (that doesn't work in Linux). If you use HMW Launcher to verify files, make sure to re-replace the `hmw-mod.exe` file inside the root directory.

## Bugs / Issues

Please report any issues you have with running `hmw-mod.exe` through WINE/Linux. If you have problems unrelated to WINE/Linux, instead use the original [HorizonMW GitHub](https://github.com/HorizonMW/HorizonMW-Client).

## UMU-Launcher

It is now possible to use UMU-Launcher to play HorizonMW. 

The most important thing being the WINE prefix location. The configuration below will use the default `~/.wine` location, you might want to change this. There is one requirement for whichever prefix location you use; within `drive_c/Program Files (x86)/Steam` you must include the files `steamclient64.dll` and `GameOverlayRenderer64.dll`, otherwise the game will crash before the Makarov screen. 

`steamclient64.dll` and `GameOverlayRenderer64.dll` can be found within the root of the Steam directory `~/.local/share/Steam/`.

My recommendation, is to create a basic .toml configuration file; you can use the following as an outline:
```
[umu]
prefix = "~/.wine"
proton = "~/.local/share/Steam/compatibilitytools.d/UMU-Latest"
game_id = "0"
exe = "~/.local/share/Steam/steamapps/common/Call of Duty Modern Warfare Remastered/hmw-mod.exe"
launch_args = ["-nosteam"]
store = "steam"
```
Change the `exe` value, to the location where you have HorizonMW installed. This configuration assumes you are using the default location where Modern Warfare Remastered (2017) is installed by Steam. You can also adjust the `proton` value, if you want to use something like Proton-GE, currently this uses the default Proton version installed when using UMU.

### Launch using UMU

Save this file somewhere, with the extension of `.toml`, such as `hmw.toml`. 
After saving the configuration file, launch using:

`umu-run --config hmw.toml`

change `hmw.toml` to your configuration file name, and include location if required.

## Game not launching

1: Try running `hmw-mod.exe` from Steam (add a non-Steam game), using Proton Experimental or Proton GE.

2: Try adding `-nosteam` as a launch option.

## Game hangs on the first loading screen

Try re-adding the shortcut to Steam.
