#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct cacheLine{
	
	int validBit;
	char* line;
	int setNumber;
	int recentlyUsed;
	int dirtyBit;
	int tagLength;
	int fifoIndex;
	//block offset bits?
} cacheLine;

typedef struct fifoStuff{
	int setFifoIndex;
	int least;
}fifoStuff;
//make an array, each index is a set number and it contains that sets setFifoIndex and least

fifoStuff *fifoArray;
int numberOfLines = 0;
int blah = 0;
int powerOfTwo();
int cacheSize = 0;
int blockSize = 0;
char* assoc;
int setSize=0; //assoc
int numberOfSets;
char* replacementPolicy;
char* writingPolicy = "   ";
int cacheHits = 0;
int memoryReads = 0;
int cacheMisses = 0;
int memoryWrites = 0;
int blockOffset = 0;
int setOffset = 0;

cacheLine* cacheMoney;

cacheLine* newCache();
void set_subs();
int getBlockOffset();
int getSetOffset();
char* hexToBinary();
int getTagLength();
int getSetNum();



cacheLine* newCache()
{
	
	int i;
	cacheLine *cacheMoney;
	cacheMoney = malloc(sizeof(cacheLine)*numberOfLines);
	i = 0;
	
	for(i=0; i<numberOfLines; i++)
	{
		cacheMoney[i].fifoIndex = 0;
		cacheMoney[i].validBit = 0;
		cacheMoney[i].line = malloc(sizeof(char)*81);
		cacheMoney[i].setNumber = i / setSize;
		cacheMoney[i].recentlyUsed = 0;
		cacheMoney[i].dirtyBit = 0;
		cacheMoney[i].tagLength=1;
	}
	return cacheMoney;
	
}

/*char* switching(char *binaryNumber)
{
	char result[strlen(binaryNumber)];
	char tempTag[getTagLength(binaryNumber)+1];
	char tempSet[setOffset+1];
	
	strncpy(tempTag, binaryNumber, getTagLength(binaryNumber));
	tempTag[getTagLength(binaryNumber)]='\0';
	
	strncpy(tempSet, binaryNumber+ getTagLength(binaryNumber), setOffset);
	tempSet[setOffset]='\0';
	
	strcat(result-1, tempSet);
	strcat(result, tempTag);
	strncat(result, binaryNumber+strlen(tempSet)+strlen(tempTag), blockOffset);
	return result-1;
}
*/

int powerOfTwo(int n){
	while((n % 2 == 0) && n > 1){
		n /= 2;
	}
    return (n == 1);
}

void set_subs()
{
	int i = 1;
	for(i;i<blockSize; i=i*2)
	{
		blockOffset++;
	}
	if(i!=blockSize &&blockOffset!=0)
	{
		printf("error\n");
		exit(1);
	}
	i=1;
	for (i; i<numberOfLines/setSize; i=i*2)
	{
		setOffset++;
	}
	if(i!= numberOfLines/setSize && setOffset!=0)
	{
		printf("error\n");
		exit(1);
	}
	
}
int getTagLength(char *address)
{
	return strlen(address) - setOffset - blockOffset;
}

char* hexToBinary(char* hexNum)
{
	int i = 0;
	char *binNum = (char *)malloc(sizeof(char)*81);
	for (i=2; i<strlen(hexNum); i++)
	{
		switch(hexNum[i]) {
			case '0': strcat(binNum,"0000"); break;
            case '1': strcat(binNum,"0001"); break;
            case '2': strcat(binNum,"0010"); break;
            case '3': strcat(binNum,"0011"); break;
            case '4': strcat(binNum,"0100"); break;
            case '5': strcat(binNum,"0101"); break;
            case '6': strcat(binNum,"0110"); break;
            case '7': strcat(binNum,"0111"); break;
            case '8': strcat(binNum,"1000"); break;
            case '9': strcat(binNum,"1001"); break;
            case 'a': strcat(binNum,"1010"); break;
            case 'b': strcat(binNum,"1011"); break;
            case 'c': strcat(binNum,"1100"); break;
            case 'd': strcat(binNum,"1101"); break;
         	case 'e': strcat(binNum,"1110"); break;
            case 'f': strcat(binNum,"1111"); break;
        }
    }
	return binNum;
}

