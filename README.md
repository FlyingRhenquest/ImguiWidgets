# ImguiWidgets

This is a collection of ImGui widgets to support a GUI for my Requirements
Manager project. I'm currently building out display window support for
the various node types RequirementsManager supports.

The initial development is for a data editor in which you will be able
to create a graph of nodes and link the nodes together. I'm quite
curious to see whether I can build this in web assembly and send
JSON data over to it via a rest service. Once the graph editor is
complete, I'll work on other data views as well. The graph editor will
be the generic approach that just lets you build out your nodes
however you want.

Currently this is just a checkpoint commit into git.

## Whats here RIGHT NOW

 * Widgets to support editing RequirementsManager Nodes. All current
   RequirementsManager Nodes are implemented.
 * NodeEditorWindow that can create the current Node Windows
   that I've implemented.
 * Initial support to load nodes from the database, but there's still
   no way to save them from here (That should be pretty easy to implement).
   Database can be loaded with a graph node or the python example
   program for testing the GUI.
 * Window placement on-load is a bit crap right now.
 * There's no way to close nodes or otherwise make them go away
   other than exiting the program.

Vulkan is nominally supported in the Makefile as I wasn't sure
if OpenGL or Vulkan was the way I wanted to go with this, but it
looks like it's OpenGL. I'll probably clean the Vulkan stuff
out fairly soon, since apparently the vulkan-dev Debian package
is not what find_package is looking for.

## Goals:

 * Add save button to Graph Node to save to database
 * Enable export to JSON file
 * Enable load from JSON file
 * Emscripten support to run in a web brower (Will need to create a node
   REST service in RequirementsManager and communicate with websockets or
   something)
 * Provide different views that graphs with specific data can
   be loaded in to (time sequence, notebook, et al.)

## Notes:

I don't usually do that much front end, so my Gui code is probably
awkward. It feels awkward anyway. This should improve with practice
though.

## Using

Node windows provide anchors at the top and bottom of the window.
These can be dragged to another anchor to link the two windows.

If a link already exists, dragging the one anchor to the opposite
end again will unlink the two nodes.

CommitableNodeWindows also have anchors to the left and right. I
put these controls at the top of the window so they won't interfere
with the other items in the window. I'm really starting to rethink
the whole CommitableNode thing. Feels like YAGNI and if it's not
YAGNI the implementation could be better. May deprecate.

Node windows have individual "Enable Editing" checkboxes which must
be checked if you want to edit the data in that node. NodeWindow
has several options for displaying this and could be set by default
to allow editing of any editable fields. I'm leaning toward displaying
the control and having it default to off just because I frequently
find myself accidentally putting text in random windows.

The load/save for database and eventually JSON to the GUI looks for
Graph Nodes to load. So you should always associate a graph node
with your data nodes to make sure you can find them again. Set the
title to something informative.

## Todos

 * Implement other views of graphs as there are several useful
   domains that will be represented in node data.
 * Python API
 * Emcripten API
