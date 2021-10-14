#include "Letter.h"

Letter::Letter() {
	frequency = 0;
	sizeOfCode = 0;
	code = 0;
}
Letter::Letter(ulInt frequency) : Letter() {
	this->frequency = frequency;
}

ulInt Letter::getFrequency() {
	return frequency;
}
void Letter::setFrequency(ulInt frequency) {
	this->frequency = frequency;
}

unsigned int Letter::getSizeOfCode() {
	return sizeOfCode;
}
void Letter::setSizeOfCode(unsigned int sizeOfCode) {
	this->sizeOfCode = sizeOfCode;
}

ulInt Letter::getCode() {
	return code;
}
void Letter::setCode(ulInt code) {
	this->code = code;
}

Letter& Letter::operator = (Letter other) {
	this->frequency = other.getFrequency();
	this->sizeOfCode = other.getSizeOfCode();
	this->code = other.getCode();
	return *this;
}

Letter::~Letter(){}
