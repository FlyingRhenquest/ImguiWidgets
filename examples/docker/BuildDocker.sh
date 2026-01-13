#!/bin/bash
#
# Build this docker image

#
# We'll use a volume so the database persists between runs.
# We'll only create it if it doesn't already exist.
#
PGDATA_EXISTS=$(docker volume ls | grep pgdata | wc -l)
if [ "${PGDATA_EXISTS}" = "0" ]; then
    echo "Creating data volume"
    docker volume create pgdata
fi

docker build $* -t "requirementsmanager_image" .
