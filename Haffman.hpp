#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <vector>
#include <map>

using namespace std;

class Haffman {
	// структура статистики
	struct Stat {
		char c; // символ
		int p; // сколько раз этот символ встречается
		string code; // код Хаффмана для символа
	};

	// дерево Хаффмана
	struct Tree {
		Stat value;
		Tree *left;
		Tree *right;
	};

	string input; // путь к входному файлу
	string output; // путь к выхдному файлу
	string phrase; // префикс фраза
	bool info;

	vector<Stat> dictionary; // словарь статистики

	Tree *GenerateTree(); // построение дерева Хаффмана
	void SetCodes(Tree *tree, string path = "");

	int IndexOfChar(char c); // индекс символа в словаре

	int CreateDictionary(const string& path); // создание таблицы символов из сжимаемого файла
	map<string, char> ReadDictionary(ifstream &f); // считывание таблицы символов из сжатого файла

public:
	Haffman(const string& input, const string& output, const string& phrase, bool info);

	void Compress(); // сдатие данных
	void Decompress(); // распаковка данных
};

// конструктор
Haffman::Haffman(const string& input, const string& output, const string& phrase, bool info) {
	this->input = input;
	this->output = output;
	this->phrase = phrase;
	this->info = info;
}

// формирование дерева Хаффмана
Haffman::Tree* Haffman::GenerateTree() {
	list<Tree *> list; // создаём список вершин дерева

	for (size_t i = 0; i < dictionary.size(); i++) {
		Tree *node = new Tree;
		node->value = dictionary[i];
		node->left = NULL;
		node->right = NULL;

		list.push_back(node);
	}
	
	// пока в списке не останется верхняя вершина
	while (list.size() != 1) {
		list.sort([this](const Tree* t1, const Tree* t2) { return t1->value.p < t2->value.p; }); // сортируем список

		Tree *left = list.front(); // слева будет первый минимум
		list.pop_front();

		Tree *right = list.front(); // справа будет второй минимум
		list.pop_front();

		Tree *tree = new Tree; // создаём новую вершину
		tree->left = left;
		tree->right = right;
		tree->value.p = left->value.p + right->value.p; // значение частоты есть сумма подвершин

		list.push_front(tree); // вставляем новую вершину в список
	}

	Tree *tree = list.front(); // получаем элемент из списка

	return tree; // возвращаем корень дерева
}

// расстановка кодов в дереве
void Haffman::SetCodes(Tree *tree, string path) {
	if (!tree->left && !tree->right) {
		dictionary[IndexOfChar(tree->value.c)].code = path == "" ? "0" : path;
		return;
	}

	SetCodes(tree->left, path + "0");
	SetCodes(tree->right, path + "1");
}

// индекс символа в словаре
int Haffman::IndexOfChar(char c) {
	for (size_t i = 0; i < dictionary.size(); i++)
		if (dictionary[i].c == c) // если нашли символ
			return i; // возвращаем его индекс

	return -1; // иначе возвращаем -1
}

// создание таблицы символов из сжимаемого файла
int Haffman::CreateDictionary(const string& path) {
	ifstream f(input.c_str(), ios::binary);

	if (!f)
		throw string("Invalid input file");

	int totalBytes = 0; // общее число байт в файле
	char byte; // байт для считывания

	while (f.read(&byte, 1)) {
		totalBytes++; // увеличиваем счётчик байт

		int index = IndexOfChar(byte); // ищем считанный символ в словаре

		// если символа в массиве нет, то нужно его добавить
		if (index == -1) {
			Stat stat;
			stat.c = byte;
			stat.p = 1;

			dictionary.push_back(stat);
		}
		else {
			dictionary[index].p++; // увеличиваем значение вероятности
		}
	}

	f.close(); // закрываем входной файл

	Tree *tree = GenerateTree(); // формируем дерево Хаффмана
	SetCodes(tree); // проставляем коды

	return totalBytes; // возвращаем считанное число байт
}

