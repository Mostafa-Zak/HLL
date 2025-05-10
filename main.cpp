#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <unordered_set>

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

// MurmurHash3 for 64-bit - faster and better statistical properties than custom hash
uint64_t murmur_hash(const std::string& key, uint64_t seed = 0) {
    const uint64_t m = 0xc6a4a7935bd1e995ULL;
    const int r = 47;
    
    uint64_t h = seed ^ (key.length() * m);
    
    const uint64_t* data = (const uint64_t*)key.data();
    const uint64_t* end = data + (key.length() / 8);
    
    while (data != end) {
        uint64_t k = *data++;
        
        k *= m;
        k ^= k >> r;
        k *= m;
        
        h ^= k;
        h *= m;
    }
    
    const unsigned char* data2 = (const unsigned char*)data;
    
    switch (key.length() & 7) {
    case 7: h ^= uint64_t(data2[6]) << 48;
    case 6: h ^= uint64_t(data2[5]) << 40;
    case 5: h ^= uint64_t(data2[4]) << 32;
    case 4: h ^= uint64_t(data2[3]) << 24;
    case 3: h ^= uint64_t(data2[2]) << 16;
    case 2: h ^= uint64_t(data2[1]) << 8;
    case 1: h ^= uint64_t(data2[0]);
            h *= m;
    };
    
    h ^= h >> r;
    h *= m;
    h ^= h >> r;
    
    return h;
}

void print_binary(uint64_t n) {
    for (int i = 63; i >= 0; --i) {
        std::cout << ((n >> i) & 1);
    }
    std::cout << std::endl;
}
int counting_zeros(uint64_t n){
	int count = 0;
	int final = 0; 
	for (int i=63;i >=0;--i){
		if(((n>>i) & 1) == 0){
			count ++;
		}
		else{
			if(count > final){
				final = count;
			}
			count = 0;
		}
	}
// In case the longest sequence ends at the end
	if (count > final) {
		final = count;
    }
	return final;
}

// Count leading zeros in a 64-bit integer
int count_leading_zeros(uint64_t n){
	if(n == 0)return 64;
	int count =0;
	for (int i =63;i>=0;--i){
		if(((n >> i)&1) == 0){
			count++;
		}else{
			break;
		}
	}
	return count;
}
void update_registers(uint64_t hash,std::vector<uint8_t> &registers){
	int p = 9;
	int m = 1<<p; //2^p
	int j = hash >> (64 - p); //get left p bits (leader)//extract bucket index 
	//shift p to the left to discard leader p we get earlier
	// to get remaining part of the hash
	uint64_t remaining = hash << p; 
	int rho = count_leading_zeros(remaining) + 1; 
	registers[j] = std::max(registers[j],(uint8_t)rho);

}
double estimate_cardinality(const std::vector<uint8_t>& registers) {
	const int m = registers.size(); //2^7 = 128 //number of registers
	// Choose the appropriate alpha value based on m
	double alpha;
	if (m == 16) {
		alpha = 0.673;
	} else if (m == 32) {
		alpha = 0.697;
	} else if (m == 64) {
		alpha = 0.709;
	} else {
		alpha = 0.7213 / (1.0 + 1.079 / m);  // General formula
	}

	//calculate harmonic mean
	//
	double sum = 0.0;
	int zeros = 0;
	for (int j=0;j<m;j++){
		sum = sum + 1.0/(1ULL << registers[j]);
		if (registers[j] == 0) zeros++;
	}
	double estimate = alpha * m * m /sum;
	//Small range correction
	if (estimate <= 2.5 * m) {
		if (zeros > 0) {
			estimate = m * std::log(static_cast<double>(m) / zeros);
		}
	}
	// Large range correction (for values > 2^32 / 30)
	if (estimate > (1ULL << 32) / 30.0) {
		estimate = -pow(2, 32) * log(1.0 - estimate / pow(2, 32));
	}
	return estimate;
}

// Process a file line by line rather than loading it all at once
void process_file(const std::string& filename, std::vector<uint8_t>& registers) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return;
    }
    
    std::string line;
    std::unordered_set<std::string> unique_words;
    
    while (std::getline(file, line)) {
        std::vector<std::string> words = split_sentence(line);
        for (const auto& word : words) {
            unique_words.insert(word);  // For actual count
            uint64_t hash_val = murmur_hash(word);
            update_registers(hash_val, registers);
        }
    }
    
    std::cout << "Actual unique words: " << unique_words.size() << std::endl;
}
int main(){
	const std::string filename = "t8.shakespeare.txt";
	const int p = 9;  // Using 2^7 = 128 registers
	const int m = 1 << p;

	std::vector<uint8_t> registers(m, 0); // m = number of registers = n^7 = 128
	std::cout << "Processing file: " << filename << std::endl;
	process_file(filename, registers);
	
	double car = estimate_cardinality(registers); 
	std::cout << "---------------------\n";
	std::cout << "Estimated cardinality: " << car << std::endl;
	return 0;
}
