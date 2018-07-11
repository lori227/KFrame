
    sh "$2"kill.sh "$3"

if [ ! -n "$3" ]; then
    sh "$2"deploy.sh "$1"
    sh "$2"run.sh "$1"
else
    sh "$2"deploy.sh "$3" 
    sh "$2"run.sh "$1" "$3" 
fi

