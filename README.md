# COMP-4800 Assignment 5

#### Group memebers: Kap Thang, Shubham Mehta

---

###### gtk4 and ffmpeg dev tools are required to compile this project.

[On Ubuntu/Debian] to install gtk4, run

```
sudo apt-get install libgtk-4-dev
```

[On Ubuntu/Debian] to install ffmpeg and the development tools, run

```
sudo apt-get install ffmpeg libavcodec-dev libavformat-dev libswscale-dev libavutil-dev
```

---

#### To compile the program (using `make`), run

Be in project folder (the `Makefile` should be at root level), run

```
make
```

This command will compile and run the program afterwards. Demo video attached showcasing how to compile with `make`. Demo video file name: `compilation-demo.mp4`.

<br>

#### Making changes to source code and re-compiling, run

Do this first, to use fresh .o files

```
make clean
```

Then re-compile and run the program with,

```
make
```

<br>

#### Opening a video in the program

###### Method 1: via the command-line, run

```
make ARGS="<path_to_file.mp4> <desired_fps>"
```

example:

```
make ARGS="videos/bunny_sample.mp4 30"
```

- This will play the video in the videos folder at 30 FPS in default buffered mode.
- Enter a negative value (ie. `-25`) FPS to load all frames into a single buffer (aka unbuffered mode). WARNING: program may crash if video length is long 5+ mins. Experimental feature.

###### Method 2: use the file chooser

Simple run the program without command-line argumente
ie:

```
make
```

Then click the `Open` button at the top to load a video.

<br>

#### Other features

1. Input validation implemented for the command-line inputs
1. A stats table in termal to show buffer activities

<br>

#### Other available `make` commands

1. Remove `.o` and executable files, only leave `.c` and `.h` files in `include` and `src` folders.

```
make clean
```

2. To dump all `.h` files' content into one `.txt` file, run

```
make mh
```

3. To dump all `.c` files' content into one `.txt` file, run

```
make mc
```

<br>

#### Current limitations

- Video colour is incorrect.
- Higher FPS values can cause video to play faster than the decoding process and cause a crash.
