#!/bin/bash
FILES=$(echo $(echo $(cut -f2 ~/.crispy/crispy_info) | tr '\n' ' ') | sed 's/~/$HOME/g')
FILES=$(eval echo $FILES)
echo "Watching:" $FILES
while true; do
	inotifywait -qq $FILES # -e modify,attrib,close_write,move,create,delete
	./crispy sync 
done
