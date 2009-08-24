fls=`ls data*.txt`
for file in $fls
do
	./runani.sh $file
done
