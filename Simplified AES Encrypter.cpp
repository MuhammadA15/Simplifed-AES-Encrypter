/*+--------------------------------------------------------------------------------+
  |  NAME: Muhammad Azam                                                           |
  |  DESCRIPTION: This program encrypts plaintext in a file using a simplified     |
  |  version of AES. The encrypted text is placed into a file of the user's        |
  |   choosing                                                                     |
  +--------------------------------------------------------------------------------+
*/
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <algorithm>
#include <cstdlib>
using namespace std;


//This function removes whitespaces and punctuations from the plaintext
string PreProcess(string line)
{
	int size, i;	
        line.erase(remove(line.begin(), line.end(), ' '), line.end()); //iterates through the string and removes any whitespaces it encounters
        size = line.size();
        for(i = 0;i < size;i++) //iterate through the string
        {
                if(ispunct(line[i])) //if a punctuation is found
                {
                        line.erase(i--, 1); //erase the character
                        size = line.size(); //adjust size of the string
                }
        }
	line[i] = '\0'; //set last character in the string as null for bounds checking
        return line; //return preprocessed plaintext
}
//This function uses vigenere's cipher to encrypt the plaintext based on a given key
string Substitution(string line, string key) //takes in  plaintext and a key
{
	int count,i,j;
	char encryptedtext[line.size()]; //used to move the vigenere cipher'ed string into a char array for easier manipulation

	count = line.size(); 
	for(i = 0; ;i++) //iterate
	{
		if(key.size() > count) //if key is larger then the plaintext, key does not need to be extended, break loop
		{
			break;
		}
		else if(key.size() == count) //if key is same size as plaintext, key does not need to be extended, break loop
		{
			break;
		}
		else
		{
			key.push_back(key[i]); //extend key by adding itself to itself
		}
	}
	key[i] = '\0'; //set null for bounds checking

	for(i = 0; i < line.size(); i++) //iterate through the plaintext
	{
		if(line[i] >= 65 && line[i] <= 90) //Checks for uppercase letters through their ASCII vals
		{
			encryptedtext[i] = (line[i] + key[i]) % 26; //vigenere's cipher (plaintext letter + key letter) mod % 26
			encryptedtext[i] += 'A'; //add A to get the corresponding uppercase letter
		}
		else
		{
			encryptedtext[i] = line[i]; 
		}
	}
	encryptedtext[i] = '\0'; //set null terminator
	return encryptedtext; //return string that has gone through vigenere cipher
}
//This function divides the text into blocks of 4x4 and adds padding if needed
string Padding(string encryptedtext, ofstream &outFile)
{
	int i, j, count = 0, blockcount = 0, rowcount = 0;
	char blocktext[200][200]; //2d array used to store rows and columns
	for(i=0;i<=16;i++) //iterate up to 16, because 15 characters is maximum amount of padding possible
	{
		if(encryptedtext.size() % 16 != 0) //if size of text is not divisible by 16, then padding is required 
		{
			encryptedtext.push_back('A'); //add A to the end of the text for padding
		}
		else
		{
			break; //break loop if padding no longer needed
		}
	}
	//nested for loop to iterate through columns and rows
	for(i=0;i<(encryptedtext.size()/4);i++) //first for loop runs through rows. Iterates for 4th of the text size because there are 4 characters per row
	{
		for(j=0;j<4;j++) //iterates through each character in a row (ie. columns) 
		{
			blocktext[i][j] = encryptedtext[count]; //store character into row i column j
			count++; //increments to next character in text
		}
		rowcount++; //counts rows 
	}

	//Send output to user-specified file, blockcount will count the rows and cut each block once four rows have been printed
	//blockcount reverts to 0 once a block is created
	//prints text that has gone through preprocessing, substitution, and padding
	outFile << "Padding:" << endl; 
	for(i=0;i<rowcount;i++)
	{
		for(j=0;j<4;j++)
		{
			outFile << blocktext[i][j];
		}
		outFile << endl;
		blockcount++;
		if(blockcount == 4)
		{
			outFile << endl;
			blockcount = 0;
		}
	}
	//print output to console
        cout << "Padding:" << endl;
        for(i=0;i<rowcount;i++)
        {
                for(j=0;j<4;j++)
                {
                        cout << blocktext[i][j];
                }
                cout << endl;
                blockcount++;
                if(blockcount == 4)
                {
                        cout << endl;
                        blockcount = 0;
                }
        }
	return encryptedtext; //return the padded text
}
//This function handles multiplication when mixing columns
unsigned char rgfMul(unsigned char letter, int num)
{
	unsigned char newletter;
	if(num == 2) //when multiplying by 2
	{
		newletter = letter << 1; //shift bits left by 1
	}
	else if(num == 3) //when multiplying by 3
	{
		newletter = letter << 1; //shift bits left by 1
		newletter = (newletter ^ letter); //XOR shifted letter with unshifted letter	
	}
	if(((letter >> 7) & 1) == 1) //if MSB is a 1
	{
		newletter = newletter ^ 27; //XOR new letter with 27(00011011)
	}
	return newletter; //return the new letter
}
//This function handles Row Shifting, Parity, and Column Mixing
void Shift_Parity_Mix(string encryptedtext, ofstream &outFile)
{
	unsigned char blocktext[200][200]; //must be unsigned or output will be off
	char hold[5]; //holds values when row shifting
	int i,j,count = 0,rowcount = 0, blockcount = 0, currRow = 0, Printblockcount = 0;
	
	//Load text into 2d char array so the characters can be manipulated as blocks
	for(i=0;i<(encryptedtext.size()/4);i++)
        {
                for(j=0;j<4;j++)
                {
                        blocktext[i][j] = encryptedtext[count];
                        count++;
                }
                rowcount++;
        }

	//**************Row Shifting**************

	for(i=0;i<rowcount;i++) //iterate through each row
	{
		if(blockcount == 1) //count starts at one because first row ([0]) has no shift
		{		    //2nd row of a block will shift to the left by 1	

			for(j=0;j<4;j++) //iterate through each letter in a row
			{
				if(j==0) //character [0] will move to [3]
				{
					//store [0] and shift remaining letters over by 1 place to the left
					hold[1] = blocktext[i][j]; 
					blocktext[i][j] = blocktext[i][j+1];
				}
				else
				{
					blocktext[i][j] = blocktext[i][j+1];
				}
			}
                        blocktext[i][3] = hold[1]; //store [0] into [3] 
		}
		if(blockcount == 2) //3rd row of a block will shift to the left by 2
		{
	                for(j=0;j<2;j++) //only two letters will shift, the other two will be stored and reassigned
                        {
                                if(j==0) //[0] will move to [2]
                                {
                                        hold[1] = blocktext[i][j];
                                        blocktext[i][j] = blocktext[i][j+2]; //move [2] to [0] 
                                }
				else if(j==1) //[1] will move to [3]
				{
					hold[2] = blocktext[i][j];
					blocktext[i][j] = blocktext[i][j+2]; //move [3] to [1]
				}
                        }
			blocktext[i][2] = hold[1]; //place [0] in [2]
			blocktext[i][3] = hold[2]; //place [1] in [3]
		}
                if(blockcount == 3) //4th row of a block will shift left by 3
                {
                        for(j=0;j<4;j++) 
                        {
                                if(j==0) //[0] will move to [1]
                                {
                                        hold[1] = blocktext[i][j];
                                        blocktext[i][j] = blocktext[i][j+3]; //only one letter needs to be shifted, the rest will be stored and reassigned
                                }
                                else if(j==1) //[1] will move to [2]
                                {
                                        hold[2] = blocktext[i][j];
                                }
				else if(j==2) //[2] will move to [3]
				{
					hold[3] = blocktext[i][j];
				}
                        }
			//reassign
                        blocktext[i][1] = hold[1];
                        blocktext[i][2] = hold[2];
			blocktext[i][3] = hold[3];
                }
		//tracks which row of a block the loop is on
		//resets to 0 when a new block begins
		blockcount++; 
		if(blockcount == 4)
		{
			blockcount = 0;
		}
	}
	//print text that has gone through preprocessing, substitution, padding, and row shifting to output file
	outFile << "ShiftRows:" << endl;
        for(i=0;i<rowcount;i++)
        {
                for(j=0;j<4;j++)
                {
                        outFile << blocktext[i][j];
                }
                outFile << endl;
                Printblockcount++;
                if(Printblockcount == 4)
                {
                        outFile << endl;
                        Printblockcount = 0;
                }
        }
	//print output to console
        cout << "ShiftRows:" << endl;
        for(i=0;i<rowcount;i++)
        {
                for(j=0;j<4;j++)
                {
                        cout << blocktext[i][j];
                }
                cout << endl;
                Printblockcount++;
                if(Printblockcount == 4)
                {
                        cout << endl;
                        Printblockcount = 0;
                }
        }

	//*************Parity**************

	int a, bitcount = 0;
	for(i=0;i<rowcount;i++) //iterate through rows
	{
		for(j=0;j<4;j++) //iterate through letters in row
		{
			bitcount = 0; //reset bit count
			for(a=0;a<8;a++) //iterate through bits in the letter
			{
				int check = (blocktext[i][j] >> a) & 1; //count amount of 1 bits
				if(check == 1)
				{
					bitcount++;
				}
			}
			if(bitcount % 2 == 1) //if amount of 1s is an odd number
			{
				blocktext[i][j] = blocktext[i][j] ^ 10000000; //flip the MSB
			}
		}
	}
	blockcount = 0;

	//print text that has gone through preprocess, substituion, padding, row shifts, and bit parity to output file (in hex)
	outFile << "Parity bit:" << endl;
        for(i=0;i<rowcount;i++)
        {
                for(j=0;j<4;j++)
                {
			outFile << hex << int(blocktext[i][j]) << " ";
                }
                outFile << endl;
        }
	//print output to console
        cout << "Parity bit:" << endl;
        for(i=0;i<rowcount;i++)
        {
                for(j=0;j<4;j++)
                {
                        cout << hex << int(blocktext[i][j]) << " ";
                }
                cout << endl;
        }

	//**************Column Mixing*****************	

	unsigned char Mixedblocktext[200][200]; //carries the rows and new mixed columns
        for(i=0;i<rowcount;i=i+4) //iterate through each block
        {
                for(j=0;j<4;j++) //iterate through each row
                {
			//every column is multiplied according to Rijndael's Galois Field
			//multiplication handling is done with the rgfMul function
			//every j handles a single column in a single block
                        Mixedblocktext[i][j] = rgfMul(blocktext[i][j],2) ^ rgfMul(blocktext[i+1][j],3) ^ blocktext[i+2][j] ^ blocktext[i+3][j];
			Mixedblocktext[i+1][j] = blocktext[i][j] ^ rgfMul(blocktext[i+1][j],2) ^ rgfMul(blocktext[i+2][j],3) ^ blocktext[i+3][j];
 			Mixedblocktext[i+2][j] = blocktext[i][j] ^ blocktext[i+1][j] ^ rgfMul(blocktext[i+2][j],2) ^ rgfMul(blocktext[i+3][j],3);
 			Mixedblocktext[i+3][j] = rgfMul(blocktext[i][j],3) ^ blocktext[i+1][j] ^ blocktext[i+2][j] ^ rgfMul(blocktext[i+3][j],2);
                }
        }

	//print text that has gone through preprocess, substituion, padding, row shifts, bit parity, and column mixing to output file
	outFile << endl << "MixedColumns:" << endl;	
        for(i=0;i<rowcount;i++)
        {
                for(j=0;j<4;j++)
                {
                        outFile << hex << int(Mixedblocktext[i][j]) << " ";
                }
                outFile << endl;
        }
	//print output to console
        cout << endl << "MixedColumns:" << endl;
        for(i=0;i<rowcount;i++)
        {
                for(j=0;j<4;j++)
                {
                        cout << hex << int(Mixedblocktext[i][j]) << " ";
                }
                cout << endl;
        }
}
int main()
{
	int i, count; //iterators
	string line; //contains full length string from input file
	string key;  //contains key
	string str;  //contains one line of string from input file
	string encryptedtext; //contains the new string after each encryption step
	char filename[50], keyfile[50], ofile[50], blocktext[200][200];

	//open plaintext file
	cout << "Enter name of file that will be encrypted: ";
	cin >> filename;
	
	ifstream inFile;
	inFile.open(filename);
	if(inFile.fail())
	{
		cout << "ERROR: Plaintext file could not be opened. terminating...." << endl;
		return 1;
	}
	//open key file
	cout << "Enter name of file containing encryption key: ";
	cin >> keyfile;
	
	ifstream kFile;
	kFile.open(keyfile);
	if(kFile.fail())
	{
		cout << "ERROR: Key file could not be opened. terminating...." << endl;
		return 1;
	}
	//open/create output file
	cout << "Enter name of file that will store the encrypted text: ";
	cin >> ofile;	
	ofstream outFile;
	outFile.open(ofile);
       	
	//get entire string from input file
        while(getline(inFile,str))
	{
		line = line + str;
	}
	//get key from key file 
	getline(kFile,key);

	line = PreProcess(line); //execute preprocess on plaintext
	outFile << "Preprocessing:" << endl << line << endl << endl; //print preprocessed text to output file
        cout << endl <<"Preprocessing:" << endl << line << endl << endl;	

	encryptedtext = Substitution(line,key); //execute vigenere's cipher, store result into string
	outFile << "Substitution:" << endl << encryptedtext << endl << endl; //print text that has gone through preprocessing and substitution to output file
        cout << "Substitution:" << endl << encryptedtext << endl << endl;

	encryptedtext = Padding(encryptedtext,outFile); //execute padding, store result into string
	Shift_Parity_Mix(encryptedtext,outFile); //execute row shift, bit parity, and column mixing

	cout << endl << "Encryption Complete" << endl << "The encryption steps have been stored in the file '" << ofile << "'" << endl;
	return 0;
}
	
