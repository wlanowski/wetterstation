/*
Name:		arduinodefs.h
Created:	11.02.2017 18:53:35
Author:		John

Definiert bestimmte Einstellungen und Hardwarepins, usw...
*/

#pragma once
/*===============
DHT-PIN für DHT Sensor
================*/
#define DEF_DHT_PIN 10

/*===============
//SD_PIN   (CS/Chipselect)
================*/
//53 für Mega, sonst 10 (bspws.)
//Jeweils SPI Einstellungen bei der Verdrahtung beachten. Mega != UNO!
#define DEF_SD_PIN 53

/*===============
	DATEINAME
================*/
//Dateiname für das Loggen der Sensordaten
#define DEF_DATEINAME "datalog.txt"

/*===============
	LUXMETER
================*/

#define LDR_DATAPIN 15 //Analog-Read-Pin!

/*
	In Anlehnung an https://www.allaboutcircuits.com/projects/design-a-luxmeter-using-a-light-dependent-resistor/
	sollten folgende Werte festgelegt werden:

	LDR_TEILERWIDERSTAND:	Fester Widerstand, der für den Spannungsteiler verwendet wird. (bspw. 5000 oder 10000 Ohm)
	LDR_EINGANGSSPANNUNG:	Eingangsspannung, die vom Board kommt (bspw. 5 oder 3.3 Volt);

	LDR_CALC_SCALAR:		Zur Berechnung benötigter Wert. Vgl. o.g. URL oder Installationsbeschreibung
	LDR_CALC_EXPONENT:		Zur Berechnung benötigter Wert. Vgl. o.g. URL oder Installationsbeschreibung

*/

#define LDR_TEILERWIDERSTAND	10000
#define LDR_EINGANGSSPANNUNG	5
#define LDR_CALC_SCALAR     7.80493E-08
#define LCR_CALC_EXPONENT   1.902123847


/*===============
	STATUSLED
================*/
//3-Farben-LED, die den Status der Wetterstattion anzeigen soll.
//Pins müssen PWM unterstützen!

#define STATUS_PINROT		2
#define STATUS_PINGRUN		3
#define STATUS_PINBLAU		4

// Farben werden hier bereits festgelegt
// 127 statt 255, für halbe statt voller Helligkeit
// Gemäß RGB-Farbmischung
#define STATUS_ROT	50, 0, 0
#define STATUS_GELB 50, 50, 0
#define STATUS_BLAU	0, 0, 50
#define STATUS_GRUN 0, 10, 0
#define STATUS_NULL 0, 0, 0