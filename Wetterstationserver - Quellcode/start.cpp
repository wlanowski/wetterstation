/*
Name:		start.cpp
Created:	26.01.2017 14:44:01
Author:		MySQL

Start/Main Programm für den Server
*/
//Schalte Sicherheitswarnungen aus
#define _CRT_SECURE_NO_WARNINGS


//Einstellungen für COM-Ports und MySQL
#include "definitionen.h"

// & Library für COM-Kommunikation
#include <boost/asio.hpp>
#include "serial.h"

// Standardbibliotheken zum Arbeiten
#include <stdlib.h>
#include <iostream>
#include <string>

#define WIN32_LEAN_AND_MEAN  //verhindert Komplikationen zwischen windows.h-APIs und BOOST
#include <windows.h>

//Library für mySQL
#include "mysql.h"

//Library für das Parsen von CSV-Dateien
#include "csv.h"

using namespace std;


string aktuellerpfadini()
{
	//Aktueller Pfad wo sich die INI befindet (benötigt für das Öffnen des INI-Files, wo Einstellungen getätigt werden können/sollen)

	char result[MAX_PATH];
	std::string sresult(result, GetModuleFileName(NULL, result, MAX_PATH));

	size_t found = sresult.rfind("\\");
	sresult.replace(found, sresult.length() - found, "\\wetter.ini");

	return sresult;
}

string erstellequeryinsert(string datum, string temp, string hum, string pres, string bright)
{
	// Erstellt aus übergebenen Daten eine Abfrage zum Upload in die Datenbank 
	// Soll Rückgabe soll so aussehen. (BSP)
	// INSERT INTO johnsdummy (DATE, TEMP, HUM, PRES, BRIGHT) VALUES ("2017-01-29 16:48:00","25","76","1168","1235")
	
	// Lese Daten aus INI
	const unsigned long puffersize = 255;
	char db_tablename[puffersize];
	GetPrivateProfileString("wetter","db_tablename",er_db_tablename,db_tablename,puffersize,aktuellerpfadini().c_str());
	// ENDE Lese Daten aus INI
	string str = "";
	str = str + "INSERT INTO " + db_tablename + "(DATE, TEMP, HUM, PRES, BRIGHT) VALUES('" + datum  + "','" + temp + "','" + hum + "','" + pres + "','" + bright + "')";
	return str;
}



void holedatenab()
{
	// Erstelle und Öffne Datei zum Zwischenspeichern

	FILE * csvdatei;
	csvdatei = fopen(CSV_DATEINAME, "a+");

	// ENDE Erstelle und Öffne Datei zum Zwischenspeichern

	// Lese Daten aus INI
	const unsigned long puffersize = 255;
	char COM_PORT[puffersize];
	GetPrivateProfileString("wetter", "COM_PORT", er_COM_PORT, COM_PORT, puffersize, aktuellerpfadini().c_str());

	int COM_BAUDRATE = GetPrivateProfileInt("wetter", "COM_BAUDRATE", er_COM_BAUDRATE, aktuellerpfadini().c_str());
	// ENDE Lese Daten aus INI

	cout << "Verbinde mit Arduino." << endl;
	// Versuche, mit Arduino zu verbinden.
	try
	{
		SimpleSerial arduino(COM_PORT, COM_BAUDRATE);
		
		// Sende W um Arduino bescheid zusagen, dass wir auf DUMP warten.
		cout << "Schreibe W" << endl;
		arduino.writeString("W");
	
		if (arduino.readLine() == "S")  // Kommando, dass es losgeht
		{
			
			cout << "Daten werden abgerufen..." << endl;
			string empfang = "";
			do
			{	
				empfang = arduino.readLine();
				if (empfang != "E") {							// "ENDE" soll nicht mit in Datei geschrieben werden
					fprintf(csvdatei, empfang.c_str());				// Schreibe empfangene Daten in Datei
					fprintf(csvdatei, "\n");						// schließe mit Zeilenumbruch ab.
				}
				
			} while (empfang != "E");  // Kommando, dass Arduino fertig ist. 

		}



		//Schließe Datei wieder
		fclose(csvdatei);
		cout << "Daten wurden erfolgreich abgerufen." << endl;

	}
	catch (boost::system::system_error& e)
	{
		cout << "Fehler beim Verbindungsaufbau mit dem Arduino: " << e.what() << endl;
		exit(1);
		return;
		
	}
	// ENDE Empfange und Speicher Daten vom Arduino
}

