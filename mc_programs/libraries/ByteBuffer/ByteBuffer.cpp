/*
Arduino Buffered Serial
A library that helps establish buffered serial communication with a 
host application.
Copyright (C) 2010 Sigurður Örn Aðalgeirsson (siggi@media.mit.edu)

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/
 
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "ByteBuffer.h"


ByteBuffer::ByteBuffer(){

}

void ByteBuffer::init(unsigned int buf_length){
	data = (byte*)malloc(sizeof(byte)*buf_length);
	capacity = buf_length;
	position = 0;
	length = 0;
}

void ByteBuffer::deAllocate(){
	free(data);
}

void ByteBuffer::clear(){
	position = 0;
	length = 0;
}

int ByteBuffer::getSize(){
	return length;
}

int ByteBuffer::getCapacity(){
	return capacity;
}

int ByteBuffer::getPosition(){
	return position;
}

byte ByteBuffer::peek(unsigned int index){
	byte b = data[(position+index)%capacity];
	return b;
}

bool ByteBuffer::arrayPeek(uint16_t index, byte &b){
	if (index < capacity){
		b = data[index];  // potentially dangerous function
		return true;
	}
	else{
		return false;
	}
}

int ByteBuffer::put(byte in){
	if(length < capacity){
		// save data byte at end of buffer
		data[(position+length) % capacity] = in;
		// increment the length
		length++;
		return 1;
	}
	else if (length == capacity){
		data[position] = in;
		position = (position + 1) % capacity;
		return 1;
	}
	// return failure
	return 0;
}

int ByteBuffer::putInFront(byte in){
	if(length < capacity){
		// save data byte at end of buffer
		if( position == 0 )
			position = capacity-1;
		else
			position = (position-1)%capacity;
		data[position] = in;
		// increment the length
		length++;
		return 1;
	}
	// return failure
	return 0;
}

byte ByteBuffer::get(){
	byte b = 0;


	if(length > 0){
		b = data[position];
		// move index down and decrement length
		position = (position + 1) % capacity;
		length--;
	}

	return b;
}

byte ByteBuffer::getFromBack(){
	byte b = 0;
	if(length > 0){
		b = data[(position+length-1)%capacity];
		length--;
	}

	return b;
}

//
// Ints
//

int ByteBuffer::putIntInFront(int in){
    byte *pointer = (byte *)&in;
	if (put(pointer[0]) && put(pointer[1])){
		return 1;
	}
	return 0;
	// putInFront(pointer[0]);	
	// putInFront(pointer[1]);	
}

int ByteBuffer::putInt(int in){
    byte *pointer = (byte *)&in;
	if (put(pointer[1]) && put(pointer[0])){
		return 1;
	}
	return 0;
	// put(pointer[1]);	
	// put(pointer[0]);	
}


int ByteBuffer::getInt(){
	int ret;
    byte *pointer = (byte *)&ret;
	pointer[1] = get();
	pointer[0] = get();
	return ret;
}

int ByteBuffer::getIntFromBack(){
	int ret;
    byte *pointer = (byte *)&ret;
	pointer[0] = getFromBack();
	pointer[1] = getFromBack();
	return ret;
}

//
// Longs
//

int ByteBuffer::putLongInFront(long in){
    byte *pointer = (byte *)&in;
	if (put(pointer[0]) && put(pointer[1]) && put(pointer[2]) && put(pointer[3])){
		return 1;
	}
	return 0;
	// putInFront(pointer[0]);	
	// putInFront(pointer[1]);	
	// putInFront(pointer[2]);	
	// putInFront(pointer[3]);	
}

int ByteBuffer::putLong(long in){
    byte *pointer = (byte *)&in;
	if (put(pointer[3]) && put(pointer[2]) && put(pointer[1]) && put(pointer[0])){
		return 1;
	}
	return 0;
	// put(pointer[3]);	
	// put(pointer[2]);	
	// put(pointer[1]);	
	// put(pointer[0]);	
}


long ByteBuffer::getLong(){
	long ret;
    byte *pointer = (byte *)&ret;
	pointer[3] = get();
	pointer[2] = get();
	pointer[1] = get();
	pointer[0] = get();
	return ret;
}

long ByteBuffer::getLongFromBack(){
	long ret;
    byte *pointer = (byte *)&ret;
	pointer[0] = getFromBack();
	pointer[1] = getFromBack();
	pointer[2] = getFromBack();
	pointer[3] = getFromBack();
	return ret;
}


//
// Floats
//

int ByteBuffer::putFloatInFront(float in){
    byte *pointer = (byte *)&in;
	if (put(pointer[0]) && put(pointer[1]) && put(pointer[2]) && put(pointer[3])){
		return 1;
	}
	return 0;
	// putInFront(pointer[0]);	
	// putInFront(pointer[1]);	
	// putInFront(pointer[2]);	
	// putInFront(pointer[3]);	
}

int ByteBuffer::putFloat(float in){
    byte *pointer = (byte *)&in;
	
	if (put(pointer[3]) && put(pointer[2]) && put(pointer[1]) && put(pointer[0])){
		return 1;
	}
	return 0;
	// put(pointer[3]);	
	// put(pointer[2]);	
	// put(pointer[1]);	
	// put(pointer[0]);	
}

float ByteBuffer::getFloat(){
	float ret;
    byte *pointer = (byte *)&ret;
	pointer[3] = get();
	pointer[2] = get();
	pointer[1] = get();
	pointer[0] = get();
	return ret;
}

float ByteBuffer::getFloatFromBack(){
	float ret;
    byte *pointer = (byte *)&ret;
	pointer[0] = getFromBack();
	pointer[1] = getFromBack();
	pointer[2] = getFromBack();
	pointer[3] = getFromBack();
	return ret;
}
bool ByteBuffer::searchString(const char *sfind, byte len){ // byte sfind[]
	uint8_t i = 0;
	uint8_t j = 0;
	
	if ((len > 0) && (len < length)){
		while (i < length){
			if (data[(position + i) % capacity] == sfind[j]){
				j++;
				if (len == j){
					return true;  // return end of string location
				}
			}
			else{
				j = 0;
			}
			i++;
		}
	}
	else if (len == length){
		for (i = 0; i < len; i++){
			if (!(sfind[i] == data[(position + i) % capacity])){
				return false;  // escape once anything doesn't match
			}
		}
		
		return true;  // return end of string
	}
	
	return false;  // return invalid number for comparison
}

