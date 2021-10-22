#pragma once
typedef unsigned long long int ulInt;
class Letter
{
	ulInt frequency; //������� ������������� �����
	unsigned int sizeOfCode; //����� ������� ������� 
	ulInt code; //������� �������
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

