clear
echo "executing make clean..."
echo ""
make clean

echo "executing make... "
echo ""
make NEWCODE=1 USE_NEW_IMAD=1

echo "make executed... running the program..."
echo ""
bin/new_router -i ibm_mad_benchmark/ibm04_mad.dat
echo ""
#echo "copying output file..."
#cp output/desmal_1.out dev/

echo "execution complete..."
