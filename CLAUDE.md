WHY
This project is created to provide simple RPC library. It will run both on linux
and windows, more OS'es and ideas to be considered later

WHAT
System is build in C++23 with usage od Catch2 library to provide tests for systems.
Public directory provides API endpoints for users to use.
Src consist all of the source files.
Wrapper is for wrapping functions send between server and client.
Coder is to code and decode the parameters of functions.
Server implements public methods of Server.
Client implements public methods of Client.
Result consist public contract on how return value from execution of command looks like.
Frame is for framing message into bytes and from bytes, platform has platform specific functions.
In client and server directory of src lay modules for specific end of the system.
Both of those folders has OS directory where sockets are implemented

HOW
Currently project is in the moment of implementing Windows site, examples folder
has examples on what was achieved while developing on linux.
System should be implemented into the most generic way possible, with clean and minimal
dependencies.