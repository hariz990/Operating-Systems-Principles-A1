#include "Header.h"

//Lists
std::list<Chunk> releasedChunks;
std::list<Chunk> allocatedChunks;

//Variables
typedef int AllocType;
AllocType choice;
int increment;
int length = 800;

//Functions
std::string getString();
void* alloc(int chunk_size);
void dealloc(void* chunk);
void newChunk(size_t chunk_size, intptr_t memory_address, bool isFree);
void getTotalChunkAverage();
void printResults();

int main(int argc, char** argv)
{
	//Check passed argument
	if (argc == 2){
		std::string arg = argv[1];
		if (arg == "firstfit"){
			choice = FIRST_FIT;
		}
		else if (arg == "bestfit"){
			choice = BEST_FIT;
		}
		else if (arg == "worstfit"){
			choice = WORST_FIT;
		}
		else{
			std::cout << "Choose firstfit/bestfit/worstfit EXAMPLE: ./file firstfit" << std::endl;
			return EXIT_FAILURE;
		}
	}
	else{
		std::cout << "Choose firstfit/bestfit/worstfit EXAMPLE: ./file firstfit" << std::endl;
		return EXIT_FAILURE;
	}

	std::cout << "Choice: " << argv[1] << std::endl;
	std::list<std::string*> myData;                   //List of strings to hold data
	void* b = sbrk(0);                                  //Current program edge
	int* p = (int*)b;                                  //Cast edge as int for address
	std::cout << "Old Program Edge: " << p << std::endl;  //Print old program edge


	std::string str; //str to hold data in the text file

	//Read text file
	for (int i = 0; i < length; i++){
		str = getString();
		std::string* data1 = (std::string*)alloc(str.size() * sizeof(std::string::value_type) + 1); //Get the data from the text file and allocate memory
		*data1 = str;                                                                                //Set the value of the data variable
		myData.push_front(data1);                                                                   //Add the data to the myData list
	}

	//Deallocates all data in myData
	std::cout << "Deallocating" << std::endl;
	std::list<std::string*>::iterator it;
	for (it = myData.begin(); it != myData.end(); it++){
		dealloc((void*)* it);
	}

	myData.clear(); //Empties the list 

	printResults(); //Prints info

	//Reallocates the data
	std::cout << "Reallocating data..." << std::endl;
	for (int i = 0; i < length; i++){
		str = getString();
		std::string* data2 = (std::string*)alloc(str.size() * sizeof(std::string::value_type) + 1); //Get data from file and allocate memory
		*data2 = str;                                                                                //Set data value 
		myData.push_front(data2);                                                                   //Add to myData list
	}

	std::cout << "Reallocate completed" << std::endl;

	printResults(); //Print out chunk info

	b = sbrk(0);  //Get current program edge
	p = (int*)b; //Cast edge as int to see address

	std::cout << "New Program Edge: " << p << std::endl; //Print out new program edge

	getTotalChunkAverage();
	return 0;
}

//For experiment purposes
std::string getString()
{
	srand(time(0) + increment);

	int num = 0 + (std::rand() % (12000 - 0 + 1));
	std::ifstream in("experiment1.txt"); //PLACE EXPERIMENT FILE HERE
	std::string data; //String to hold data from file

	//Read text file lines
	while (std::getline(in, data) && num >= 0){
		num--;
	}

	std::cout << data << std::endl;
	increment++;
	return data;
}

