<p align="center">
<img width="554" src="https://user-images.githubusercontent.com/204594/113575181-c946a400-961d-11eb-8347-a8829fa3830c.png">
</p>

# DevilutionX Fork

This is modified version of the [DevilutionX](https://github.com/diasurgical/DevilutionX) repository.

## What is changed ?

Mainly addition of the `bifrost.cpp` and `bifrost.h`.<br>
It starts server when `devilutionx.exe` is run and AI Agent from [DeepDungeon](https://github.com/lciesielski/DeepDungeon) request for game data, every step it takes.<br>
Besides that, `devilutionx.exe` accepts new argument `--learning` which will force butcher quest to always spawn and it will prevent user to travel further than level 2 of the cathedral.<br>

## Building from Source

Since this is a fork I'm gonna piggyback on the [instructions](https://github.com/diasurgical/DevilutionX/blob/master/docs/building.md) of the original repository.<br>
Build process has not changed, I'm using `Windows via Visual Studio` section with `Through Open->CMake in Visual Studio` option.<br>
**Note:** Build against **x64-RelWithDebInfo**<br>

## Setup Instructions

Once built you'll have to run `aiTools/initial_setup.bat`<br>
This will create `Saves` folder structure for each client in the build root<br>
It will also create `Config` folder which holds `diablo.ini` file<br>

## Credits

[DevilutionX](https://github.com/diasurgical/DevilutionX) and [Everyone](https://github.com/diasurgical/DevilutionX/graphs/contributors) who worked on this project (as well as OG [Devilution](https://github.com/diasurgical/devilution))