int getSetNum(char *address, int tagLength)
{
	int setNum = 0;
	int temp = 1; //2^0
	int i = strlen(address) - 1 - blockOffset;
	for(i; i>=tagLength; i--)
	{
		if(address[i]=='1')
		{
			setNum += temp;
		}
		temp = temp<<1; //2^x power
	}
	return setNum;
}

void updateLRU(int new, int index)
{
	int i = index * setSize;	
	for (i; i < (index * setSize) + setSize; i++)
	{
		cacheMoney[i].recentlyUsed++;
	}
	cacheMoney[new].recentlyUsed=0;
}

void readFromCache(char* address, char* policy)
{
	int index = getSetNum(address, getTagLength(address)) * setSize;
    int highestIndex = index;
    int highest = 0;
    int i = index;
    for(i; i < index + setSize; i++){
        if(strncmp(cacheMoney[i].line, address, cacheMoney[i].tagLength)==0){
			if(cacheMoney[i].validBit == 1){
				//issue is here againnnn
                cacheHits++;
				
				if(strcmp(replacementPolicy, "LRU")==0)
					updateLRU(i, cacheMoney[i].setNumber);
				else{
					cacheMoney[i].fifoIndex = fifoArray[cacheMoney[i].setNumber].setFifoIndex;
					fifoArray[cacheMoney[i].setNumber].setFifoIndex++;
				}
				
                return;
            }

        }
    }
    memoryReads++;
    cacheMisses++;
	
    index = getSetNum(address, getTagLength(address)) * setSize;
    i = index;
    for(i; i < index + setSize; i++){
        if(cacheMoney[i].validBit == 0)
		{ //empty spot in cache
            cacheMoney[i].validBit = 1;
			if(strcmp(policy, "wb")==0)
			{
				cacheMoney[i].dirtyBit = 0; //this is the only thing
			}
            cacheMoney[i].tagLength = getTagLength(address);
            strcpy(cacheMoney[i].line, address);
			if(strcmp(replacementPolicy, "LRU")==0)
				updateLRU(i, cacheMoney[i].setNumber);
			else{
				cacheMoney[i].fifoIndex = fifoArray[cacheMoney[i].setNumber].setFifoIndex;
				fifoArray[cacheMoney[i].setNumber].setFifoIndex++;
			}
            return;
		}
    }
    index = getSetNum(address, getTagLength(address)) * setSize;
    i = index;
    for(i; i < index + setSize; i++){
		
        if (strcmp(replacementPolicy, "LRU")==0)
		{
			if(cacheMoney[i].recentlyUsed > highest)
			{
			highest = cacheMoney[i].recentlyUsed;
			highestIndex = i;
			}
		}			
		else if(cacheMoney[i].fifoIndex==fifoArray[cacheMoney[i].setNumber].least){
			highest = cacheMoney[i].fifoIndex;
			highestIndex = i;
			//lowest or first in this case but why use another variable?
		}
    }
    if(strcmp(policy, "wb")==0 && cacheMoney[highestIndex].dirtyBit== 1){
        memoryWrites++;
    }
    cacheMoney[highestIndex].tagLength = getTagLength(address);
    strcpy(cacheMoney[highestIndex].line, address);
	if(strcmp(policy, "wb")==0)
	{
		cacheMoney[highestIndex].dirtyBit = 0; //same here
	}
	if(strcmp(replacementPolicy, "LRU")==0)
		updateLRU(highestIndex, cacheMoney[highestIndex].setNumber);
	else
	{
		cacheMoney[highestIndex].fifoIndex = fifoArray[cacheMoney[highestIndex].setNumber].setFifoIndex;
		fifoArray[cacheMoney[highestIndex].setNumber].setFifoIndex++;
		fifoArray[cacheMoney[highestIndex].setNumber].least++;
	}
	return;
}