void ladedatenhoch()
{
	// Bevor Daten hochgeladen werden können, muss CSV zu SQL-Befehlen gewandelt werden.
	// Die gewandelteten Befehle werden dann ausgeführt

	io::CSVReader<5> in(CSV_DATEINAME);  // 5 Spalten
	
	//in.read_header(io::ignore_extra_column, "csv_DATE","csv_TEMP", "csv_HUM", "csv_PRES", "csv_BRIGHT");
	
		string csv_DATE;
		string csv_TEMP;
		string csv_HUM;
		string csv_PRES;
		string csv_BRIGHT;
	
		cout << "Daten werden hochgeladen";
		int zahl = 0;
	while (in.read_row(csv_DATE, csv_TEMP, csv_HUM, csv_PRES, csv_BRIGHT))
	{
		zahl++;
		cout << "\nDatensatz: " << zahl << '\t';
		string execute = erstellequeryinsert(csv_DATE, csv_TEMP, csv_HUM, csv_PRES, csv_BRIGHT);
		queryexe(execute);
	}
	
}

void offnedaten()
{
	/* TODO */
}

void leerearduino()
{
	// Lese Daten aus INI
	const unsigned long puffersize = 255;
	char COM_PORT[puffersize];
	GetPrivateProfileString("wetter", "COM_PORT", er_COM_PORT, COM_PORT, puffersize, aktuellerpfadini().c_str());

	int COM_BAUDRATE = GetPrivateProfileInt("wetter", "COM_BAUDRATE", er_COM_BAUDRATE, aktuellerpfadini().c_str());
	// ENDE Lese Daten aus INI


	// Versuche, mit Arduino zu verbinden.
	try
	{
		SimpleSerial arduino(COM_PORT, COM_BAUDRATE);

		// Sende W um Arduino bescheid zusagen, dass wir auf DUMP warten.
		arduino.writeString("L");
		cout << "Entferne CSV-Datei auf Arduino..." << endl;
		while (arduino.readLine() != "F")  //Warte darauf, dass Arduino geleert ist.
		{
			;
		}

		cout << "Arduino erfolgreich geleert." << endl;

	}
	catch (boost::system::system_error& e)
	{
		cout << "Fehler beim Verbindungsaufbau mit dem Arduino: " << e.what() << endl;
		exit(1);
	}
	
}
int main(int argc, char *argv[])
{

	// Lese Daten aus INI
	const unsigned long puffersize = 255;
	char COM_PORT[puffersize];
	GetPrivateProfileString("wetter", "COM_PORT", er_COM_PORT, COM_PORT, puffersize, aktuellerpfadini().c_str());

	int COM_BAUDRATE=GetPrivateProfileInt("wetter", "COM_BAUDRATE", er_COM_BAUDRATE, aktuellerpfadini().c_str());



	if (argc == 1)
	{
		cout << "Tool zur Verwaltung der Arduino-Wetterstation" << endl << endl;
		cout << "WETTERSTATION [[A], [H], [O], [U], [L]]" << endl << endl;
		cout << '\t' << "A\t Holt Daten vom Arduino ab und lädt diese in die MySQL-Datenbank hoch, und leert Arduino und abgeholte Daten" << endl;
		cout << '\t' << "H\t Holt Daten vom Arduino ab." << endl;
		cout << '\t' << "O\t Öffnet abgeholte Daten." << endl;
		cout << '\t' << "U\t Lädt abgeholte Daten in die MySQL-Datenbank hoch." << endl;
		//cout << '\t' << "C\t Konfiguriert das Programm (COM-Port und MySQL)" << endl;
		return 0;
	}

	switch (*argv[1])
	{
	case 'A':holedatenab(); ladedatenhoch(); /* leerearduino(); */ break;
	case 'H':holedatenab(); break;
	case 'O':offnedaten(); break;
	case 'U':ladedatenhoch(); break;
	case 'L':leerearduino(); break;

	case 'a':holedatenab(); ladedatenhoch(); /* leerearduino(); */ break;
	case 'h':holedatenab(); break;
	case 'o':offnedaten(); break;
	case 'u':ladedatenhoch(); break;
	case 'l':leerearduino(); break;

	default: cout << "Fehler: Unbekannter Befehl!"; return 1; break;

	}

}