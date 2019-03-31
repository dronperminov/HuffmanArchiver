#include <iostream>
#include <string>
#include <cstdlib>
#include <cassert>

#include "Haffman.hpp"

using namespace std;

string GetString(int n) {
	string s = "";

	for (int i = 0; i < n; i++) {
		int k = rand() % 3;

		if (k == 0) {
			s += '0' + rand() % 10;
		}
		else if (k == 1) {
			s += 'a' + rand() % 26;
		}
		else {
			s += 'A' + rand() % 26;
		}
	}

	return s;
}

void WritetoFile(const string& path, const string& s) {
	ofstream f(path.c_str());
	f << s;
	f.close();
}

string ReadFromFile(const string& path) {
	string s;
	ifstream f(path.c_str());
	getline(f, s);
	f.close();

	return s;
}

int main() {
	for (int n = 1; n <= 1000000; n *= 10) {
		string s = GetString(n);
		WritetoFile("in.txt", s);

		Haffman h1("in.txt", "compressed", "HF11", false);
		h1.Compress();

		Haffman h2("compressed", "out.txt", "HF11", false);
		h2.Decompress();

		string s2 = ReadFromFile("out.txt");

		assert(s == s2);
	}

	system("rm in.txt out.txt compressed");
}