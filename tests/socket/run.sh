echo "run gcc on $1"
gcc "$1" -lGL -lGLU -lglut -lm -o test
./test

