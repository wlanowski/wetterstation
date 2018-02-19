/*
 Name:		wetterstationarduino.ino
 Created:	11.02.2017 00:41:27
 Author:	John
*/

#define DEBUG
// Allgemeine Festlegungen für das Projekt (#defines)
#include "arduinodefs.h"

// Bibliotheken für RTC
// Quelle/Download: Adafruit https://github.com/adafruit/RTClib
#include <RTClib.h>
#include <Wire.h>		// für SPI (Bestandteil der Arduino-IDE)
RTC_DS3231 rtc;			// rtc ist Objekt der RTC_DS3231-Klasse
int nexttime;			// Beeinhaltet Zahl der Minute bei der die nächste Messung geloggt werden soll.


// Bibliotheken für SD
// Quelle: Standard-Libraries der Arduino-IDE)
#include <SPI.h>
#include <SD.h>
File myFile;


// Bibliothek für DHT
// Quelle/Download: Adafruit https://github.com/adafruit/DHT-sensor-library
#include <DHT.h>
DHT dht(DEF_DHT_PIN, DHT11); //DHT-11 kann bei anderem Typ bspw auch auf DHT22 geändert werden

// Bibliothek für BMP
// Quelle/Download: https://github.com/LowPowerLab/SFE_BMP180
#include <SFE_BMP180.h>
// #include <Wire.h>   //Wire.h wurde bereits beim RTC-Modul inkludiert und muss nicht nochmal eingefügt werden
SFE_BMP180 bmp;


void setStatus(int rot, int grun, int blau)
{
	//Färbt je nach Status die Status-LED in der entsprechenden Farbe
	analogWrite(STATUS_PINROT, rot);
	analogWrite(STATUS_PINGRUN, grun);
	analogWrite(STATUS_PINBLAU, blau);
}

void fehler(short code)
{
	/*Bei einem Fehler, beispielsweise bei der Initialisierung,
	//soll der Arduino gestoppt werden, und die eine LED zum leuchten gebracht werden
	
	1: SD-Fehler
	2: RTC-Fehler
	3: BMP-Fehler
	*/

	// Code kann zukünftig verwendet werden
	
	// LED ROT
	setStatus(STATUS_ROT);

	while (1)
	{
		;
	}
	return;
}

void init_sd() {

#ifdef DEBUG
	Serial.println("SD-Karte wird initialisiert.");
#endif // DEBUG


	// SD-Karte wird initialisiert. Bei Fehler wird in Fehler-Routine gegangen.
	if (!SD.begin(DEF_SD_PIN)) {
		Serial.println("SD-Karte konnte nicht initialisiert werden.");
		fehler(1);
		
		return;
	}

#ifdef DEBUG
	Serial.println("SD-Karte erfolgreich initialisiert.");
	Serial.println(" "); // Leerzeile für mehr Übersicht
#endif // DEBUG



}

void init_rtc() {
#ifdef DEBUG
	Serial.println("RTC wird initialisiert.");
#endif // DEBUG

	// RTC wird initialisiert. Bei Fehler wird in Fehler-Routine gegangen.
	if (!rtc.begin()) {
		Serial.println("RTC konnte nicht gefunden werden!");
		fehler(2);
		return;
	}

#ifdef DEBUG
	Serial.println("RTC erfolgreich initialisiert");
	Serial.println(" "); // Leerzeile für mehr Übersicht
#endif // DEBUG



	// Sollte keine RTC-Zeit gesetzt sein wird diese auf den Zeitpunkt des Kompilieren gesetzt.
	if (rtc.lostPower()) {
#ifdef DEBUG
		Serial.println("RTC-Zeit wird zurückgesetzt");
#endif // DEBUG
		rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
	}

}

void init_bmp() {
#ifdef DEBUG
	Serial.println("Initialisiere BMP-Sensor.");
#endif // DEBUG


	if (bmp.begin())
#ifdef DEBUG
		Serial.println("BMP-Sensor erfolgreich gestartet");
#endif // DEBUG

	else
	{
		Serial.println("BMP-Sensor konnte nicht initialisiert werden.");
		fehler(3);
	}
}

