input=$1
fileName="${input%.*}"
clear
g++ -O3 -o "${fileName}.out" "${fileName}.cpp" $(root-config --cflags --libs) && ./"${fileName}.out"