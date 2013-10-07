for i in `seq 1 100`;
do
	./testdriver ../test_data/inter_test.txt
	if [ $? -ne 0 ]; then
		break
	fi
done
