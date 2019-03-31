#include <iostream>
#include <string>

#include "Haffman.hpp"

using namespace std;

// вывод сообщения об использовании программы
void help() {
	cout << "Usage: ./haffman arguments" << endl;
	cout << "Arguments" << endl;
	cout << "  --compress              for compressing" << endl;
	cout << "  --decompress            for decompressing" << endl;
	cout << "  --in=[path/to/file]     path to input file" << endl;
	cout << "  --out=[path/to/file]    path to output file" << endl;
	cout << "  --prefix=[HF11]         prefix for algorithm identification" << endl;
	cout << "  --info                  print information" << endl;
}

int main(int argc, char **argv) {
	// если аргуметов два и второе --help
	if (argc == 2 && string(argv[1]) == "--help") {
		help(); // выводим информацию об использовании программы
		return 0;
	}

	// если аргументов не 5 и не 4, то выходим
	if (argc < 4 || argc > 5) {
		cout << "Invalid number of arguments. Check --help for usage" << endl;
		return -1;
	}

	string mode = ""; // режим работы
	string input = ""; // входной файл
	string output = ""; // выходной файл
	string prefix = ""; // префикс
	bool info = false;

	for (int i = 1; i < argc; i++) {
		string arg = argv[i];

		if (arg == "--compress" || arg == "--decompress") {
			mode = arg;
		}
		else if (!arg.compare(0, 5, "--in=")) {
			input = arg.substr(5);
		}
		else if (!arg.compare(0, 6, "--out=")) {
			output = arg.substr(6);
		}
		else if (!arg.compare(0, 9, "--prefix=")) {
			prefix = arg.substr(9);
		}
		else if (arg == "--info") {
			info = true;
		}
		else {
			cout << "Invalid argument '" << arg << "'" << endl; // иначе некорректный аргумент
			return -1; // выходим
		}
	}

	if (input == "") {
		cout << "input file is not set" << endl;
		return -1;
	}

	if (output == "") {
		cout << "output file is not set" << endl;
		return -1;
	}

	if (prefix == "")
		prefix = "HF11";

	try {
		Haffman haffman(input, output, prefix, info);

		if (mode == "--compress") {
			haffman.Compress();
		}
		else {
			haffman.Decompress();
		}
	}
	catch (string error) {
		cout << "Error: " << error << endl;
	}

	return 0;
}