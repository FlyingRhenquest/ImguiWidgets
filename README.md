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
 * Load/Save to database
 * Load/Save to JSON file
 * Window placement on-load is a bit crap right now.
 * There's no way to close nodes or otherwise make them go away
   other than exiting the program.

## Goals:

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

Load/Save both use graph nodes, so start your graph with a graph
node ("utlity nodes" -> "graph node" on the main menu) and link one
other node in your graph to the graph node. The graph node "File"
menu allows saving from the database or to JSON. The main editing
window File menu can load from the database or JSON.

## Todos

 * Implement a way to close graphs without having to exit.
 * Do something about window placement.
 * Modify CMake instrumentation so database is optional (AND
   pass this to RequirementsManager if I'm building it.)
 * Add emscripten build.
 * Add Copyright window for the GUI so I can cite the various
   MIT/Apache licensed dependencies in the GUI.
 * Implement other views of graphs as there are several useful
   domains that will be represented in node data.
 * Python API
 * Emcripten API
