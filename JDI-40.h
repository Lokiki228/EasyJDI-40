



/*
	Coded by Lok 2023
	
	
	
	
	
	
*/
#pragma once
#include <Arduino.h>


#define _SIMPLEX 0
#define _DUPLEX 1


#define _IF_ON "Wake\r\n"
#define _IF_SET "OK\r\n"




class JDI40 {
	public://Инициализация
	JDI40(Stream& s):serial(s) {}
	
	void configDevice(bool mode, uint8_t cs, uint8_t set) {
		_mode = mode;
		_cs = cs;
		_set = set;
		pinMode(_cs, OUTPUT);
		pinMode(_set, OUTPUT);
		if (_mode == _SIMPLEX) {off();} else {on();}
		digitalWrite(_set, 1);
	}
	
	bool begin() {
		off();
		delay(5);
		on();
		delay(5);
		static char in[4];
		for (int i = 0; serial.available() > 0 && i < 4; i++) {
			in[i] = serial.read();
		}
		if ((strcmp(in, "Wake\r\n")) || (strcmp(in, "START\r\n"))) {
			if (_mode == _SIMPLEX) {off();} else {on();}
			return 1;
		} else {if (_mode == _SIMPLEX) {off();} else {on();} return 0;}
	}
	
	void on() {
		digitalWrite(_cs, 0);
		_on = 1;
	}
	
	void off() {
		digitalWrite(_cs, 1);
		_on = 0;
	}
	
	bool setAllSettings(uint8_t baud, uint16_t rfid, uint16_t dvid, uint8_t rfc, uint8_t power) {
		on();
		debug(1);
		setBaud(baud);
		setID(rfid, dvid);
		setChannel(rfc);
		setPower(power);
		if (_mode == _SIMPLEX) {
			off();
		}
		debug(0);
		return _ok;
	}
	
	bool setBaud(uint8_t baud) {
		on();
		debug(1);
		
		static String str;
		str = "";
		str += "AT+";
		str += "BAUD";
		str += String(constrain(baud, 1, 6));
		str += "\r\n";
		sendData(str);
		
		if (_mode == _SIMPLEX) {
			off();
		}
		debug(0);
		return _ok;
	}
	
	bool setID(uint16_t rfid, uint16_t dvid) {
		on();
		debug(1);
		
		static String str;
		str = "";
		str += "AT+";
		str += "RFID";
		str += String(constrain(rfid, 1000, 9999));
		str += "\r\n";
		sendData(str);
		
		str = "";
		str += "AT+";
		str += "DVID";
		str += String(constrain(dvid, 1000, 9999));
		str += "\r\n";
		sendData(str);
		
		if (_mode == _SIMPLEX) {
			off();
		}
		debug(0);
		return _ok;
	}
	
	bool setChannel(uint8_t rfc) {
		on();
		debug(1);
		
		static uint8_t _rfc;
		_rfc = constrain(rfc, 0, 127);
		static String str;
		str = "";
		str += "AT+";
		str += "RFC";
		if (rfc < 9) {
			str += "00";
			str += String(_rfc);
			} else if (rfc < 99) {
			str += "0";
			str += String(_rfc);
		} else {str += String(_rfc);}
		str += "\r\n";
		sendData(str);
		
		if (_mode == _SIMPLEX) {
			off();
		}
		debug(0);
		return _ok;
	}
	
	bool setPower(uint8_t power) {
		on();
		debug(1);
		
		static String str;
		str = "";
		str += "AT+";
		str += "POWE";
		str += String(constrain(power, 0, 9));
		str += "\r\n";
		sendData(str);
		
		if (_mode == _SIMPLEX) {
			off();
		}
		debug(0);
		return _ok;
	}
	//Отправка в симплексе
	void print(String str) {
		on();
		delay(5);
		serial.print(str);
		if (_mode == _SIMPLEX) {
			off();
		}
	}
	void print(const char* str) {
		on();
		delay(5);
		serial.print(str);
		if (_mode == _SIMPLEX) {
			off();
		}
	}
	//coming soon... More, MORE!
	
	
	
	
	
	private:
	Stream& serial;
	bool _mode = 0;
	uint8_t _cs;
	uint8_t _set;
	bool _on;
	bool _ok = 1;
	
	bool are_you_ok() {
		_ok = 1;
		static char in[4];
		for (int i = 0; serial.available() > 0 && i < 4; i++) {
			in[i] = serial.read();
		}
		if (strcmp(in, "OK\r\n")) {
			_ok = 1;
			return 1;
		} else {_ok = 0; return 0;}
	}
	
	void debug(bool state) {
		digitalWrite(_set, !state);
		delay(5);
	}
	
	void sendData(String str) {
		serial.print(str);
		are_you_ok();
	}
};