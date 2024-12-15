# Compile the widescreen version of Cave Story from the source code 

### Install the necessary packages:  

```
sudo apt update

sudo apt install build-essential libpng-dev libjpeg-dev make cmake cmake-data git libsdl2-dev libsdl2-doc libsdl2-gfx-dev libsdl2-gfx-doc libsdl2-image-dev libsdl2-mixer-dev libsdl2-net-dev libsdl2-ttf-dev cmake -y -qq
```


### Compile the engine (it takes about 1 hour): 
```
cd ~
git clone https://github.com/nxengine/nxengine-evo
```
### Modified to the widescreen version: 

`vim ~/nxengine-evo/src/graphics/Renderer.cpp`


### Edit the 225 line and save and exit the following line 
```
{(char *)"1280x480", 1280, 480, 640, 240, 2, true, true},
```

```
cd nxengine-evo
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
cd ~/nxengine-evo
wget https://www.cavestory.org/downloads/cavestoryen.zip
unzip cavestoryen.zip
cp -r CaveStory/data/* data/
cp CaveStory/Doukutsu.exe .
./build/nxextract
cd build
sudo make install
```

### Confirm that the original version can run normally: 
`./nxengine-evo`




### Run the modified version: 
`./nxengine-evo`

### After running, press the Esc key, enter Options->Graphics, select the resolution as 1280x480, and open the full screen. 

 
### Reinstall after confirming that there is no problem: 
```
cd ~/nxengine-evo/build
sudo make install
```

You can use the command **nxengine-evo **to run the game in any directory  