if [ $1 -z ]; then
    echo "Missing container name"
    exit 1
fi
if [ $2 -z ]; then
  echo "Missing type"
  exit 1
fi

if [ $2 == 1 ];then
  sudo docker logs "$1" > "logs_${1}_data.txt"
  echo "Logs : $1"
fi