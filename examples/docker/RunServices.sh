#!/bin/bash
#
# This is the image entrypoint
#
# Starts up the database and runs the RequirementsManager REST
# service. This will run CreateTables each time it's invoked,
# which is safe for the purposes of this demo because it
# will only create tables that don't exist.

export PATH=${PATH}:/usr/lib/postgresql/18/bin

# pgdata must match the directory we made in the dockerfile

PGDATA="/var/lib/postgres"
if [ ! -e "${PGDATA}" ]; then
    sudo mkdir -p ${PGDATA}
    sudo chown postgres:postgres ${PGDATA}
    sudo chmod 755 ${PGDATA}
fi
if [ ! -e "${PGDATA}/PG_VERSION" ]; then
    echo "No PG_VERSION detected in ${PGDATA}. Initializing database."
    # Insure the directory is otherwise emtpty
    initdb -D "${PGDATA}"
fi
echo "Starting Postgres..."
pg_ctl start -D ${PGDATA}
# Give Postgres a moment to start
sleep 5
echo "Creating tables if necessary"
CreateTables
echo "Starting nginx"
sudo service nginx start
echo "Running REST service"
# I plan to forward this through ngnix for https
# so once this works I'll set that up
RestServer -p 8080 -a 0.0.0.0

