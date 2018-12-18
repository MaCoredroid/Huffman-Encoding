#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <map>
#include <vector>
#include <queue>
#include <string>
#include <bitset>
using namespace std;

struct node {
	char data;
	unsigned freq;
	node *left, *right;
	bool flag = 1;
	node(bool flag,char data, unsigned freq)
	{

		left = right = NULL;
		this->flag = flag;
		this->data = data;
		this->freq = freq;
	}
};
struct compare {

	bool operator()(node* l, node* r)

	{
		return (l->freq > r->freq);
	}
};

void outputcode(map<char, string>& huffmancode, struct node *top, string str)
{
	if (!top)
	{
		return;
	}
	if (top->flag != 0)
	{
		huffmancode[top->data] = str;
	}
	outputcode(huffmancode, top->left, str + "0");
	outputcode(huffmancode, top->right, str + "1");

		
}

void HuffmanCodes(map<char, int> freq, priority_queue<node*, vector<node*>, compare>& minHeap, map<char, string>& huffmancode)
{
	struct node *left, *right, *top;
	top= new node(1,0, 0);
	map<char, int>::iterator it;
	for (it = freq.begin(); it != freq.end(); ++it)
	{
		minHeap.push(new node(1,it->first, it->second));
	}
	while (minHeap.size() != 1)
	{
		left = minHeap.top();
		minHeap.pop();
		right = minHeap.top();
		minHeap.pop();
		top = new node(false,'$', left->freq + right->freq);
		top->left = left;
		top->right = right;
		minHeap.push(top);
	}
	outputcode(huffmancode, top, "");
	


}

void writefreq(ofstream& ofs, map<char, int> freq)
{
	char *temp;
	int numofleaf = freq.size();
	temp = (char*)&numofleaf;
	for (int i = 0; i < 4; ++i) 
	{
		ofs.put(temp[i]);
	}
	map<char, int>::iterator it;
	for (it = freq.begin(); it != freq.end(); ++it)
	{
		ofs.put(it->first);
		int freq = it->second;
		char *temp;
		temp = (char*)&freq;
		for (int i = 0; i < 4; ++i)
		{
			ofs.put(temp[i]);
		}
	}

}

void naiveCopy(string inputFilename, string outputFilename) {
  ofstream ofs(outputFilename.c_str(), ios::out | ios::binary);
  ifstream ifs(inputFilename.c_str(), ios::in | ios::binary);
  char c;
  while (ifs.get(c)) ofs.put(c);
  ofs.close();
  ifs.close();
}

void createfreq(map<char, int>& freq, ifstream& ifs)
{
	char c;
	while (ifs.get(c)) 
	{
		if (freq.count(c)) 
		{
			freq[c] += 1;
		}
		else 
		{
			freq[c] = 1;
		}
	}
	ifs.close();
}

void readfreq(map<char, int>& freq, ifstream& ifs)
{
	int numofleaf = 0;
	char *temp;
	temp = (char*)&numofleaf;
	for (int i = 0; i < 4; ++i) 
	{
		ifs.get(temp[i]);
	}
	for (int i = 0; i < numofleaf; i++)
	{
		char leaf;
		ifs.get(leaf);
		char *temp1;
		int fre = 0;
		temp1 = (char*)&fre;
		for (int j = 0; j<4; j++)
		{
			ifs.get(temp1[j]);
		}
		freq[leaf] = fre;
	}
	
}
void createcache(vector<char> &cache, map<char, int>& freq, ifstream& ifs, map<char, string> huffmancode)
{
	char c;
	while (ifs.get(c))
	{
		
			//std::cout << huffmancode[c] << endl;
			cache.insert(cache.end(), huffmancode[c].begin(), huffmancode[c].end());
		
	}
	

}

void collectbits(vector<char> &cache, ifstream& ifs)
{
	char tempbyte ;
	int cachesize = 0;
	char blank;
	char* temp;
	temp = (char*)&cachesize;
	for (int i = 0; i < 4; ++i)
	{
		ifs.get(temp[i]);
	}
	//std::cout << cachesize << endl;
	while (1)
	{
		if (ifs.get(tempbyte))
		{
			if (cachesize >= cache.size() + 8)
			{
				for (int i = 0; i < 8; i++)
				{
					if (int(((tempbyte >> i) & 1)) == 1)
					{
						cache.push_back('1');
					}
					else
					{
						cache.push_back('0');
					}


				}
				
			}
			else
			{
				int size = cache.size();
				for (int i = 0; i < cachesize-size; i++)
				{
					//std::cout << int(((tempbyte >> i) & 1))<<endl;
					if (int(((tempbyte >> i) & 1)) == 1)
					{
						cache.push_back('1');
					}
					else
					{
						cache.push_back('0');
						
					}
				}
				
				
			}
		}
		else
		{
			break;
		}
	}
}

