



/*
	Coded by Lok 2023
	что имеем Сейчас
	- гибкая настройка модуля из программы
	- передача любого Stream объекта в класс
	- какое никакое энергосбережение в режиме симплекса
	- симплексный режим работы UART-моста(в разработке, но уже есть пара методов)
	- отчет об успешности выполнения настройки(ТОЛЬКО в режиме настройки через АТ): отправили команду - получаем данные с UART - и понимаем получили или нет
	- проверка наличия модуля на шине
	что имеем В будующем
	- наследование класса print
	- получение всех настроек с модуля
	- быстрая синхронизация двух систем по проводу(например вемос в связке с jdi), я еще не придумал поэтому не могу описать.
	
	
	Буду рад вашим дополнениям к библиотеке, а особенно дополнениям к оптимизации либы
	и алгоритмам парсинга ответов на запросы установленных параметров с модуля.
	Если есть какие-то идеи и дополнения к проекту прошу писать на lokkiki343@gmail.com
	
	
	ver 1.0 -> создание и небольшой тест.
*/
#pragma once
#include <Arduino.h>


#define _SIMPLEX 0
#define _DUPLEX 1




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
		_err = 1;
		static uint16_t tmr;
		tmr = millis();
		while (millis() - tmr < 100) {
			if (serial.available() != 0) {
				if (serial.find("Wake") || serial.find("START")) {
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
		static bool yes;
		yes = 1;
		if (!setRfID(rfid)) yes = 0;
		if (!setDevID(dvid)) yes = 0;
		if (!setChannel(rfc)) yes = 0;
		if (!setPower(power)) yes = 0;
		if (!setBaud(baud)) yes = 0;
		if (_mode == _SIMPLEX) {
			off();
		}
		debug(0);
		return yes;
	}
	
	bool setBaud(uint8_t baud) {
		on();
		debug(1);
		static bool yes;
		yes = 1;
		static String str;
		str = "";
		str += "AT+";
		str += "BAUD";
		str += String(constrain(baud, 1, 6));
		str += "\r\n";
		if (!sendData(str)) yes = 0;
		if (_mode == _SIMPLEX) {
			off();
		}
		debug(0);
		return yes;
	}
	
	bool setDevID(uint16_t dvid) {
		on();
		debug(1);
		static bool yes;
		yes = 1;
		static String str;
		str = "";
		str += "AT+";
		str += "DVID";
		str += String(constrain(dvid, 1000, 9999));
		str += "\r\n";
		if (!sendData(str)) yes = 0;
		if (_mode == _SIMPLEX) {
			off();
		}
		debug(0);
		return yes;
	}
	
	bool setRfID(uint16_t rfid) {
		on();
		debug(1);
		static bool yes;
		yes = 1;
		static String str;
		str = "";
		str += "AT+";
		str += "RFID";
		str += String(constrain(rfid, 1000, 9999));
		str += "\r\n";
		if (!sendData(str)) yes = 0;
		
		if (_mode == _SIMPLEX) {
			off();
		}
		debug(0);
		return yes;
	}
	
	bool setChannel(uint8_t rfc) {
		on();
		debug(1);
		static bool yes;
		yes = 1;
		static String str;
		str = "";
		str += "AT+";
		str += "RFC";
		switch (constrain(rfc, 0, 127)) {
			case 0 ... 9:
			str += "00";
			str += constrain(rfc, 0, 127);
			break;
			case 10 ... 99:
			str += "0";
			str += constrain(rfc, 0, 127);
			break; 
			case 100 ... 127:
			str += constrain(rfc, 0, 127);
			break;
		}
		str += "\r\n";
		if (!sendData(str)) yes = 0;
		
		if (_mode == _SIMPLEX) {
			off();
		}
		debug(0);
		return yes;
	}
	
	bool setPower(uint8_t power) {
		on();
		debug(1);
		static bool yes;
		yes = 1;
		static String str;
		str = "";
		str += "AT+";
		str += "POWE";
		str += String(constrain(power, 0, 9));
		str += "\r\n";
		if (!sendData(str)) yes = 0;
		
		if (_mode == _SIMPLEX) {
			off();
		}
		debug(0);
		return yes;
	}
	//Отправка в симплексе
	void print(String str) {
		on();
		delay(50);
		serial.print(str);
		if (_mode == _SIMPLEX) {
			off();
		}
	}
	void print(const char* str) {
		on();
		delay(50);
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
	bool _err;
	bool _ok;
	
	void are_you_ok() {
		static uint16_t tmr;
		tmr = millis();
		while (millis() - tmr < 100) {
			if (serial.available() != 0) {
				if (serial.find("OK")) {
					_err = 0;
					break;
				}
			}
		}
	}
	
	void debug(bool state) {
		digitalWrite(_set, !state);
		delay(15);
	}
	
	bool sendData(String str) {
		delay(15);
		_err = 1;
		serial.print(str);
		are_you_ok();
		delay(15);
		return !_err; 
	}
};																																																																																																																						