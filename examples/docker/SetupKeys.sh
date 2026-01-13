#!/bin/bash

#
# In order to serve REST data to the emscripten version of the
# GUI application built with RequirementsManager, you're going
# to need to serve it data with SSL, which I haven't set
# Pistache up to do. So the docker image will need to run
# nginx with some certificates.
#
# If you're runing this on a cloud service, you could use
# certbot with an external facing domain. If you don't have
# an external domain and are doing local testing on a subnet,
# a self-signed root CA is really the way to go. You'll just
# need to trust your root CA certificate on your browser or you'll
# receive "Page is insecure" messages or some such.
#
# The Openssl commands will prompt you for a password or
# passphrase, which you should use if you intend to trust
# your CA on a browser.

echo "Checking root CA..."
# if rootCA.key doesn't exist, create it
if [ ! -e RequirementsManager.key ]; then
    echo "Generating root key"
    openssl genrsa -aes256 -out RequirementsManager.key 4096
fi

echo "Checking root cert..."
# If rootCA.pem doesn't exist, create it
# It will expire in 90 days, which you can modify if you want to
if [ ! -e RequirementsManager.pem ]; then
    echo "Generating root cert"
    openssl req -x509 -new -nodes -key RequirementsManager.key -sha256 -days 90 -out RequirementsManager.pem
fi

echo "Checking server key..."
# Create a server key
if [ ! -e RequirementsManagerServer.key ]; then
    echo "Generating server key"    
    openssl genrsa -out RequirementsManagerServer.key 2048
fi

# You need a certificate signing request to sign the server key
# will assume if one exists it's valid
echo "Checking CSR"
if [ ! -e RequirementsManagerServer.csr ]; then
    echo "Creating signing request"
    openssl req -new -key RequirementsManagerServer.key -out RequirementsManagerServer.csr
    # And also sign the certificate
    openssl x509 -req -in RequirementsManagerServer.csr -CA RequirementsManager.pem -CAkey RequirementsManager.key -CAcreateserial -out RequirementsManagerServer.crt -days 365 -sha256 -extfile RequirementsManagerServer.ext
fi

# Build server bundle if it doesn't exist
echo "Checking server bundle"
if [ ! -e server-bundle.crt ]; then
   cat RequirementsManagerServer.crt RequirementsManager.pem > server-bundle.crt
fi
