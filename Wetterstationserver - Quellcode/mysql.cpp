/*
Name:		mysql.cpp
Created:	29.01.2017 16:22:41
Author:		MySQL

MySQL-Methoden zum Absenden von Queries zum MySQL-Server. Angepasst durch John
*/

#include "definitionen.h"

#include "mysql_connection.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

using namespace std;

string aktuellerpfadini1()
{
	//Aktueller Pfad wo sich die INI befindet (benötigt für das Öffnen des INI-Files, wo Einstellungen getätigt werden können/sollen)

	char result[MAX_PATH];
	std::string sresult(result, GetModuleFileName(NULL, result, MAX_PATH));

	size_t found = sresult.rfind("\\");
	sresult.replace(found, sresult.length() - found, "\\wetter.ini");

	return sresult;
}

void queryexe(string abfrage)
{

	try {
		// Initialisierungen
		sql::Driver *driver;
		sql::Connection *con;
		sql::Statement *stmt;

		//Lese Daten aus INI
		const unsigned long puffersize = 255;

		char db_host[puffersize];
		GetPrivateProfileString("wetter", "db_host", er_db_host, db_host, puffersize, aktuellerpfadini1().c_str());

		char db_user[puffersize];
		GetPrivateProfileString("wetter", "db_user", er_db_user, db_user, puffersize, aktuellerpfadini1().c_str());

		char db_passwort[puffersize];
		GetPrivateProfileString("wetter", "db_passwort", er_db_passwort, db_passwort, puffersize, aktuellerpfadini1().c_str());

		char db_name[puffersize];
		GetPrivateProfileString("wetter", "db_name", er_db_name, db_name, puffersize, aktuellerpfadini1().c_str());


//		Antwort nicht nötig, da bei Fehler eh catch eintrifft.
//		sql::ResultSet *res;


		/* Definiere Verbindung */
		driver = get_driver_instance();
		con = driver->connect(db_host, db_user, db_passwort);
		/* verbinde zur Datenbank */
		con->setSchema(db_name);

		stmt = con->createStatement();

		sql::SQLString query_sql = abfrage.c_str();

		stmt->execute(query_sql);


		// Falls ich die Antwort doch benötigen sollte, steht hier der Code
		/*

		//Abfrage zum Testen
		res = stmt->executeQuery("UPDATE db_297671_8.wetterdummy SET TEMP=13 WHERE  ID=17;");
		
		while (res->next()) {
		cout << "\t... MySQL replies: ";
		// Access column data by alias or column name
		cout << res->getString("_message") << endl;
		cout << "\t... MySQL says it again: ";
		//Access column data by numeric offset, 1 is the first column
		cout << res->getString(1) << endl;
		}
		
		cout << res;
		
		delete res;
		*/

		delete stmt;
		delete con;
	}

	//FEHLERMELDUNG
	catch (sql::SQLException &e) {
		cout << "FEHLER beim Eintrag in die Datenbank (" << e.what() << ")";
		/*
		cout << "FEHLER: SQLException in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") in Zeile" << __LINE__ << endl;
		cout << "# ERR: " << e.what();
		cout << " (MySQL Fehlercode: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
		*/
	}
}



