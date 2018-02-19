CREATE TABLE `wetterdummy` (
	`ID` INT(10) UNSIGNED NOT NULL AUTO_INCREMENT COMMENT 'Fortlaufende Nr.',
	`DATE` DATETIME NOT NULL COMMENT 'Datum der Messung',
	`TEMP` INT(11) NOT NULL COMMENT 'Temperatur',
	`HUM` INT(11) NOT NULL COMMENT 'Luftfeuchtigkeit',
	`PRES` INT(11) NOT NULL COMMENT 'Luftdruck',
	`BRIGHT` INT(11) NOT NULL COMMENT 'Helligkeit in Lux',
	PRIMARY KEY (`ID`),
	UNIQUE INDEX `DATE` (`DATE`),
	INDEX `ID` (`ID`),
	INDEX `ID_2` (`ID`)
)
COMMENT='Wetterdaten für das Software-Engineering Projekt'
COLLATE='utf8_german2_ci'
ENGINE=MyISAM
AUTO_INCREMENT=13
;
