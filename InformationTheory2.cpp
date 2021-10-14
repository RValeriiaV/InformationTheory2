#include <fstream>
#include <iostream>
#include <cmath>
#include "Letter.h"
#include "Array.h"
using namespace std;

//	ПОСЛЕ КОНСТРУКТОРА КОПИРОВАНИЯ LETTER ВЫЗЫВАЕТСЯ ДЕСТРУКТОР

typedef unsigned long long int ulInt;

ulInt* Frequency(ifstream& fin, int& extra, ulInt& sizeOfAlphabet);
Array* Fano(Array start);
void Splitting(Array start, Array arr1, Array arr2);
void Sort(ulInt* array, ulInt size); //по убыванию


int main() {
	setlocale(LC_ALL, "Russian");
	while (true) {
		cout << "Выберите действие: " << endl;
		cout << "1. Сжать файл" << endl;
		cout << "2. Распаковать файл" << endl;
		cout << "0. Завершить работу программы" << endl;
		char flag;
		cin >> flag;
		switch (flag) {

		//кодирование файла
		case '1': {
			cout << "Введите имя файла" << endl;
			string filename;
			cin >> filename;
			ifstream fin(filename, ios::binary | ios::in);
			if (!fin.is_open()) {
				cout << "Извините, файл не найден" << endl << endl;
				break;
			}
			
			int extra;
			ulInt sizeOfAlphabet;
			ulInt* frequency = Frequency(fin, extra, sizeOfAlphabet);

			Sort(frequency, sizeOfAlphabet);
			ulInt i = 0;
			ulInt shift = 0;

			while (frequency[i] == 0) { //убираем из рассмотрения нули
				shift++;
				i++;
			}

			for (ulInt i = shift, j = 0; i < sizeOfAlphabet; i++, j++) {
				frequency[j] = frequency[i];
			}
			sizeOfAlphabet -= shift;
			
			//из массива ulInt создаем массив Letter
			Letter* ForFano = new Letter[sizeOfAlphabet];
			for (ulInt i = 0; i < sizeOfAlphabet; i++) ForFano[i] = Letter(frequency[i]);

			Array arrayForFano = Array(ForFano, sizeOfAlphabet, sizeOfAlphabet);

			//так как при создании Array значения Letter копируются, массив ForFano можно удалять
			delete[]ForFano;

			Fano(arrayForFano);
			
			break;
		}

		//распаковка файла
		case '2': {
			//распаковка файла
			break;
		}
		case '0': return 0;
		default: {
			cout << "Извините, действие не найдено. Попробуйте ещё раз" << endl << endl;
			break;
		}
		}
	}
}

//возвращает массив встречаемости буквы (индекс - буква, значение - количество встреч)
ulInt* Frequency(ifstream& fin, int& extra, ulInt& sizeOfAlphabet) {
	unsigned int sizeOfLetter; //длина буквы в битах

			//определение длины буквы
	while (true) {
		cout << "Введите длину буквы в битах (не больше 32)" << endl;
		cin >> sizeOfLetter;
		if (sizeOfLetter < 1) {
			cout << "Действие невозможно. Операция отменена" << endl << endl;
			break;
		}
		else
			if (sizeOfLetter == 1) {
				cout << "Действие бесполезно. Операция отменена" << endl << endl;
				break;
			}
			else {
				cout << "Результат будет сохранен в файле Сжатие.bin" << endl;
				break;
			}
	}

	fin.seekg(0, ios::end); //перемещаемся в конец файла
	ulInt size = fin.tellg(); //измеряем расстояние от начала файла до курсора (в байтах)

	sizeOfAlphabet = 1; //потенциальное количество букв входного алфавита
	for (unsigned int i = 0; i < sizeOfLetter; i++) sizeOfAlphabet *= 2;

	//сколько раз встретилась буква: индекс - буква, значение - количество встреч
	ulInt* frequency = new ulInt[sizeOfAlphabet];
	for (ulInt i = 0; i < sizeOfAlphabet; i++) frequency[i] = 0;

	int _inLetter = 0; //какой по номеру бит в букве учтен
	ulInt letter = 0;

	fin.seekg(0, ios::beg);
	for (ulInt i = 0; i < size; i++) { //читаем побайтово

		unsigned char buf; //хранится байт
		fin.read((char*)&buf, sizeof(char));

		unsigned char divisor = 128; //вспомогательное

		for (int j = 0; j < 8; j++) { //разбираем побитово
			if (_inLetter == sizeOfLetter) { //буква готова
				frequency[letter]++; //учитываем, что встретили ее
				letter = 0; //обнуляем букву
				_inLetter = 0; //обнуляем позицию в букве
			}
			char bit = buf / divisor; //рассматриваем старший бит
			letter = letter << 1; //сдвинули биты
			letter += bit; //дописали новый бит
			_inLetter++;
			buf %= divisor; //оставили все кроме сташего бита
			divisor /= 2;
		}
	}

	//рассматриваем последнюю букву
	extra = 0; //лишние нули в конце
	while (_inLetter != sizeOfLetter) {
		letter = letter << 1; //дописываем нули справа
		extra++;
		_inLetter++;
	}
	frequency[letter]++;

	return frequency;
}