//ALLOC FUNCTION
void* alloc(int chunk_size)
{
	void* currentAddress = sbrk(0); //Gets the current address of the edge
	void* returnAddress = nullptr; 

	int chunkRemain = 0; //Chunk Remainder

	if (releasedChunks.size() > 0){
		Chunk chunkSelect = { 0, 0 };
		int dif = -1;
		std::list<Chunk>::iterator it;
		for (it = releasedChunks.begin(); it != releasedChunks.end(); it++){
			//First fit algorithm
			if (choice == FIRST_FIT){
				if (it->chunk_size >= (size_t)chunk_size){
					std::cout << it->chunk_size - chunk_size << std::endl;
					chunkRemain = it->chunk_size - chunk_size;
					returnAddress = (void*)it->memory_address;
					chunkSelect.memory_address = it->memory_address;
					chunkSelect.chunk_size = it->chunk_size;
					break;
				}
			}
			//Best fit algorithm
			else if (choice == BEST_FIT){
				int chunkDif = it->chunk_size - chunk_size;
				if (chunkDif > 0 && (chunkDif < dif || dif == -1)){
					dif = chunkDif;
					returnAddress = (void*)it->memory_address;
					chunkRemain = dif;
					chunkSelect.memory_address = it->memory_address;
					chunkSelect.chunk_size = it->chunk_size;
				}
			}
			//Worst fit algorithm
			else if (choice == WORST_FIT) {
				int chunkDif = it->chunk_size - chunk_size;
				if (chunkDif > 0 && (chunkDif > dif || dif == -1)) {
					dif = chunkDif;
					returnAddress = (void*)it->memory_address;
					chunkRemain = dif;
					chunkSelect.memory_address = it->memory_address;
					chunkSelect.chunk_size = it->chunk_size;
				}
			}
		}

		if (chunkSelect.chunk_size != 0){
			std::cout << "Size of chunk needed: " << chunk_size << " Chunk size is: " << chunkSelect.chunk_size << std::endl;
			returnAddress = (void*)chunkSelect.memory_address;
			releasedChunks.remove(chunkSelect);
			allocatedChunks.push_front(chunkSelect);
		}

		if (chunkRemain > 0){
			std::cout << "Splitting chunk to a new one..." << std::endl;
			newChunk(chunkRemain, (intptr_t)((intptr_t)chunkSelect.memory_address + (intptr_t)chunkRemain), true);
		}
	}

	if (returnAddress == nullptr){
		sbrk(chunk_size + 1);
		newChunk((size_t)chunk_size, (intptr_t)currentAddress, false);
		returnAddress = (void*)((intptr_t)currentAddress);
	}
	return returnAddress;
}

//DEALLOC FUNCTION
void dealloc(void* chunk)
{
	Chunk removeChunk = { 0, 0 };
	std::list<Chunk>::iterator it;
	//Iterates through chunks to check for memory address
	for (it = allocatedChunks.begin(); it != allocatedChunks.end(); it++){
		if (it->memory_address == (intptr_t)chunk){
			removeChunk = *it;
		}
	}

	//For any chunk address that is found, chunk is transfered from allocated to available ones
	if (removeChunk.chunk_size != (size_t)0 && removeChunk.memory_address != (intptr_t)0){
		releasedChunks.push_front(removeChunk);
		allocatedChunks.remove(removeChunk);
	}
}

void newChunk(size_t chunk_size, intptr_t memory_address, bool isFree) //Function to allocate new chunk
{
	sbrk(sizeof(Chunk));
	Chunk chunk;
	chunk.chunk_size = (size_t)chunk_size;
	chunk.memory_address = (intptr_t)memory_address;

	if (isFree) {
		releasedChunks.push_front(chunk);
	}
	else {
		allocatedChunks.push_front(chunk);
	}
}

void getTotalChunkAverage() //Calculation for statistics of both lists
{
	size_t allocatedSize = 0;
	size_t emptySize = 0;

	std::list<Chunk>::iterator it;
	if (releasedChunks.size() > 0){
		for (it = releasedChunks.begin(); it != releasedChunks.end(); it++){
			emptySize += it->chunk_size;
		}
	}

	if (allocatedChunks.size() > 0){
		for (it = allocatedChunks.begin(); it != allocatedChunks.end(); it++){
			allocatedSize += it->chunk_size;
		}
	}

	std::cout << "Allocated bytes: " << allocatedSize << " Free Bytes: " << emptySize << std::endl;
}

void printResults()
{
	std::list<Chunk>::iterator it;
	std::cout << "Allocated Chunks" << std::endl;
	for (it = allocatedChunks.begin(); it != allocatedChunks.end(); it++){
		it->print();
	}

	std::cout << "Released Chunks" << std::endl;
	for (it = releasedChunks.begin(); it != releasedChunks.end(); it++){
		it->print();
	}
}
