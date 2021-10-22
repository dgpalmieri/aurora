# Aurora

Notes from class 2021-09-30:

- EPR is a good place to start
  - We should start here and work out
- Code and Storm directories are probably not useful
- There is definitely duplicate code (for example, EPR/Library/rays.c and
Utils/rays.c are almost exactly the same)
- We are currently missing some textures, which Genetti will be providing later

### Decompressing the Textures

Run `tar zxvf textures.tar.gz` at the top level of the repository to un-gzip and un-tar the
textures tarball - this should automatically put all the textures where they need to go.

## Build EPR

This project requires the following:
  - GCC
  - make

A typical linux install would be:  
`sudo apt install gcc make`

A windows install can be done, but it is recommended that you install using WSL and follow the linux instructions.

Start by cloning this project:  
  - `git clone git@github.com:dgpalmieri/aurora.git`

Build Dependencies:  
  - `cd aurora\EPR\Library`
  - `make clean libepr.a`
  - `cd ..`
  - `make clean epr`
  - `cd ../Code`
  - `make sp2ppm`

Move sp2ppm to bin, make sure your .local/bin is in `$PATH`.  
  - `mv sp2ppm /home/$USER/.local/bin`

You can check that this is working by running `which sp2ppm` which should return: `/home/$USER/.local/bin/sp2ppm`

## Rendering

Once the system is build you can now render your first images.  
The scirpt tacc1 provides a simple 1 frame render. 

`cd Aurora/EPR`

tacc1 will produce a single frame from 7400 to 7400 by steps of size 1. below is the name of outfile, and -res provides pixel resolution. You must provide a square res to render the fisheye.    

`tacc1` also uses the cam.se59 camera path, and uses the se59_08K configuration file.

`./epr.gre se59_08K cam.se59 -range 7400 7400 1 below -res 4096 4096`  

### Example

To render frames 6500 to 7500 with step size 10 on the cam.se59 path using the se59_04k config with 1920x1920 res.  
The command would be `./epr.gre se59_08K cam.se59 -range 6500 7500 10 below -res 1920 1920`  
Once the command finishes, there will be several `below_{frame number}.green` files.  

## Converting to PPM

The `sp2ppm` utility is used to generate ppm from the energy levels created by `epr`.  
There is a helper script that is included to generate the `ppm` images.  
The script provided to generate `*.ppm` for each frame is `conv_raw`.  

The script will step through the current directory matching all files that are named `below_{frame_id}.green` and generating a matching `below_{frame_id}.ppm` file that is the image.  

The single file command is as follows:  
 - `sp2ppm $b $b.ppm 0.0 600.0 0.0 400.0 0.0 400.0`

Where b is the file name without the .green extension.  

### Example

To swap a single file to `.ppm` the command is as follows:
I would like to render `below_06500.green` to `below_06500.ppm`    
  - `sp2ppm below_06500 below_06500.ppm 0.0 600.0 0.0 400.0 0.0 400.0`

The parameters are tuning of energy levels to color. There are examples of other color mappings in the `conv_raw` script. 

## Opening PPM

To open the `*.ppm` [Inkscape](https://inkscape.org/) or [Irfanview](https://www.irfanview.com/) can be used.

## Making a video

### FFMPEG

ffmpeg is a free way to make a ppm into a video.  
Install with `sudo apt install ffmpeg`

To render a sequence into a video you can use the following command  
`ffmpeg -pattern_type glob -s 1920x1920 -framerate 25 -i "*.ppm" output_vid.avi`

This will grab all ppm files in the directory, set framerate to 25 and the scale at 1920x1920. The output file will be `output_vid.avi`, feel free to mess around with these as you make different changes to renders.