//принимает массив букв и возвращает его с дописанными кодами
Array* Fano(Array start) {
	Array arr1 = Array(start.getSizeArr());
	Array arr2 = Array(start.getSizeArr());
	arr1.setSize(0);
	arr2.setSize(0);
	Splitting(start, arr1, arr2);
	for (int i = 0; i < arr1.getSize(); i++) cout << arr1.getArray()[i].getFrequency() << "  ";
	for (int i = 0; i < arr2.getSize(); i++) cout << arr2.getArray()[i].getFrequency() << "  ";
	return NULL;
}

void Splitting(Array start, Array arr1, Array arr2) {
	if (start.getSum() + arr1.getSum() <= arr2.getSum()) {
		for (ulInt i = 0, j = arr1.getSize(); i < start.getSize(); i++)
			arr1.getArray()[j] = start.getArray()[i];
		arr1.setSum(arr1.getSum() + start.getSum());
		arr1.setSize(arr1.getSize() + start.getSize());
	}
	else if (start.getSum() + arr2.getSum() <= arr1.getSum()) {
		for (ulInt i = 0, j = arr2.getSize(); i < start.getSize(); i++)
			arr2.getArray()[j] = start.getArray()[i];
		arr2.setSum(arr1.getSum() + start.getSum());
		arr2.setSize(arr1.getSize() + start.getSize());
	}


	Array firstArr1 = Array(start.getSizeArr());
	Array firstArr2 = Array(start.getSizeArr()); 
	Array secondArr1 = Array(start.getSizeArr()); 
	Array secondArr2 = Array(start.getSizeArr());
	firstArr1 = arr1; 
	firstArr2 = arr2; 
	secondArr1 = arr1; 
	secondArr2 = arr2;

	//добавляем самый большой элемент исходного массива в первый массив первой ветви
	firstArr1.getArray()[firstArr1.getSize()] = start.getArray()[start.getSize() - 1];
	firstArr1.setSize(firstArr1.getSize() + 1);
	firstArr1.setSum(firstArr1.getSum() + start.getArray()[start.getSize() - 1].getFrequency());

	//во второй массив второй ветви
	secondArr2.getArray()[secondArr2.getSize()] = start.getArray()[start.getSize() - 1];
	secondArr2.setSize(secondArr2.getSize() + 1);
	secondArr2.setSum(secondArr2.getSum() + start.getArray()[start.getSize() - 1].getFrequency());

	//скрываем из рассмотрения в основном массиве этот элемент
	start.setSum(start.getSum() - start.getArray()[start.getSize() - 1].getFrequency());
	start.setSize(start.getSize() - 1);

	Splitting(start, firstArr1, firstArr2);
	Splitting(start, secondArr1, secondArr2);

	long long int delta1 = firstArr1.getSum() - firstArr2.getSum();
	long long int delta2 = secondArr1.getSum() - secondArr2.getSum();
	if (abs(delta1) < abs(delta2)) {
		arr1 = firstArr1; 
		arr2 = firstArr2;
	}
	else {
		arr1 = secondArr1;
		arr2 = secondArr2;
	}
	
	firstArr1.~Array();
	firstArr2.~Array();
	secondArr1.~Array();
	secondArr2.~Array();
	return;
}



void Part(ulInt* A, ulInt L, ulInt R, ulInt& i, ulInt& j) {
	i = L; j = R;
	ulInt x = A[(i + j) / 2];
	do {
		while (A[i] < x) i++;
		while (A[j] > x) j--;
		if (i <= j) {
			ulInt t = A[i];
			A[i] = A[j];
			A[j] = t;
			i++;
			j--;
		}
	} while (i <= j);
}
void QuickSort(ulInt* A, ulInt L, ulInt R) {
	ulInt i, j;
	Part(A, L, R, i, j);
	if (L < j) QuickSort(A, L, j);
	if (i < R) QuickSort(A, i, R);
}
void QuickSort(ulInt* A, ulInt n) {
	QuickSort(A, 0, n - 1);
}
void Sort(ulInt* array, ulInt size) { //по возрастанию
	QuickSort(array, size); //по возрастанию
	/*for (ulInt i = 0; i < size / 2; i++) {
		ulInt buf = array[i];
		array[i] = array[size - 1 - i];
		array[size - 1 - i] = buf;
	}*/
}
