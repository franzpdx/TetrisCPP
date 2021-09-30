# Tetris C++
This is a Tetris game!

## Purpose

This was my senior project, finished in 2007. It meets requirements through the following features:
* The design is object-oriented.
* It features client-server architecture. There is a multiplayer mode available by launching the server app, where sessions of the Tetris app can connect online using the server's IP. Players can host and join games on the server with up to 4 players in a session.
* A database is used by the server to save high scores. There are database options to either use a SQL Server instance, or to write to a flat (text) file.
* This project uses OpenGL for a 3D graphic environment and FMOD for sound and music.
* An installer is built using the Nullsoft Scriptable Install System (NSIS) which will install the software in Program Files, update the registry, and optionally create shortcuts on the start menu and desktop

Additional features:
* The user can change the size of the playing board. Tetris games tend to have a standard board size, and when experimenting with this feature users may find that difficulty increases quickly as the board width and height are decreased!
* User can select between several different graphic skins for block appearance.

## Design

Here is the class diagram, showing the major functions:

![Class Diagram](/doc/ClassDiagram.png)

The main loop passes the user input and current timestamp to its member objects through the advance() function. Along with the display() function, these are communicated down through the classes as individual objects act on them.

## Improvements

These are improvements that could be done for this project:

This project was built in 2007, using dependencies that have not been maintained. If you attempt to build it in 2021, expect to experience difficulty with these dependencies! I am considering updating it with current OpenGL libraries.

These c++ header files contain not only function declarations, but also definitions. Definitions should really be moved to accompanying .cpp files. 

Tetris block display is done on a very granular level. It would be more efficient to gather the whole board's state and render it at once, using instancing to take a single tetris block model and translate a copy of it into every appropriate location.

## Notice

Tetris is a registered trademark of The Tetris Company! You are viewing an independent non-commercial project.

## User Interface

- **Gallery**: The following are some screenshots from the 2007 build:
![Screenshot](/doc/tetriscpp1.jpg)![Screenshot](/doc/tetriscpp2.jpg)![Screenshot](/doc/tetriscpp3.jpg)![Screenshot](/doc/tetriscpp4.jpg)![Screenshot](/doc/tetriscpp5.jpg)![Screenshot](/doc/tetriscpp6.jpg)![Screenshot](/doc/tetriscpp7.jpg)![Screenshot](/doc/tetriscpp8.jpg)
  
