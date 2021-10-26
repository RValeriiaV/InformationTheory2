#include "Array.h"

Array::Array() {
	array = NULL;
	sizeArr = 0;
	size = 0; 
	sum = 0;
}
Array::Array(ulInt sizeArr) {
	array = new Letter* [sizeArr];
	for (ulInt i = 0; i < sizeArr; i++) array[i] = new Letter();
	this->sizeArr = sizeArr;
	this->sum = 0;
	this->size = sizeArr;
}
Array::Array(Letter** array, ulInt sizeArr, ulInt size) {
	this->array = new Letter*[sizeArr];
	sum = 0;
	for (ulInt i = 0; i < size; i++) {
		this->array[i] = new Letter(array[i]);
		sum += array[i]->getFrequency();
	}
	for (ulInt i = size; i < sizeArr; i++) this->array[i] = new Letter();
	this->sizeArr = sizeArr;
	this->size = size;
}

Letter** Array::getArray() {
	return array;
}
void Array::setArray(Letter** array) {
	this->array = array;
}

ulInt Array::getSize() {
	return size;
}
void Array::setSize(ulInt size) {
	this->size = size; 
}

ulInt Array::getSizeArr() {
	return sizeArr;
}
void Array::setSizeArr(ulInt sizeArr) {
	this->sizeArr = sizeArr;
}

ulInt Array::getSum() {
	return sum; 
}
void Array::setSum(ulInt sum) {
	this->sum = sum;
}



////быстрая сортировка
//
//void Part(Letter** A, ulInt L, ulInt R, ulInt& i, ulInt& j) {
//	i = L; j = R;
//	ulInt x = A[(i + j) / 2]->getFrequency();
//	do {
//		while (A[i]->getFrequency() < x) i++;
//		while (A[j]->getFrequency() > x) j--;
//		if (i <= j) {
//			Letter* t = A[i];
//			A[i] = A[j];
//			A[j] = t;
//			i++;
//			j--;
//		}
//	} while (i <= j);
//}
//
//
//void QuickSort(Letter** A, ulInt L, ulInt R) {
//	ulInt i, j;
//	Part(A, L, R, i, j);
//	if (L < j) QuickSort(A, L, j);
//	if (i < R) QuickSort(A, i, R);
//}
//
//void QuickSort(Letter** A, ulInt n) {
//	QuickSort(A, 0, n - 1);
//}

void qsortRecursive(Letter** mas, int size) {
	//Указатели в начало и в конец массива
	int i = 0;
	int j = size - 1;

	//Центральный элемент массива
	int mid = mas[size / 2]->getFrequency();

	//Делим массив
	do {
		//Пробегаем элементы, ищем те, которые нужно перекинуть в другую часть
		//В левой части массива пропускаем(оставляем на месте) элементы, которые меньше центрального
		while (mas[i]->getFrequency() < mid) {
			i++;
		}
		//В правой части пропускаем элементы, которые больше центрального
		while (mas[j]->getFrequency() > mid) {
			j--;
		}

		//Меняем элементы местами
		if (i <= j) {
			Letter* tmp = mas[i];
			mas[i] = mas[j];
			mas[j] = tmp;

			i++;
			j--;
		}
	} while (i <= j);


	//Рекурсивные вызовы, если осталось, что сортировать
	if (j > 0) {
		//"Левый кусок"
		qsortRecursive(mas, j + 1);
	}
	if (i < size) {
		//"Првый кусок"
		qsortRecursive(&mas[i], size - i);
	}
}
void Array::Sort() {
	qsortRecursive(this->array, size); //по возрастанию
	/*for (ulInt i = 0; i < size / 2; i++) {
		ulInt buf = array[i].getFrequency();
		array[i] = array[size - 1 - i];
		array[size - 1 - i] = buf;
	}*/
}


Array& Array::operator = (Array& other) {
	this->sum = other.getSum();
	this->sizeArr = other.getSizeArr();
	this->size = other.getSize();
	if (this->array) {
		for (ulInt i = 0; i < this->sizeArr; i++) delete array[i];
		delete[]array;
	}
	this->array = new Letter* [sizeArr];
	for (ulInt i = 0; i < sizeArr; i++) this->array[i] = new Letter(other.getArray()[i]);
	return *this;
}

Array::~Array() {
	for (int i = 0; i < sizeArr; i++) delete this->getArray()[i];
	delete[]this->getArray();
}