double get_pressure() {

		/*	Übernommen aus Beispiel der im Header verlinkten BMP-Bibliothek
			Angepasst an Anforderungen der Wetterstation, nötige Kommentare übersetzt
		*/


		char status;	// Status der Messungen
		double bmp_temp, bmp_pres, bmp_p0, bmp_a;


		// Es MUSS eine Temperaturmessung vor einer Druckmessung durchgeführt werden

		// Starte eine Temperaturmessung
		// erfolgreich: Rückgabe der Zeit in ms für das Durchführen der Messung
		// nicht erfolgreich: Rückgabe 0

		status = bmp.startTemperature();
		if (status != 0)
		{
			// Warte auf die Messung
			delay(status);

			// Empfange Temperaturmessung
			// Speicher in bmp_temp
			// Funktion antwortet bei Erfolg mit 1, sonst 0.

			status = bmp.getTemperature(bmp_temp);
			if (status != 0)
			{
				
				// Starte Druck-Messung
				// Parameter: 0 bis 3: Oversampling-Einstellung. (3= höchste Auflösung, längste Dauer)
				// erfolgreich: Rückgabe der Zeit in ms für das Durchführen der Messung
				// nicht erfolgreich: Rückgabe 0

				status = bmp.startPressure(3);
				if (status != 0)
				{
					// Warte auf die Messung
					delay(status);

					// Empfange Druckmessung
					// Speicher in bmp_pres
					// Funktion braucht vorangegangene Temperaturmessung (bmp_temp)
					// (If temperature is stable, you can do one temperature measurement for a number of pressure measurements.)
					// Function returns 1 if successful, 0 if failure.

					status = bmp.getPressure(bmp_pres, bmp_temp);
					if (status != 0)
					{
						return bmp_pres;
					}
					else Serial.println("Fehler beim Empfangen der Druckmessung");
				}
				else Serial.println("Fehler beim Start der Druckmessung");
			}
			else Serial.println("Fehler beim Empfangen der Temperaturmessung");
		}
		else Serial.println("Fehler beim Start der Temperaturmessung");

		return 0;
}

float get_lux()
{
	/*
	Gibt LUX zurück, Messung mit Photowiderstand (engl. akro: LDR)
	Genutzt wurde fertiges Modul mit bereits eingebautem Widerstand (10 kOhm)
	
	Bei Benutzung eines reinen LDR muss ein Widerstand zur Spannungsteilungberechnung verschaltet werden.
	Nähere Informationen rufen Sie bitte hier ab:
	https://www.allaboutcircuits.com/projects/design-a-luxmeter-using-a-light-dependent-resistor/
	
	Bitte beachten Sie in der arduinodefs.h eingetragene, von Sensor abhängige Daten!
	*/

	float rawData = analogRead(LDR_DATAPIN);							// Misst Digitalen Wert von 0 bis 1023 ein, abhängig von der Spannung
	float resistorVoltage = rawData / 1023 * LDR_EINGANGSSPANNUNG;		// Wandelt diesen Wert in eine Spannung in Volt um
	float ldrVoltage = LDR_EINGANGSSPANNUNG - resistorVoltage;			// Berechnung der Spannung über dem LDR (Spannungsteilerregel)
	float ldrResistance = ldrVoltage / resistorVoltage * LDR_TEILERWIDERSTAND;	// Berechnung des Widerstandes von LDR (Spannungsteilerregel)
	float ldrLux = LDR_CALC_SCALAR * pow(ldrResistance, LCR_CALC_EXPONENT);		// Umrechnung vom Widerstand in Lux-Zahl

	return ldrLux;

}

void check_file() {
	//überprüft nur ob Datei bereits vorhanden und erreichbar ist, sonst neu;

	if (SD.exists(DEF_DATEINAME)) {
		Serial.print(DEF_DATEINAME);
		Serial.println(" ist vorhanden.");
	}
	else {
		Serial.print(DEF_DATEINAME);
		Serial.println(" ist NICHT vorhanden und wird neu erstellt.");
		myFile = SD.open(DEF_DATEINAME, FILE_WRITE);
		myFile.close();

	}

}

