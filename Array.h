#pragma once
#include "Letter.h"
#include <stdlib.h>
typedef unsigned long long int ulInt;
class Array
{
	Letter* array; //массив
	ulInt sizeArr; //размер массива
	ulInt size; //количество ненулевых элементов
	ulInt sum; //сумма элементов

public:

	Array();
	Array(ulInt sizeArr);
	Array(Letter* array, ulInt sizeArr, ulInt size);

	Letter* getArray();
	void setArray(Letter* array);

	ulInt getSize();
	void setSize(ulInt size);

	ulInt getSizeArr();
	void setSizeArr(ulInt sizeArr);

	ulInt getSum();
	void setSum(ulInt sum);

	void Sort();

	Array& operator = (Array other);


	~Array();
};