void createbits(ofstream &ofs, vector<char> &cache)
{
	char tempbyte = 0;
	int bitcount = 0;
	int limit = 0;
	int cachesize = cache.size();
	char *temp;
	temp = (char*)&cachesize;
	for (int i = 0; i < 4; ++i) 
    {
		ofs.put(temp[i]);
	}
	//std::cout << cachesize << endl;
	for (int i = 0; i < cache.size(); i++)
	{
		char mask = 1;
		bool flag = true;
		if (flag)
		{
			bool bit = true;
			if (cache[i] == '1')
			{
				mask = mask << bitcount;
				tempbyte = tempbyte | mask;
			}
			bitcount++;

			if (bitcount == 8)
			{
				ofs.put(tempbyte);
				tempbyte = 0;
				bitcount = 0;
				if (i + 8 >= cachesize)
				{
					limit = i+1;
					break;
				}
			}
		}
	}
	if (limit == 0)
	{
		return;
	}
	tempbyte = 0;
	for (int i = limit; i < cachesize; i++)
	{
		char mask = 1;
		if (cache[i] == '1')
		{
			mask = mask << i - limit;
			tempbyte = tempbyte | mask;

		}
		if (i == cachesize - 1)
		{
			for (int j = cachesize; j < limit + 8; j++)
			{
				mask = 1;
				mask = mask << j - limit;
				tempbyte = tempbyte | mask;
			}
			ofs.put(tempbyte);
			break;
		}
	}



	
}

void writefile(ofstream& ofs, vector<char>& cache, map<char, string>& huffmancode)
{
	map<string, char> huffmancodere;
	map<char, string>::iterator it;
	for (it = huffmancode.begin(); it != huffmancode.end(); it++)
	{
		//std::cout << it->first << " " << it->second << endl;
		huffmancodere[it->second] = it->first;
	}
	int i = 0;
	string temp;
	
	while (i < cache.size())
	{
		
		//std::cout << temp << endl;
		temp = temp + cache[i];
		if (huffmancodere.count(temp)) 
		{
			ofs.put(huffmancodere[temp]);
			//std::cout << temp << endl;
			temp.clear();
		}
		
		i++;
	}
	
}


// TODO: implement your compression
void compress(string inputFilename, string outputFilename) {
	ifstream ifs(inputFilename.c_str(), ios::in | ios::binary);
	ofstream ofs(outputFilename.c_str(), ios::out | ios::binary);
	char c;
	if (ifs.eof())
	{
		return;
	}
	if (!ifs.get(c))
	{

		return;
	}
	ifs.putback(c);
	map<char, int> freq;
	createfreq(freq, ifs);
	vector<node> nodes;
	priority_queue<node*, vector<node*>, compare> minHeap;
	map<char, string> huffmancode;
	HuffmanCodes(freq, minHeap, huffmancode);
	writefreq(ofs, freq);
	vector<char> cache;
	ifs.open(inputFilename.c_str(), ios::in | ios::binary);
	//std::cout << huffmancode.size() << endl;
	createcache(cache,freq, ifs, huffmancode);
	createbits(ofs, cache);
	ofs.close();
}

// TODO: implement your decompression
void decompress(string inputFilename, string outputFilename) {
	ifstream ifs(inputFilename.c_str(), ios::in | ios::binary);
	ofstream ofs(outputFilename.c_str(), ios::out | ios::binary);
	char c;
	if (ifs.eof())
	{
		return;
	}
	if (!ifs.get(c))
	{

		return;
	}
	ifs.putback(c);
	map<char, int> freq;
	readfreq(freq, ifs);
	vector<node> nodes;
	priority_queue<node*, vector<node*>, compare> minHeap;
	map<char, string> huffmancode;
	HuffmanCodes(freq, minHeap, huffmancode);
	vector<char> cache;
	collectbits(cache, ifs);
	writefile(ofs, cache, huffmancode);
	ofs.close();

}

void usage(string prog) {
  cerr << "Usage: " << endl
      << "    " << prog << "[-d] input_file output_file" << endl;
  exit(2);
}

int main(int argc, char* argv[]) {
  int i;
  string inputFilename, outputFilename;
  bool isDecompress = false;
  for (i = 1; i < argc; i++) {
    if (argv[i] == string("-d")) isDecompress = true;
    else if (inputFilename == "") inputFilename = argv[i];
    else if (outputFilename == "") outputFilename = argv[i];
    else usage(argv[0]);
  }
  if (outputFilename == "") usage(argv[0]);
  if (isDecompress) decompress(inputFilename, outputFilename);
  else compress(inputFilename, outputFilename);
  return 0;
}

/*int main(int argc, char* argv[]) {
  int i;
  string inputFilename, outputFilename;
  inputFilename = "test.txt";
  outputFilename = "test1.mid";
  bool isDecompress = true;
  for (i = 1; i < argc; i++) {
	if (argv[i] == string("-d")) isDecompress = true;
	else if (inputFilename == "") inputFilename = argv[i];
	else if (outputFilename == "") outputFilename = argv[i];
	else usage(argv[0]);
  }
  if (outputFilename == "") usage(argv[0]);
  if (isDecompress) decompress(inputFilename, outputFilename);
  else compress(inputFilename, outputFilename);
  return 0;
}*/


