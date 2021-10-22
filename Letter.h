#pragma once
typedef unsigned long long int ulInt;
class Letter
{
	ulInt frequency; //частота встречаемости буквы
	unsigned int sizeOfCode; //длина кодовой цепочки 
	ulInt code; //кодовая цепочка
public: 
	Letter();
	Letter(ulInt frequency);
	Letter(Letter* other);

	ulInt getFrequency();
	void setFrequency(ulInt frequency);

	unsigned int getSizeOfCode();
	void setSizeOfCode(unsigned int sizeOfCode);

	ulInt getCode();
	void setCode(ulInt code);
	
	Letter& operator = (Letter other);

	~Letter();
};