void writeToCache(char* address, char* policy)
{
	
	int highest = 0;
	int index = getSetNum(address, getTagLength(address)) * setSize;
	int i = index;
	int highestIndex = index;
	if(strcmp(policy, "wt")==0)
	{
		memoryWrites++;
	}
	for(i; i< index + setSize; i++)
	{
		if(strncmp(cacheMoney[i].line, address, cacheMoney[i].tagLength)==0)
		{
			//printf("blah %d\n", blah++);
			if(cacheMoney[i].validBit == 1)
			{
				cacheHits++;
				if(strcmp(policy, "wt")==0)
				{
					//SOMETHING IS WRONT WITH THE NEXT STATEMENT, SEGFAULT WHENEVER
					//I TRY IT, WHEREVER
					strcpy(cacheMoney[i].line, address);
				}
				cacheMoney[i].tagLength = getTagLength(address);
				if(strcmp(policy, "wb")==0)
				{
					cacheMoney[i].dirtyBit = 1;
				}
				
				if(strcmp(replacementPolicy, "LRU")==0)
					updateLRU(i, cacheMoney[i].setNumber);
				else{
					cacheMoney[i].fifoIndex = fifoArray[cacheMoney[i].setNumber].setFifoIndex;
					fifoArray[cacheMoney[i].setNumber].setFifoIndex++;	
				}
				return;
			}
		}
	}	
	cacheMisses++;
	memoryReads++;
	index = getSetNum(address, getTagLength(address)) * setSize;
	i = index;
	for(i; i < index + setSize; i++)
	{
		if(cacheMoney[i].validBit == 0)
		{
			cacheMoney[i].validBit = 1;
			cacheMoney[i].tagLength = getTagLength(address);
			strcpy(cacheMoney[i].line, address);
			if(strcmp(policy, "wb")==0)
			{
				cacheMoney[i].dirtyBit = 1;
			}
			if(strcmp(replacementPolicy, "LRU")==0)
				updateLRU(i, cacheMoney[i].setNumber);
			else
			{
				cacheMoney[i].fifoIndex = fifoArray[cacheMoney[i].setNumber].setFifoIndex;
				fifoArray[cacheMoney[i].setNumber].setFifoIndex++;
			}
			
			return;
		}
	}
	index = getSetNum(address, getTagLength(address)) * setSize;
	i = index;
	for(i; i < index + setSize; i++)
	{
		
		if (strcmp(replacementPolicy, "LRU")==0)
		{
			if(cacheMoney[i].recentlyUsed > highest)
			{
			highest = cacheMoney[i].recentlyUsed;
			highestIndex = i;
			}
		}
		else if(cacheMoney[i].fifoIndex==fifoArray[cacheMoney[i].setNumber].least){
			highest = cacheMoney[i].fifoIndex;
			highestIndex = i;
			//lowest or first in this case but why use another variable?
		}
		
	}
	if(strcmp(policy, "wb")==0 && cacheMoney[highestIndex].dirtyBit == 1){
		
        memoryWrites++;
    }
	
	cacheMoney[highestIndex].tagLength = getTagLength(address);
	strcpy(cacheMoney[highestIndex].line, address);
	if(strcmp(policy, "wb")==0)
	{
		cacheMoney[highestIndex].dirtyBit = 1;
	}
	if(strcmp(replacementPolicy, "FIFO")==0)
	{
		cacheMoney[highestIndex].fifoIndex = fifoArray[cacheMoney[highestIndex].setNumber].setFifoIndex;
		
		fifoArray[cacheMoney[highestIndex].setNumber].setFifoIndex++;
		fifoArray[cacheMoney[highestIndex].setNumber].least++;
	}
	
	else
		updateLRU(highestIndex, cacheMoney[highestIndex].setNumber);
	
	
}

