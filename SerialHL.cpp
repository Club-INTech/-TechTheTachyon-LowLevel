/**
*
*Classe g�rant la communication avec le Haut Niveau(messages arrivant, sortant)
*
*ASCII:13 = CR, 10 = LF
*
*/

#include "SerialHL.h"

SerialHL::SerialHL()
{
}

bool inline SerialHL::read_char(char & byte)
{
	byte = Serial.read();
	return (byte != '\r');
}

bool SerialHL::read(String& order)
{
	order = "";
	char buffer[64] = "";
	char incomingChar;
	uint8_t i = 0;

	while (!available()) {}	//On attend l'entr�e des commandes

	if (available() > 0) {					//Si il y a quelque chose � lire dans le port s�rie
		while (read_char(incomingChar) && i < RX_BUFFER_SIZE) {	//Tant qu'on n'est pas � la fin d'un message(\r)
			buffer[i]=incomingChar;
			i++;												//Au cas o� on ne re�oit jamais de terminaison
		}
		read_char(incomingChar);								//On �limine le \n
	}
	order.append(buffer);

	return (!order.equals(""));
}

bool SerialHL::read(int16_t & value)
{
	String readValue = "";

	bool status = read(readValue);

	value = strtol(readValue.c_str(), nullptr, DEC);

	return status;
}

bool SerialHL::read(volatile int8_t & value)
{
	String readValue = "";

	bool status = read(readValue);

	value = strtol(readValue.c_str(), nullptr, DEC);

	return status;
}

bool SerialHL::read(float& value) {
	String readValue = "";

	bool status = read(readValue);

	value = strtof(readValue.c_str(), nullptr);
	
	return status;
}

uint8_t inline SerialHL::available()
{
	return Serial.available();
}

void SerialHL::sendUS(uint16_t value)
{
	String data = "";
	char header[HEADER_LENGTH] = US_HEADER;

	for (int i = 0; i < HEADER_LENGTH;i++) {
		data.append(header[i]);
	}

	data.append(value);

	Serial.println(data);
}

void SerialHL::printfln(const char* message, ...) {
	va_list args;										//Variable contenant la liste des arguments apr�s log (...)
	va_start(args, message);

	char logToSend[64];

	vsnprintf(logToSend, 64, message, args);			//Ajoute dans logToSend de log, en formattant avec les arguments

	Serial.println(logToSend);

	va_end(args);
}

void SerialHL::log(const char* log, ...) {
	char data[HEADER_LENGTH + 64] = DEBUG_HEADER;
	data[HEADER_LENGTH] = '\0';

	strcat(data, log);

	va_list args;								//Variable contenant la liste des arguments apr�s log (...)
	va_start(args, log);

	char logToSend[HEADER_LENGTH+64];
	vsnprintf(logToSend, 64, data, args);			//Ajoute dans le buffer log, en formattant les 
	Serial.println(logToSend);

	va_end(args);
}