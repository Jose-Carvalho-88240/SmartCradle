# README

# SmartCradle

<p align="center">
    <b>Not your ordinary babysitter.</b><br>
    <br>
    <img width="250" src="https://github.com/Jose-Carvalho-88240/SmartCradle/blob/main/logo.png?raw=true" alt="SmartCradle"/>
</p>

Software Documentation can be found at [Documentation](https://rawcdn.githack.com/Jose-Carvalho-88240/SmartCradle/dcb9e30f353c03f342479c0d7f9408a93bd66c23/doc/files/html/index.html)

### Compiling the program

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
    
### Running the program

Copy every file in **bin** folder and **src/database.py** to the development board:

    scp bin/* src/database.py root@<Raspberry IP>:<Location>
    
Call **main.elf** to run the program. The daemon and device drivers will be called automatically.

### Authors:
- [José Carvalho](https://github.com/Jose-Carvalho-88240)
- [João Carneiro](https://github.com/JoaoLuis00)
