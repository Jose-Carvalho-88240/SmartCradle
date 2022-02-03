# README
# Compiling the program

To **compile the program for the target**:
    
    make src

- Images will be compiled to bin/

To **generate the software documentation**:

    make documentation
    
- HTML page can be viewed in doc/html/index.html

To **compile the program for the target and generate the software documentation**:
    
    make all

To **clean the compiled images and documentation**:

    make clean
    
# Running the program

Copy every file in **bin** folder and **src/database.py** to the development board:

    scp bin/* src/database.py root@<Raspberry IP>:<Location>
    
Call **main.elf** to run the program. The daemon and device drivers will be called automatically.
