#include "JDY-40.h"


JDY40::JDY40(Stream* s):serial(s) {_send.reserve(15);} //конструктор


void JDY40::configDevice(bool mode, uint8_t cs, uint8_t set) {  //конфигурация устройства
	_mode = mode;
	_cs = cs;
	_set = set;
	pinMode(_cs, OUTPUT);
	pinMode(_set, OUTPUT);
	if (_mode == _SIMPLEX) {off();} else {on();}
	digitalWrite(_set, 1);
}



bool JDY40::begin() {  //проверка наличия модуля
	off();
	delay(5);
	on();
	_err = 1;
	static uint16_t tmr;
	tmr = millis();
	while (millis() - tmr < 100) {
		if (serial->available() != 0) {
			if (serial->find("Wake") || serial->find("START")) {
				_err = 0;
				break;
			}
		}
	}
	if (_mode == _SIMPLEX) {
		off();
		} else {
		on();
	}
	return !_err;
}


///////////////////////////////////////////////различные настройки//////////////////////////////

void JDY40::on() {
	digitalWrite(_cs, 0);
	_on = 1;
}
void JDY40::off() {
	digitalWrite(_cs, 1);
	_on = 0;
}
bool JDY40::setAllSettings(uint8_t baud, uint16_t rfid, uint16_t dvid, uint8_t rfc, uint8_t power) {
	on();
	settings(1);
	yes = 1;
	if (!setRfID(rfid)) yes = 0;
	if (!setDevID(dvid)) yes = 0;
	if (!setChannel(rfc)) yes = 0;
	if (!setPower(power)) yes = 0;
	if (!setBaud(baud)) yes = 0;
	if (_mode == _SIMPLEX) {
		off();
	}
	settings(0);
	return yes;
}
bool JDY40::setBaud(uint8_t baud) {
	on();
	settings(1);
	yes = 1;
	_send = "";
	_send += "AT+";
	_send += "BAUD";
	_send += String(constrain(baud, 1, 6));
	_send += "\r\n";
	if (!sendData()) yes = 0;
	if (_mode == _SIMPLEX) {
		off();
	}
	settings(0);
	return yes;
}
bool JDY40::setDevID(uint16_t dvid) {
	on();
	settings(1);
	yes = 1;
	_send = "";
	_send += "AT+";
	_send += "DVID";
	_send += String(constrain(dvid, 1000, 9999));
	_send += "\r\n";
	if (!sendData()) yes = 0;
	if (_mode == _SIMPLEX) {
		off();
	}
	settings(0);
	return yes;
}
bool JDY40::setRfID(uint16_t rfid) {
	on();
	settings(1);
	yes = 1;
	_send = "";
	_send += "AT+";
	_send += "RFID";
	_send += String(constrain(rfid, 1000, 9999));
	_send += "\r\n";
	if (!sendData()) yes = 0;
	
	if (_mode == _SIMPLEX) {
		off();
	}
	settings(0);
	return yes;
}
bool JDY40::setChannel(uint8_t rfc) {
	on();
	settings(1);
	yes = 1;
	_send = "";
	_send += "AT+";
	_send += "RFC";
	switch (constrain(rfc, 0, 127)) {
		case 0 ... 9:
		_send += "00";
		_send += constrain(rfc, 0, 127);
		break;
		case 10 ... 99:
		_send += "0";
		_send += constrain(rfc, 0, 127);
		break; 
		case 100 ... 127:
		_send += constrain(rfc, 0, 127);
		break;
	}
	_send += "\r\n";
	if (!sendData()) yes = 0;
	
	if (_mode == _SIMPLEX) {
		off();
	}
	settings(0);
	return yes;
}
bool JDY40::setPower(uint8_t power) {
	on();
	settings(1);
	yes = 1;
	_send = "";
	_send += "AT+";
	_send += "POWE";
	_send += String(constrain(power, 0, 9));
	_send += "\r\n";
	if (!sendData()) yes = 0;
	
	if (_mode == _SIMPLEX) {
		off();
	}
	settings(0);
	return yes;
}



/////////////////////////////////////////Принты для симплекса////////////////////////////////
void JDY40::print(String str) {
	on();
	delay(50);
	serial->print(str);
	if (_mode == _SIMPLEX) {
		off();
	}
}
void JDY40::print(const char* str) {
	on();
	delay(50);
	serial->print(str);
	if (_mode == _SIMPLEX) {
		off();
	}
}




////////////////////////////////////////////Приваты////////////////////////////////////////////
void JDY40::are_you_ok() {
	static uint16_t tmr;
	tmr = millis();
	while (millis() - tmr < 100) {
		if (serial->available() != 0) {
			if (serial->find("OK")) {
				_err = 0;
				break;
			}
		}
	}
}
void JDY40::settings(bool state) {
	digitalWrite(_set, !state);
	delay(15);
}
bool JDY40::sendData() {
	delay(15);
	_err = 1;
	serial->print(_send);
	are_you_ok();
	delay(15);
	return !_err; 
}