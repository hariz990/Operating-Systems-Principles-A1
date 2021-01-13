#HOW TO RUN THE PROGRAM#

Step 1
Copy and paste the following files into a folder (experiment1.txt, Header.h, OSP-Assignment1.cpp)

Step 2 
cd into the directory where the folder is 

Step 3. Run this command to compile code: 
g++ -Wall -pedantic -std=c++17 OSP-Assignment1.cpp -o file -g

OR (If the first one does not work for RMIT Jupiter server)

g++ -Wall -pedantic -std=c++0x OSP-Assignment1.cpp -o file -g 

Step 4. Run this command to run code 
Option 1: 
./file firstfit

Option 2: 
./file bestfit

Option 3: 
./file worstfit


#EXTRA NOTES#

experiment1.txt contains a large value of names to test the program, to add a new experiment file open OSP-Assignment1.cpp file, 
go to line 101 and replace another file to experiment with in between quotation.

Line 101 std::ifstream in("experiment1.txt");