# Emscripten Usage

Run this in your browser just as if it's native!

# Setup

This has a lot of dependencies I really need to document, but between the
submodules and it downloading a lot of what it needs, hopefully it won't
be too odious to build. I'll put together a docker image in the next day or
two as well.

## Emscripten SDK

You need the emscripten SDK to build this. You can download and install
it [here](https://emscripten.org/docs/getting_started/downloads.html)

## CMake

Build requires a fairly recent version of CMake. The CMake file says
version 3.31, but you might be able to get away with older versions
if you change that. I wouldn't go much older than 3.25 though.

If you REALLY want to be sure, it's not terribly difficult to build for
yourself. Their github is [here](https://github.com/Kitware/CMake)

## Pistache

I just apt installed this with Debian

sudo apt install libpistache-dev

## OpenGL/GLX

I just apt installed the dev packages (libopengl-dev, libglx-dev)
for debian, for native builds. The emscripten build uses 
the emscripten SDK libraries for this.

## SDL3

The build will download this for the emscripten build.

## Postgres/pqxx

The emscripten build doesn't need these. If you want to build the native
version, you can apt install these. I ended up installing postgresql,
postgresql-client and postgresql-common-dev, and built libpqxx to
/usr/local/lib. That can be found [here](https://github.com/jtv/libpqxx)

## Boost

Any fairly recent version should work. I mostly just use signals2 from
boost, but that has like 12 dependencies in boost. The emscripten build
downloads what it needs and builds it during the build process.

## Cereal

Linux packages should work fine for the native build. The build will
download and build it if it doesn't find it.

## FlyingRhenquest Types Library

My typelist library. The build will download it if it doesn't find it.

## gcc/g++ or clang

This should compile with either compiler. The emscripten SDK seems to
come with what you need to build to emscripten.

# Building

 * Make a directory somewhere. I usually use /tmp/build
 * git clone this repo somewhere. Don't forget to use --recurse-submodules.
   I usually put my git things in ~/sandbox.
 * cd /tmp/build
 * emcmake cmake ~/sandbox/ImguiWidgets
 * cmake --build .
 * This will take a couple minutes. Ignore all those warnings.
 
While you're waiting for it to build, let's talk about the RequirementsManager
REST server. For that to work, you DO need to be running Postgres and have
populated the database with some nodes.

You also need to build the REST server natively. So you'll need to check out
the [RequirementsMananager](https://github.com/FlyingRhenquest/RequirementsManager)
repo as well and build it in a separate directory using a native build.
I'll document that in more detail in that project, but in brief you can
currently only launch the REST server from Python. Assuming you have the
Python library built, you can treate Python like a console to start the server:

    $python3
    Python 3.13.11 (main, Dec  8 2025, 11:43:54) [GCC 15.2.0] on linux
    Type "help", "copyright", "credits" or "license" for more information.
    >>> import FRRequirements
    >>> server = FRRequirements.GraphServer(8080)
    >>> server.start(2,2)
    >>>
    
You can then navigate with your browser to:

http://127.0.0.1:8080/graphs to verify that the server is running. That's 
worth checking out in case you have a firewall or something blocking it.
You should get some JSON back. If you don't have any graphs in your database
yet, you can build the RequirmentsManager tests and run them, or use the
example Python program to create a larger organization graph.

Leave that running and go back to your Emscripten build.

If the build completes successfully, you can run the application with:

    $ emrun GraphEditor.html 

This will start the application in your browser. You should see a
menu across the top of the window. You can create nodes with the
various node menu options. To load from REST, select file->Query REST Service.
Make the window bigger if I haven't fixed that yet.

In the URL type in http://127.0.0.1:8080/graphs

It should populate a menu with any graphs you have saved in your database.
Click the load button next to any of them, and it should load that graph.

Please note that I haven't implemented *saving* your graphs through the
REST API yet, but I just need to implement a POST method in the GraphNodeWindow,
and the Pistache service already supports this. The native build can save to
JSON or directly to the Postgres database via the GraphNodeWindow.
Currently all the tools look for Graph Nodes to locate graphs, so be
sure to attach a GraphNode to your graph somewhere if you want to load and
save it through the GUI mechanisms. If you're using the Python or C++ library
directly, you can load and save arbitrary nodes via the node UUID, or
implement other Postgres mechanisms to find the nodes you want to work
with. GraphNode is pretty convenient.

I hope you found this interesting and useful!
