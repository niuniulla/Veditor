# VEDITOR

I would like to build an editor driven by AI technologies.
Notoriously, a voice commanded editor, which can save user from relying on keyboard input.
For now, it is only on dev phase, so the UI is for more for debgging and testing purposes.

![Screenshot from 2024-04-18 00-29-50](https://github.com/niuniulla/Veditor/assets/22479680/b368688d-2c67-4ad6-862a-240157c2d027)


## Build

### install

 * create build folder:
```bash
    mkdir build && cd build
```
 * generate
 ```bash
    cmake ..
 ```
 * build
 ```bash
    make
 ```

Finally, launch the executable in bin folder from build folder:
 ```bash
    ./bin/vedit
 ```

## Platform 

 * Linux

## Dependencies

 * WhisperCpp
 * ImGui
 * ImPlot
