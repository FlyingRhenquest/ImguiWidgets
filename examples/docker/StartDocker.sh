#!/bin/bash

# Start this docker image

docker run --rm --name requirementsmanager_container -p80:80 -p443:443 -p8080:8080 --mount source=pgdata,target=/var/lib/postgres requirementsmanager_image 
