# Compiling Minesweeper (C Version)

This project uses a Makefile-based build system on Windows with CMD/PS. Follow the instructions below to configure, build, and run the application.

---

## Prerequisites MSYS2

### Step 1: Install MSYS2 and ensure the following packages are installed in the UCRT64 environment:

```bash
pacman -S mingw-w64-ucrt-x86_64-gcc 
pacman -S mingw-w64-ucrt-x86_64-cmake
pacman -S mingw-w64-ucrt-x86_64-make
pacman -S mingw-w64-ucrt-x86_64-sdl3
```
Make sure to add MSYS2 to your ```PATH```. Usually ```C:\msys64\ucrt64\bin```

## Opening VS Code

### Step 1: Open Windows CMD

    Windows Key + r
    cmd
    hit enter

### Step 2: Navigate to the project location

    cd PATH_TO_PROJECT\minesweeper_c

### Step 3: Run VS Code

    code .

This is the best way to open the project to ensure compatability.

## Build Instructions in VS Code

Open the terminal in VS Code. VERIFY THAT YOU ARE USING THE CMD TERMINAL. All these commands will take place in the ```root``` of the project. ```C:\PATH_TO_PROJECT\minesweeper_c>```

### Step 1: Navigate to project root:

    cd PATH_TO_PROJECT\minesweeper_c

### Step 2: Clean Any Previous Builds (Optional)

    make clean

This will remove any prexisting ```build``` directory.

### Step 3: Configure and Generate Build System

    make setup

This will create the a build directory (if it does not exist yet), ```cd``` in to it and run ```cmake``` with some requirements.

### Step 4: Make (Debug/Release)

    make

By defautl this will make to the ```Debug``` directory if you wish to specify for ```Release``` run the follow:

    make release

This is create the .exe and put its in the ```build/bin/Debug``` or ```Release``` directory.

### Step 5: Run (Debug/Release)

    make run

By default this will run the ```Debug``` .exe. If you wish to specify for ```Release``` .exe, run the follow:

    make run CONFIG=Release