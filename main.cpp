#include <algorithm>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
std::vector<std::string> split_sentence(std::string sen){
	//create string stream object
	std::stringstream ss(sen);
	//variable to hold each word
	std::string word;
	//vector to store words
	std::vector<std::string> words;
	//extract words of the sentense
	while(ss >> word){
		words.push_back(word);
	}
	return words;
}

uint64_t my_hash(std::string s) {
	uint64_t hash = 0;
	for (int i = 0; i< s.length();i++){
    		hash ^= ((uint64_t)s[i]) << ((i % 8) * 8);
	}
    return hash;
}
std::string dec_to_bin(uint64_t n){
	if (n == 0) return "0";
	std::string bin= ""; 
	while(n>0){
		int bit = n%2;
		bin.push_back(bit+'0');
		n /=2;
	}
	std::reverse(bin.begin(),bin.end());
		return bin;
}
int counting_trailing_zero(std::string bin){
return 0;
}
int main(){
	std::string text,line;
	std::vector<std::string> words;
	//reading the file
	std::fstream file;
	file.open("t8.shakespeare.txt",std::ios::in);
	std::cout<<"opend file\n";
	if(file.is_open()){
		while(std::getline(file,line)){
			text+=line+" ";
		}
		file.close();
	}
	// Tokenize words
    	words = split_sentence(text);
	

	for (int i = 0; i < 100 && i < words.size(); i++) {
	    std::string w = words[i];
	    uint64_t hash_val = my_hash(w);  // simulate 100 bucket array
	    std::cout << w << " " << hash_val << std::endl;
	}
	return 0;
}
