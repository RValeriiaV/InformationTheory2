#include <fstream>
#include <iostream>
#include <cmath>
#include "Letter.h"
#include "Array.h"
using namespace std;

typedef unsigned long long int ulInt;

ulInt* Frequency(ifstream& fin, int& extra, ulInt& sizeOfAlphabet, char& letterSize);
void Fano(Array*& start);
void Splitting(Array* start, Array*& arr1, Array*& arr2);
void Sort(ulInt* array, ulInt size); //по убыванию

void Heading(Array* arr, ulInt sizeOfAlphabet, ofstream& fout, char sizeOfLetter);
void MainPart(ifstream& fin, ofstream& fout, Array* arr, char letterSize);

void ReadHeading(Array* arr, ulInt sizeOfAlphabet, char letterSize, ifstream& fin);

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
			char letterSize;
			ulInt* frequency = Frequency(fin, extra, sizeOfAlphabet, letterSize);
			if (!frequency) break;
			ulInt* letters = new ulInt[sizeOfAlphabet];
			for (ulInt i = 0; i < sizeOfAlphabet; i++)
				letters[i] = frequency[i]; //сохраняем неотсортированный массив, чтобы потом можно было сопоставить буквы и коды

			Sort(frequency, sizeOfAlphabet);
			ulInt i = 0;
			ulInt shift = 0;

			while (frequency[i] == 0) { //убираем из рассмотрения нули
				shift++;
				i++;
			}

			for (ulInt i = shift, j = 0; i < sizeOfAlphabet; i++, j++) frequency[j] = frequency[i]; 
			

			//из массива ulInt создаем массив Letter
			Letter** ForFano = new Letter*[sizeOfAlphabet - shift];
			for (ulInt i = 0; i < sizeOfAlphabet - shift; i++) ForFano[i] = new Letter(frequency[i]);
			delete[]frequency;

			Array* arrayForFano = new Array(ForFano, sizeOfAlphabet - shift, sizeOfAlphabet - shift);

			//так как при создании Array значения Letter копируются, массив ForFano можно удалять
			for (ulInt i = 0; i < sizeOfAlphabet - shift; i++) delete ForFano[i];
			delete[]ForFano;

			Fano(arrayForFano);

			Array* arrayForRecord = new Array(sizeOfAlphabet);
			arrayForRecord->setSize(arrayForFano->getSize());
			for (ulInt i = 0; i < arrayForFano->getSize(); i++)
				(*arrayForRecord->getArray()[i]) = (*arrayForFano->getArray()[i]);
			delete arrayForFano;

			Letter* buf;
			for (ulInt i = 0; i < sizeOfAlphabet; i++) {
				buf = arrayForRecord->getArray()[i];
				if (letters[i] == 0) {
					arrayForRecord->getArray()[i] = arrayForRecord->getArray()[arrayForRecord->getSize()];
					arrayForRecord->getArray()[arrayForRecord->getSize()] = buf;
					arrayForRecord->setSize(arrayForRecord->getSize() + 1);
				}
				else {
					ulInt j = i;
					while (letters[i] != arrayForRecord->getArray()[j]->getFrequency()) 
						j++; //ищем элемент с той же встречаемостью, что и буква
					arrayForRecord->getArray()[i] = arrayForRecord->getArray()[j];
					arrayForRecord->getArray()[j] = buf;
				}
			}
			sizeOfAlphabet -= shift; 
			ofstream fout("Сжатие.bin", ios::binary);
			//мощность алфавита без учета нулевых букв
			fout.write((char*)&sizeOfAlphabet, sizeof(ulInt));
			fout.write(&letterSize, sizeof(char)); //длина исходной буквы
			fout.write((char*)&extra, sizeof(char)); //прочитано лишних бит

			int excess; //выравнивание на байт при записи
			ulInt sum = 0; //сколько битов будет в новом тексте
			for (ulInt i = 0; i < arrayForRecord->getSize(); i++)
				sum += (arrayForRecord->getArray()[i]->getFrequency() * arrayForRecord->getArray()[i]->getSizeOfCode());
			excess = (8 - sum % 8) % 8;

			fout.write((char*)&excess, sizeof(char)); //записано лишних бит 

			
			//нужно просмотреть весь массив, поэтому с нулевыми буквами
			Heading(arrayForRecord, sizeOfAlphabet + shift, fout, letterSize);

			MainPart(fin, fout, arrayForRecord, letterSize);

			delete arrayForRecord;
			fout.close();
			
			break;
		}

		//распаковка файла
		case '2': {
			cout << "Введите имя файла" << endl;
			string filename;
			cin >> filename;
			ifstream fin(filename, ios::binary | ios::in);
			if (!fin.is_open()) {
				cout << "Извините, файл не найден" << endl << endl;
				break;
			}

			ulInt sizeOfAlphabet;
			fin.read((char*)&sizeOfAlphabet, sizeof(ulInt));
			char letterSize;
			fin.read(&letterSize, sizeof(char));
			char extraReading; 
			fin.read(&extraReading, sizeof(char));
			char extraRecording; 
			fin.read(&extraRecording, sizeof(char));

			ulInt sizeArr = 1; //сколько букв могло бы быть в исходном алфавите
			for (int i = 0; i < letterSize; i++) sizeArr *= 2; 
			Array* arr = new Array(sizeArr);

			for (ulInt i = 0; i < sizeArr; i++)
				arr->getArray()[i] = new Letter();

			ReadHeading(arr, sizeOfAlphabet, letterSize, fin);
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
ulInt* Frequency(ifstream& fin, int& extra, ulInt& sizeOfAlphabet, char& letterSize) {
	int sizeOfLetter; //длина буквы в битах

	//определение длины буквы
	while (true) {
		cout << "Введите длину буквы в битах (не больше 32)" << endl;
		cin >> sizeOfLetter;
		if (sizeOfLetter < 1) {
			cout << "Действие невозможно. Операция отменена" << endl << endl;
			return NULL;
		}
		else
			if (sizeOfLetter == 1) {
				cout << "Действие бесполезно. Операция отменена" << endl << endl;
				return NULL;
			}
			else {
				cout << "Результат будет сохранен в файле Сжатие.bin" << endl;
				break;
			}
	}

	letterSize = sizeOfLetter;

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
	if (_inLetter != 0) //если буква началась, но не закончилась
		while (_inLetter != sizeOfLetter) {
			letter = letter << 1; //дописываем нули справа
			extra++;
			_inLetter++;
		}
	frequency[letter]++;

	return frequency;
}

//дописывание цифры к кодовой цепочке, возвращает true, если дальше дробить не надо (было не больше двух элементов)
bool Adding(Array* arr, int digit) {
	for (ulInt i = 0; i < arr->getSize(); i++) {
		arr->getArray()[i]->setCode(arr->getArray()[i]->getCode() * 2 + digit);
		arr->getArray()[i]->setSizeOfCode(arr->getArray()[i]->getSizeOfCode() + 1);
	}

	if (arr->getSize() == 1) return true;

	if (arr->getSize() == 2) {
		arr->getArray()[0]->setCode(arr->getArray()[0]->getCode() * 2); //как бы дописали 0
		arr->getArray()[0]->setSizeOfCode(arr->getArray()[0]->getSizeOfCode() + 1);
		arr->getArray()[1]->setCode(arr->getArray()[1]->getCode() * 2 + 1); //как бы дописали 0
		arr->getArray()[1]->setSizeOfCode(arr->getArray()[1]->getSizeOfCode() + 1);
		return true;
	}
	return false;
}

//принимает массив букв и возвращает его с дописанными кодами
void Fano(Array*& start) {
	Array* arr1 = new Array(start->getSize());
	Array* arr2 = new Array(start->getSize());
	arr1->setSize(0);
	arr2->setSize(0);
	Splitting(start, arr1, arr2);
	
	if (!Adding(arr1, 0)) {
		arr1->Sort();
		Fano(arr1);
	}
	if (!Adding(arr2, 1)) {
		arr2->Sort();
		Fano(arr2);
	}

	delete start;

	start = new Array(arr1->getSize() + arr2->getSize());
	ulInt i = 0;
	for (ulInt j = 0; j < arr1->getSize(); j++, i++)
		(*start->getArray()[i]) = (*arr1->getArray()[j]);
	for (ulInt j = 0; j < arr2->getSize(); j++, i++)
		(*start->getArray()[i]) = (*arr2->getArray()[j]);
	start->setSum(arr1->getSum() + arr2->getSum());
	start->Sort();

	delete arr1;
	delete arr2;
		
	/*for (int i = 0; i < arr1->getSize(); i++) cout << arr1->getArray()[i]->getFrequency() << "  ";
	for (int i = 0; i < arr2->getSize(); i++) cout << arr2->getArray()[i]->getFrequency() << "  ";*/
	return;
}

void Splitting(Array* start, Array*& arr1, Array*& arr2) {
	if (start->getSum() + arr1->getSum() <= arr2->getSum()) {
		for (ulInt i = 0, j = arr1->getSize(); i < start->getSize(); i++, j++)
			(*arr1->getArray()[j]) = (*start->getArray()[i]);
		arr1->setSum(arr1->getSum() + start->getSum());
		arr1->setSize(arr1->getSize() + start->getSize());
		return;
	}
	else if (start->getSum() + arr2->getSum() <= arr1->getSum()) {
		for (ulInt i = 0, j = arr2->getSize(); i < start->getSize(); i++, j++)
			(*arr2->getArray()[j]) = (*start->getArray()[i]);
		arr2->setSum(arr2->getSum() + start->getSum());
		arr2->setSize(arr2->getSize() + start->getSize());
		return;
	}


	Array* firstArr1 = new Array(arr1->getSizeArr());
	Array* firstArr2 = new Array(arr2->getSizeArr());

	(*firstArr1) = (*arr1); 
	(*firstArr2) = (*arr2); 
	

	//добавляем самый большой элемент исходного массива в первый массив первой ветви
	(*firstArr1->getArray()[firstArr1->getSize()]) = (*start->getArray()[start->getSize() - 1]);
	firstArr1->setSize(firstArr1->getSize() + 1);
	firstArr1->setSum(firstArr1->getSum() + start->getArray()[start->getSize() - 1]->getFrequency());

	//скрываем из рассмотрения в основном массиве этот элемент
	start->setSum(start->getSum() - start->getArray()[start->getSize() - 1]->getFrequency());
	start->setSize(start->getSize() - 1);

	//сохраняем, так как рекурсия испортит значения
	ulInt sizeOfStart = start->getSize();
	ulInt sumOfstart = start->getSum();

	Splitting(start, firstArr1, firstArr2);
	long long int delta1 = firstArr1->getSum() - firstArr2->getSum();

	if (abs(delta1) < 2) { //лучше уже не будет, дельта 0 или 1
		delete arr1;
		delete arr2;
		arr1 = firstArr1;
		arr2 = firstArr2;
		return;
	}

	if (arr1->getSum() != arr2->getSum()) { //если суммы массивов равны, нет смысла ставить
		//во второй массив второй ветви
		Array* secondArr1 = new Array(arr1->getSizeArr());
		Array* secondArr2 = new Array(arr2->getSizeArr());
		(*secondArr1) = (*arr1);
		(*secondArr2) = (*arr2);
		start->setSize(sizeOfStart);
		start->setSum(sumOfstart);
		(*secondArr2->getArray()[secondArr2->getSize()]) = (*start->getArray()[start->getSize()]);
		secondArr2->setSize(secondArr2->getSize() + 1);
		secondArr2->setSum(secondArr2->getSum() + start->getArray()[start->getSize()]->getFrequency());

		Splitting(start, secondArr1, secondArr2);

		long long int delta2 = secondArr1->getSum() - secondArr2->getSum();
		if (abs(delta1) < abs(delta2)) {
			delete arr1;
			delete arr2;
			arr1 = firstArr1;
			arr2 = firstArr2;
			delete secondArr1;
			delete secondArr2;
		}
		else {
			delete arr1;
			delete arr2;
			arr1 = secondArr1;
			arr2 = secondArr2;
			delete firstArr1;
			delete firstArr2;

		}
	}
	else {
		delete arr1;
		delete arr2;
		arr1 = firstArr1;
		arr2 = firstArr2;
	}
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

void Heading(Array* arr, ulInt sizeOfAlphabet, ofstream& fout, char sizeOfLetter) {

	for (ulInt i = 0; i < arr->getSize(); i++) {
		if (arr->getArray()[i]->getSizeOfCode() != 0) {
			fout.write((char*)&i, ceil(sizeOfLetter / 8.0) * sizeof(char));

			char newLetterSize = arr->getArray()[i]->getSizeOfCode();
			fout.write(&newLetterSize, sizeof(char));

			ulInt code = arr->getArray()[i]->getCode();
			fout.write((char*)&code, ceil(newLetterSize / 8.0) * sizeof(char));
		}
	}
}

void remainderForRecord(ulInt& letter, Array* arr, ulInt& remainder, char& sizeOfRemainder, ofstream& fout) {
	//добавляем к цепочки для записи новый код собранной буквы
	remainder = (remainder << arr->getArray()[letter]->getSizeOfCode()) + arr->getArray()[letter]->getCode();
	sizeOfRemainder += arr->getArray()[letter]->getSizeOfCode();

	ulInt buf_divisor = 1; //чтобы отрезать старшие разряды от remainder
	for (int i = 0; i < sizeOfRemainder - 1; i++) buf_divisor *= 2;

	//пока цепочка для записи не меньше байта делаем
	while (sizeOfRemainder > 7) {

		char forRecording = 0; //байт, который будет записан
		for (int i = 0; i < 8; i++) {
			char bit = remainder / buf_divisor; //старший разряд цепочки для записи
			remainder %= buf_divisor; //всё, кроме старшего разряда

			forRecording = forRecording << 1; //к байту для записи дописываем отрезанный разряд
			forRecording += bit;

			buf_divisor /= 2;
		}
		fout.write(&forRecording, sizeof(char)); //записываем собранный байт
		sizeOfRemainder -= 8;
	}
}

void MainPart(ifstream& fin, ofstream& fout, Array* arr, char letterSize) {

	//работа с исходным файлом
	fin.seekg(0, ios::end); //перемещаемся в конец файла
	ulInt size = fin.tellg(); //измеряем расстояние от начала файла до курсора (в байтах)

	int _inLetter = 0; //какой по номеру бит в букве учтен
	ulInt letter = 0;

	fin.seekg(0, ios::beg);

	ulInt remainder = 0; //остаток записываемого кода (цепочка для записи)
	char sizeOfRemainder = 0; //битовая длина остатка

	for (ulInt i = 0; i < size; i++) { //читаем побайтово

		unsigned char buf; //хранится байт
		fin.read((char*)&buf, sizeof(char));

		unsigned char divisor = 128; //вспомогательное

		for (int j = 0; j < 8; j++) { //разбираем побитово
			if (_inLetter == letterSize) { //буква готова
				remainderForRecord(letter, arr, remainder, sizeOfRemainder, fout);
				letter = 0; //обнуляем букву
				_inLetter = 0; //обнуляем позицию в букве
			}

			//собираем букву
			char bit = buf / divisor; //рассматриваем старший бит
			letter = letter << 1; //сдвинули биты
			letter += bit; //дописали новый бит
			_inLetter++;
			buf %= divisor; //оставили все кроме сташего бита
			divisor /= 2;
		}
	}
	if (_inLetter != 0) { //рассматриваем последнюю букву, если ей не хватило бит
		while (_inLetter != letterSize) {
			letter = letter << 1; //дописываем нули справа
			_inLetter++;
		}
		remainderForRecord(letter, arr, remainder, sizeOfRemainder, fout);
	}

	//если цепочка для записи не пуста
	if (sizeOfRemainder != 0)
		while (sizeOfRemainder < 8) { //добавляем нулевые биты
			remainder = remainder << 1;
			sizeOfRemainder++;
		}
	fout.write((char*)&remainder, sizeof(char));
}

void ReadHeading(Array* arr, ulInt sizeOfAlphabet, char letterSize, ifstream& fin) {
	for (ulInt i = 0; i < sizeOfAlphabet; i++) {
		ulInt letter = 0; 
		char length = ceil(letterSize / 8.0);
		fin.read((char*)&letter, length * sizeof(char));

		char sizeOfCode; 
		fin.read(&sizeOfCode, sizeof(char));

		ulInt code = 0; 
		length = ceil(sizeOfCode / 8.0);
		fin.read((char*)&code, length * sizeof(char));

		arr->getArray()[letter]->setCode(code);
		arr->getArray()[letter]->setSizeOfCode(sizeOfCode);
	}
}