//REMEMBER THAT THE BLOCK NUMBER DOESNT MATTER

int main (int argc, char **argv){
	char firstAdd[20];
	char wr[2];
	char addr[20];
	char *binaryNumber;
	if (argc != 7){
		printf("error, number or arguments is too big or too small\n");
		return 1;
	}
	
	char buff[100];
	
	FILE* file = fopen(argv[6], "r");

	if(file == NULL){
		printf("error with file\n");
		return 1;
	}
	
	cacheSize = atoi(argv[1]);
	if(!powerOfTwo(cacheSize)){
		printf("error: cacheSize size must be power of 2\n");
		return 1;
	}
	
	blockSize = atoi(argv[3]);
	if(!powerOfTwo(blockSize)){
		printf("error: blockSize size must be power of 2\n");
		return 1;
	}
	
	assoc = argv[2];
	if(strcmp(assoc, "direct") == 0){
		setSize = 1;
	}
	else if(strcmp(assoc, "assoc")==0){
		setSize = cacheSize/blockSize;
	}
	else if(powerOfTwo((assoc[strlen(assoc)-1] - '0'))){
		setSize = assoc[strlen(assoc)-1] - '0';
	}
	else{
		printf("error: associativity\n");
		return 1;
	}
	
	writingPolicy = argv[5];
	if(strcmp(writingPolicy, "wt") != 0 && strcmp(writingPolicy, "wb") != 0){
		printf("error: writingPolicy\n");
		return 1;
	}
	numberOfLines = cacheSize / blockSize;
	numberOfSets = cacheSize/(setSize*blockSize);
	if(cacheSize != numberOfSets * setSize * blockSize){
		printf("error: cacheSize\n");
		return 1;
	}
	
	replacementPolicy = argv[4];
	if(strcmp(replacementPolicy, "LRU") != 0 && strcmp(replacementPolicy, "FIFO") != 0){
		printf("error: replacement policy\n");
		return 1;
	}
	
	cacheMoney = newCache();
	set_subs();
	fifoArray = malloc(sizeof(fifoStuff)*numberOfSets);
	int j = 0;
	//initialize fifo array for each set
	for (j;j<numberOfSets; j++)
	{
		fifoArray[j].setFifoIndex = 0;
		fifoArray[j].least = 0;
	}
	//printf("cacheSize: %d\nassociativity: %s\nset size: %d\n#sets: %d\nblockSize size: %d\nreplacement policy: %s\nwriting policy: %s\n", cacheSize, assoc, setSize, numberOfSets, blockSize, replacementPolicy, writingPolicy);		
	while (fscanf(file, "%s %s %s", firstAdd, wr, addr) != EOF) 
	{
		if(strcmp(firstAdd, "#eof")==0)
		{
			break;
		}
		
		//THIS IS FOR THE REPORT
		binaryNumber = hexToBinary(addr);
		//strcpy(binaryNumber, switching(binaryNumber));
		if(strcmp(writingPolicy, "wt")==0)
		{
			
			if(strcmp(wr, "W")==0)
			{
				writeToCache(binaryNumber, "wt");
			}
			else if(strcmp(wr, "R")==0)
			{
				readFromCache(binaryNumber, "wt");
			}
		}
		else //writing policy is wb
		{
			if (strcmp(wr, "W")==0)
			{
				
				writeToCache(binaryNumber, "wb");
			}
			else if(strcmp(wr, "R")==0)
			{
				readFromCache(binaryNumber, "wb");
			}
		}
	}
	
	printf("Memory reads: %d\n", memoryReads);
	printf("Memory writes: %d\n", memoryWrites);
	printf("Cache hits: %d\n", cacheHits);
    printf("Cache misses: %d\n", cacheMisses);
	for(j=0; j<numberOfLines; j++)
	{
		free(cacheMoney[j].line);
	}
	fclose(file);
	return 0;
}

