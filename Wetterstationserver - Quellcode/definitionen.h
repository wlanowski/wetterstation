/*
Name:		definitionen.h
Created:	09.02.2017 14:45:21
Author:		John


Headerdatei, die globale Einstellungen für MySQL und COM in
einer Datei zentralisiert.

Die hier genannten Einstellungen werden benutzt, wenn das INI-File
nicht geöffnet werden konnte oder die jeweilige Variable
nicht geparst werden konnte

*/

#pragma once


//Einstellungen für die Datenbankverbindungen
//Einstellt sind Standart-MySQL-Daten von XAMPP
#define er_db_host "localhost"
#define er_db_user "root"
#define er_db_passwort ""
#define er_db_name "wetterstation"
#define er_db_tablename "wetterdaten"

//Einstellungen COMPORT
#define er_COM_PORT "COM8"
#define er_COM_BAUDRATE 9600

//Dateiname für zwischengespeicherte CSV
#define CSV_DATEINAME "daten.csv"