mkdir robot-$1
cp *.cpp common.hpp Makefile dat*.txt *.sh *.conf robot-$1
tar c robot-$1 > robot-$1.tar
if [ -f "robot-$1.tar.gz" ]
then
	rm robot-$1.tar.gz
fi
gzip robot-$1.tar
rm -rf robot-$1/
