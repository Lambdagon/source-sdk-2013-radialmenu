# TF2 RadialMenu 

<img width="800" height="620" alt="radialmenu" src="https://github.com/user-attachments/assets/d5dd6543-5cdd-48c3-ad34-d103df248dfe" />

This is an attempt to port the Radial Menu system used in ASW/L4D/PORTAL2 to Team Fortress 2
It works fine for the most part aside from a couple of small bugs I'm working on fixing. 
However in it's current state it's usable via the "+mouse_menu" command
so if you want to use it in game you can do "bind z +mouse_menu". this will tie the menu to the button you bound 
I know it's called mouse_menu yet its on a keyboard button but this is how it is in Valve games

All the files needed to make this work are included in this repo. 
However for use in any other mod you need:
- The fonts and defaultsettings in clientscheme.res
- RadialMenu.res and .txt files in resource/ui and scripts folders
- Hudlayout.res entry

This project and it's code were made for Fortress Connected 
Only default menu was tested. additional menus yet to be added/tested.

All I ask is if any of this code is used in your mod that I Vvis (or the Fortress Connected mod) are credited in some way that is publicly visible.

Please report any issues or fixes via the github issue/PR systems 

# Source SDK 2013 (original description)

Source code for Source SDK 2013.

Contains the game code for Half-Life 2, HL2: DM and TF2.

**Now including Team Fortress 2! ✨**

## Build instructions

Clone the repository using the following command:

`git clone https://github.com/ValveSoftware/source-sdk-2013`

### Windows

Requirements:
 - Source SDK 2013 Multiplayer installed via Steam
 - Visual Studio 2022 with the following workload and components:
   - Desktop development with C++:
     - MSVC v143 - VS 2022 C++ x64/x86 build tools (Latest)
     - Windows 11 SDK (10.0.22621.0) or Windows 10 SDK (10.0.19041.1)
 - Python 3.13 or later

Inside the cloned directory, navigate to `src`, run:
```bat
createallprojects.bat
```
This will generate the Visual Studio project `everything.sln` which will be used to build your mod.

Then, on the menu bar, go to `Build > Build Solution`, and wait for everything to build.

You can then select the `Client (Mod Name)` project you wish to run, right click and select `Set as Startup Project` and hit the big green `> Local Windows Debugger` button on the tool bar in order to launch your mod.

The default launch options should be already filled in for the `Release` configuration.

### Linux

Requirements:
 - Source SDK 2013 Multiplayer installed via Steam
 - podman

Inside the cloned directory, navigate to `src`, run:
```bash
./buildallprojects
```

This will build all the projects related to the SDK and your mods automatically against the Steam Runtime.

You can then, in the root of the cloned directory, you can navigate to `game` and run your mod by launching the build launcher for your mod project, eg:
```bash
./mod_tf
```

*Mods that are distributed on Steam MUST be built against the Steam Runtime, which the above steps will automatically do for you.*

## Distributing your Mod

There is guidance on distributing your mod both on and off Steam available at the following link:

https://partner.steamgames.com/doc/sdk/uploading/distributing_source_engine

## Additional Resources

- [Valve Developer Wiki](https://developer.valvesoftware.com/wiki/Source_SDK_2013)

## License

The SDK is licensed to users on a non-commercial basis under the [SOURCE 1 SDK LICENSE](LICENSE), which is contained in the [LICENSE](LICENSE) file in the root of the repository.

For more information, see [Distributing your Mod](#markdown-header-distributing-your-mod).
