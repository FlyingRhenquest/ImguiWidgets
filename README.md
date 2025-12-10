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

 * A basic window/widget setup with some base classes
 * Node windows for a couple of different node types.
 * An imgui opengl demo.

Vulkan is nominally supported in the Makefile as I wasn't sure
if OpenGL or Vulkan was the way I wanted to go with this, but it
looks like it's OpenGL. I'll probably clean the Vulkan stuff
out fairly soon, since apparently the vulkan-dev Debian package
is not what find_package is looking for.

## Goals:

 * Have windows for all the different node types to provide editing
   support for data in nodes.
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

CommitableNodeWindows also have anchors to the left and right.
Currently placement is at the vertical center of the window, but this
interferes with the text widget display. When I get around to addressing
that I'll need to decide if I want to just relocate them to the top
or if I should add some ImGui layout code.

Node windows have individual "Enable Editing" checkboxes which must
be checked if you want to edit the data in that node. NodeWindow
has several options for displaying this and could be set by default
to allow editing of any editable fields. I'm leaning toward displaying
the control and having it default to off just because I frequently
find myself accidentally putting text in random windows.

At the moment there isn't really a way to save data, but at the
moment there are only a few node windows hard coded into the
main.cpp demo anyway.

Once I do have data saving, the factories I build for it will
look for GraphNodes specifically. So if you want to be able to
find your graphs, you should always have a graph node connected
to the graph somewhere.

## Todos

 * Implement edit windows for all the different node types in
   RequirementsManager
 * Derive an editor window from GridWindow that allows creation
   of new nodes as well as loading and saving from the database,
   REST or JSON text files.
 * Implement other views of graphs as there are several useful
   domains that will be represented in node data.
 * Python API
 * Emcripten API