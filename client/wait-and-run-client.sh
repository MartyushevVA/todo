#!/bin/sh

echo "Waiting for $WAIT_HOST:$WAIT_PORT..."

while ! nc -z $WAIT_HOST $WAIT_PORT; do
  echo "Waiting for $WAIT_HOST:$WAIT_PORT to be available..."
  sleep 1
done

echo "$WAIT_HOST:$WAIT_PORT is available, starting client..."

while true; do
  ./client $WAIT_HOST $WAIT_PORT
  echo "Client exited with code $?. Restarting in 5 seconds..."
  sleep 5
done