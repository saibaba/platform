argc=$#

if [ $argc -ne 1 ]; then
  echo `basename $0` group_name
  exit 1
fi

controllers="memory,cpu"
path="$controllers:/$1"
mem=10000000   #10 mb
cpu=100        #10%

x=`cgget $1 2>&1`

rv=$?

if [ $rv -eq 0 ]; then
  echo "group exists!"
else
  sudo cgcreate -g $path
  sudo cgset -r memory.limit_in_bytes=$mem $path
  sudo cgset -r cpu.shares=$cpu $path
fi