// считывание таблицы символов из сжатого файла
map<string, char> Haffman::ReadDictionary(ifstream &f) {
	int statBytes; // число элементов в таблице статистики
	char c;

	f >> statBytes; // считываем число символов в таблице статистики

	map<string, char> map; // таблица соответствия между кодом и символом

	// считываем таблицу символов
	for (int i = 0; i < statBytes; i++) {
		Stat stat;

		f.read(&c, 1); // пропускаем пробел
		f.read(&stat.c, 1); // считываем символ
		f.read(&c, 1); // пропускаем двоеточие
		f >> stat.p; // считываем частоту

		dictionary.push_back(stat); // добавляем в таблицу
	}

	f.read(&c, 1); // игнорируем пробел после последнего кода

	Tree *tree = GenerateTree(); // формируем дерево Хаффмана
	SetCodes(tree); // проставляем коды

	// формируем таблицу соответствия кода и символа
	for (size_t i = 0; i < dictionary.size(); i++)
		map[dictionary[i].code] = dictionary[i].c;

	return map; // возвращаем таблицу
}

// сжатие файла
void Haffman::Compress() {
	int totalBytes = CreateDictionary(input);

	ofstream fout(output, ios::binary);

	fout << phrase << " "; // записываем фразу
	fout << totalBytes << " "; // записываем длину исходных данных
	fout << dictionary.size() << " "; // записываем размер таблицы статистики

	// записываем таблицу статистики
	for (size_t i = 0; i < dictionary.size(); i++)
		fout << dictionary[i].c << ":" << dictionary[i].p << " ";

	string code = ""; // код символа
	char byte; // байт для считывания
	int k = 0; // счётчик считанных байт

	ifstream fin(input.c_str(), ios::binary);

	// пока есть байты во входном файле
	while (fin.read(&byte, 1)) {
		k++; // увеличиваем счётчик считанных байт
		code += dictionary[IndexOfChar(byte)].code; // добавляем код символа

		// пока длина кода больше 8 или дошли до последнего
		while (code.length() >= 8 || (k == totalBytes && code.length())) {
			byte = 0;

			// формируем байт
			for (size_t i = 0; i < 8; i++)
				byte = (byte << 1) | (i < code.length() ? code[i] - '0' : 0);

			fout.write(&byte, 1); // записываем байт
			code.erase(0, 8); // сокращаем строку
		}
	}

	
	// вывод статистики
	if (info) {
		double avg = 0;

		for (size_t i = 0; i < dictionary.size(); i++)
			avg += dictionary[i].p * 1.0 / totalBytes * dictionary[i].code.length();

		cout << endl;
		cout << "Input data length: " << totalBytes << " bytes" << endl;
		cout << "Output data length: " << (fout.tellp()) << " bytes" << endl << endl;
		cout << "Compressed value: " << (100 * (1.0 - fout.tellp() * 1.0 / totalBytes)) << "%" << endl;

		cout << "Average code length: " << avg << " bits / character" << endl << endl;
	}

	fin.close(); // закрываем входной файл
	fout.close(); // закрываем выходной файл
}

// распаковка данных
void Haffman::Decompress() {
	ifstream fin(input.c_str(), ios::binary);
	
	fin.seekg (0, fin.end);
    int length = fin.tellg(); // общий размер файла в байтах
    fin.seekg (0, fin.beg);

	// если файл не открылся
	if (!fin)
		throw string("Invalid input file for decompressing"); // бросаем исключение

	string phraseFile; // префикс фраза
	fin >> phraseFile; // считываем префикс фразу

	// если не совпадают фразы, то бросаем исключение
	if (phraseFile != phrase)
		throw string("phrase does not match");
	
	int totalBytes; // числоа байт в тексте
	fin >> totalBytes; // считываем число байт в тексте
	
	map<string, char> map = ReadDictionary(fin); // считываем таблицу символов

	ofstream fout(output, ios::binary); // открываем выходной файл
	string code = ""; // код символа

	// считываем сжатые данные
	while (length && totalBytes) {
		char byte;

		fin.read(&byte, 1);
		length--; // уменьшаем счётчик оставшихся символов

		for (int i = 0; i < 8 && totalBytes; i++) {
			code += '0' + ((byte >> (7 - i)) & 1);

			std::map<string, char>::iterator it = map.find(code);

			if (it != map.end()) {
				fout << it->second; // выводим в выходной файл символ по найденному индексу
				
				totalBytes--; // уменьшаем количество считанных байт
				code = ""; // сбрасываем код
			}
		}
	}

	fin.close(); // закрываем входной файл
	fout.close(); // закрываем выходной файл
}