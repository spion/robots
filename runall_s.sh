fls=`ls data*.txt`
for file in $fls
do
	./runsonar.sh $file
done
