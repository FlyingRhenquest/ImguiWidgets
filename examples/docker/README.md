# Building this Docker Image


This docker image sets up a nginx proxy to forward https to the backend REST service.
This enables you to run this application on a local subnet or on the open internet
if you're very foolish. The REST service is bare bones and the service is not (yet)
authenticated. If I decide to roll keycloak into this image for authentication,
the REST service will still be very bare bones. This is a simple demonstration of
how to bring up the app.


Web browsers like security and will refuse to run the application or talk to the
REST service unless it has a SSL cert. This is easy enough if you have an
internet domain, and slightly less easy if you don't. This directory includes
a script to set up a certificate authority and a self-signed server certificate.
If you're running this on a cloud service, you can automate getting a cert with
certbot or something. You can also set up keycloak, which provides a huge number
of options for allowing your users to register and access web services you're
providing.


Once you generate the keys, you'll have to install them in your browser for the
application to work (YMMV here depending on your browser if you don't -- maybe
you just get a security warning, maybe the application doesn't want to start
at all. It'll usually error out with a pthreads error message if there's
something wrong with the certs or http headers. It's not that pthreads is the
cause of the problem there, it's just that pthreads is the first thing it tries
to invoke when the browser thinks the application is dangerous. We'll get to
that in a moment, though.


This example directory will create a "pgdata" volume in docker so that the
database can persist between docker sessions. If you're not familiar with
docker, any changes you make on the image typically disappear when the
docker service is stopped. By mounting a docker volume or a data
directory on your system, you can docker applications store data
between sessions.


Personally I don't like mounting a home directory in docker though --
the image being immutable makes it a great place to do your build
prior to releasing to make sure that you don't have environment
changes on your system that other users will not have on their
systems. Building your docker image up with the dependencies the build
needs to run documents the dependencies quite nicely and has the
benefit of avoiding the "Builds on my laptop" problem that a lot of 
teams run into.


## 1. Native Build of RequirementsManager

This provides the REST service for the backend.


Assuming you git cloned this repo to ~/sandbox, run the build and copy the
debian package file to this directory:


    cd /tmp
    mkdir build_native
    cd build_native
    cmake ~/sandbox/ImguiWidgets/RequirementsManager
    make
    make package
    cp *.deb ~/sandbox/ImguiWidgets/examples/docker


# 2. Emscripten Build of ImguiWidgets


This builds the UI that runs in the browser. You must have the emscripten SDK
installed so you can invoke the emscripten toolchain.


    cd /tmp
    mkdir build_emscripten
    cd build_emscripten
    emcmake cmake ~/sandbox/ImguiWidgets
    cmake --build .
    cp GraphEditor* ~/sandbox/ImguiWidgets/examples/docker


# 3. Run "SetupKeys.sh" in this directory:


    cd ~/sandbox/ImguiWidgets/examples/docker
    ./SetupKeys.sh
    

Follow the prompts. The docker build will find the keys it needs and put 
them in the correct locations.


Here is where you add your certificate to your browser. Look for
RequirementsManager.pem. In you browser, go to 
settings -> Privacy and Security and scroll down to "Certificates."
On firefox (chrome should be similiar,) select "View Certificates"
and click on "import." Find RequirementsManager.pem and install
it here.


*WARNING*


Note that if you don't keep the keys in this directory secure,
this key could be used to run a man in the middle attack against
your browser if you install it as a trusted key. If you're
just experimenting with this app and no one else is involved,
this should be safe enough. In a corproate environment, you
should discuss this with your IT prior to attempting it.


You can also un-trust the cert when you're done working with
this. The cert will also expire in 90 days. If you're actually
using this in a browser environment, you'll need to renew the
certificate, rebuild the image and re-trust the certificate
in your browser every 90 days.


# 4. Build the image


In this directory, run:


    ./BuildDocker.sh
    
    
This script is designed so you can pass parameters to docker
build. Any parameters you add after ./BuildDocker.sh will be
passed on to docker build.


# 5. Run this image

In this directory run:


    ./StartDocker.sh
    
    
This will start the container and set up the database the
first time it runs.


Now you should be able to navigate your browser to:


https://127.0.0.1/GraphEditor.html


The GUI should run immediately. Your database probably
doesn't have anything in it yet, so create a Graph Node
(under "utilities" in the file menu, click "enable editing"
on it and name it "Test Graph" or something. You can add a
few more nodes to the graph by creating additional nodes
and dragging links from the anchors at the top and bottom
of each window to the graph node. Only one other item in
your graph needs to be linked to the graph node.


Once you have a little test graph set up, click
"File -> Save to REST service" on the graph node. It'll
pop up a dialog where it currently pops all the windows
it creates up, find the save dialog and type in

https://127.0.0.1/graph/

And hit save. If you then got to the application menu
at the top of the window and select "File -> Query REST
Service" and then enter "https://127.0.0.1/graphs" into
the URL input and hit "Refresh", you should see your
new graph listed with a "Load" button next to it.
Note that since there isn't currently a way to close
your graph, shift-refreshing the application will make
everything go away if you want to verify you can reload
the graph.
