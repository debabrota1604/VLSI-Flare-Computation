clear
echo "executing make clean..."
echo ""
make clean

echo "executing make... "
echo ""
make NEWCODE=1 USE_NEW_IMAD=1


echo "clearing data on output file..." 
echo "" > output.txt

echo "make executed..."
echo "Running the program with file ibm01_mad.dat..."
echo "Running the program with file ibm01_mad.dat..." >> output.txt
echo "Output written to output.txt"
echo ""
bin/new_router -i ibm_mad_benchmark/ibm01_mad.dat >> output.txt
echo ""
sh compile_devProg.sh >> output.txt
echo "execution complete..."
echo "" >> output.txt


echo "Running the program with file ibm03_mad.dat..."
echo "Running the program with file ibm03_mad.dat..." >> output.txt
echo "" >> output.txt
echo "Output written to output.txt"
echo ""
bin/new_router -i ibm_mad_benchmark/ibm03_mad.dat >> output.txt
echo ""
echo "" >> output.txt
sh compile_devProg.sh >> output.txt
echo "execution complete..."
echo "" >> output.txt


echo "Running the program with file ibm04_mad.dat..."
echo "Running the program with file ibm04_mad.dat..." >> output.txt
echo "" >> output.txt
echo "Output written to output.txt"
echo ""
bin/new_router -i ibm_mad_benchmark/ibm04_mad.dat >> output.txt
echo "" >> output.txt
echo ""
sh compile_devProg.sh >> output.txt
echo "" >> output.txt
echo "execution complete..."


echo "Running the program with file ibm06_mad.dat..."
echo "Running the program with file ibm06_mad.dat..." >> output.txt
echo "" >> output.txt
echo "Output written to output.txt"
echo ""
bin/new_router -i ibm_mad_benchmark/ibm06_mad.dat >> output.txt
echo "" >> output.txt
echo ""
sh compile_devProg.sh >> output.txt
echo "execution complete..."


echo "Running the program with file ibm07_mad.dat..."
echo "Running the program with file ibm07_mad.dat..." >> output.txt
echo "" >> output.txt
echo "Output written to output.txt"
echo ""
bin/new_router -i ibm_mad_benchmark/ibm07_mad.dat >> output.txt
echo "" >> output.txt
echo ""
sh compile_devProg.sh >> output.txt
echo "execution complete..."

echo "Running the program with file ibm08_mad.dat..."
echo "Running the program with file ibm08_mad.dat..." >> output.txt
echo "" >> output.txt
echo "Output written to output.txt"
echo ""
bin/new_router -i ibm_mad_benchmark/ibm08_mad.dat >> output.txt
echo "" >> output.txt
echo ""
sh compile_devProg.sh >> output.txt
echo "execution complete..."

echo "Running the program with file ibm09_mad.dat..."
echo "Running the program with file ibm09_mad.dat..." >> output.txt
echo "" >> output.txt
echo "Output written to output.txt"
echo ""
bin/new_router -i ibm_mad_benchmark/ibm09_mad.dat >> output.txt
echo "" >> output.txt
echo ""
sh compile_devProg.sh >> output.txt
echo "execution complete..."

echo "Running the program with file ibm10_mad.dat..."
echo "Running the program with file ibm10_mad.dat..." >> output.txt
echo "" >> output.txt
echo "Output written to output.txt"
echo ""
bin/new_router -i ibm_mad_benchmark/ibm10_mad.dat >> output.txt
echo "" >> output.txt
echo ""
sh compile_devProg.sh >> output.txt
echo "execution complete..."

echo "All operations completed successfully..."
echo "Output written to output.txt"
