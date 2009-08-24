fls=`ls data*.txt`
for file in $fls
do
	./runson3d.sh $file
done
