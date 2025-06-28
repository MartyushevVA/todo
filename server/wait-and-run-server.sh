#!/bin/sh

# Ожидаем доступности сервиса на хосте $WAIT_HOST:$WAIT_PORT
echo "Waiting for $WAIT_HOST:$WAIT_PORT..."

while ! nc -z $WAIT_HOST $WAIT_PORT; do
  echo "Waiting for $WAIT_HOST:$WAIT_PORT to be available..."
  sleep 1
done

echo "$WAIT_HOST:$WAIT_PORT is available, starting server..."

./server




# Запускаем сервер в бесконечном цикле — если сервер падает, ждём и перезапускаем
#while true; do
#  ./server
#  echo "Server exited with code $?. Restarting in 5 seconds..."
#  sleep 5
#done