void logtofile() {
	DateTime jetzt = rtc.now(); //brauchs noch mal.

	String str = ""; //Leerer String, der zusammengebaut wird um dann in die Datei angefügt zu werden.


	// Füge Datum in Format D-M-YYYY ein (keine führenden Nullen)
	str += jetzt.year();
	str += "-";
	str += jetzt.month();
	str += "-";
	str += jetzt.day();

	str += " ";	

	// Füge Zeit im Format H:M:S ein
	str += jetzt.hour();
	str += ":";
	str += jetzt.minute();
	str += ":";
	str += jetzt.second();

	// "," als Kategorietrenner

	str += ",";

	// Folgt: Temperatur
	str += dht.readTemperature();
	str += ",";

	// Folgt: rel. Luftfeuchtigkeit
	str += dht.readHumidity();
	str += ",";

	// Folgt: Luftdruck in hPa
	str += get_pressure();
	str += ",";

	// Folgt: Beleuchtungsstärke in Lux:
	str += get_lux();
	str += '\n'; //letzer Wert => neue Zeile




	//Öffne Datei...
	File log = SD.open(DEF_DATEINAME, FILE_WRITE);

	if (log) {
		log.print(str);		//...schreibe zusammengebauten String in die Datei...
		log.close();			//...und schließe die Datei danach wieder.

#ifdef DEBUG
		Serial.print(str);	//schreibt zusammengebauten String zusätzlich in den Seriellen Bus.
#endif // DEBUG

	}

	// if the file isn't open, pop up an error:
	else {
		Serial.println("Datei konnte nicht geöffnet werden!");
	}

}

void dumptoserial1()
{

	//Die Datei wird gedumpt an Serial1 (Bluetooth) übergeben;

#ifdef DEBUG
	Serial.println("Starte Dateidump");
	Serial.println("S");
#endif // DEBUG

myFile = SD.open(DEF_DATEINAME);

if (myFile) {

	Serial1.println("S");

	// read from the file until there's nothing else in it:
	while (myFile.available()) {
		Serial1.write(myFile.read());
	}
	// close the file:
	myFile.close();
}
else {
	// if the file didn't open, print an error:
	Serial1.println("error opening test.txt");
}
//Serial1.println(""); // Zeilen Abschluss, dediziertes Ende;

Serial1.println("E");
#ifdef DEBUG
Serial.println("E");
#endif // DEBUG

}

void setup() {

	// following line sets the RTC to the date & time this sketch was compiled
	
		
	// Setzt Pins für Status-Led auf OUTPUT
	pinMode(STATUS_PINROT, OUTPUT);
	pinMode(STATUS_PINGRUN, OUTPUT);
	pinMode(STATUS_PINBLAU, OUTPUT);

	// LED GELB (Initialisierung)
	setStatus(STATUS_GELB);
	
	//Für den LDR: Eingangsspannung kommt von Digitalpin 48:
	pinMode(48, OUTPUT);
	digitalWrite(48, HIGH);

	Serial.begin(9600);		//Kommunikation über RX0/TX0 (USB (DEBUG))
	Serial1.begin(9600);	//Kommunikation über RX1/TX1 (Bluetooth)

	init_sd();				//Initialisiere SD

	init_rtc();				//Initialisiere RTC


	init_bmp();				//Initialisiere BMP

	dht.begin();			//Starte DHT-Sensor



#ifdef DEBUG
	check_file();			//Überprüft ob DATEINAME bereits auf der SD-Karte vorhanden ist. Nur zur Information. (DEBUG)
#endif // DEBUG

	//Setze nexttime auf aktuelle Minute
	DateTime jetzt = rtc.now();
	nexttime = jetzt.hour();



}


void loop() {
	
	//In Loop: Betrieb => LED grün
	setStatus(STATUS_GRUN);

	DateTime jetzt = rtc.now();

	if (nexttime == jetzt.hour())
	{
		logtofile();
		nexttime++;
		if (nexttime == 24)
		{
			nexttime = 0;
		}
	}
	
	if (Serial1.available() > 0)
	{	
		
		char ser1char = Serial1.read();
		
		// Bei Verbindung über BT: LED blau;
		setStatus(STATUS_BLAU);



		if (ser1char == 'W')			// W ist das Zeichen dafür, dass auf der anderen Seite der Dateidump erwartet wird.
		{
			dumptoserial1();
		}
		else {
			if (ser1char == 'L')			// L ist das Zeichen dafür, dass der Arduino die csv-Datei leeren soll
			{
				Serial.println("Removing example.txt...");
				SD.remove(DEF_DATEINAME);

				if (SD.exists(DEF_DATEINAME)) {
					Serial.println("Die Datei exisiert noch. (FEHLER!!)");
				}
				else {
					Serial.println("Datei erfolgreich entfernt");
					Serial1.println("F");
				}

				check_file(); // Damit der Arduino weiter machen kann, braucht er ja eine Datei;
				check_file();
			}
		}
	}

	
}
