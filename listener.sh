FILES=$(echo $(cut -f2 crispy_info) | tr '\n' ' ')
echo "Watching:" $FILES
while true; do
	inotifywait -qq $FILES # -e modify,attrib,close_write,move,create,delete
	echo "sync"
done
