-- MySQL dump 9.08
--
-- Host: localhost    Database: KLASA
---------------------------------------------------------
-- Server version	4.0.13

--
-- Table structure for table 'frekwencje'
--

DROP TABLE IF EXISTS frekwencje;
CREATE TABLE frekwencje (
  IDfrekwencji int(11) NOT NULL auto_increment,
  IDucznia int(11) default NULL,
  RazemGodz int(11) default NULL,
  NieobecnychGodz int(11) default NULL,
  Spoznien int(11) default NULL,
  NrMiesiaca int(11) default NULL,
  PRIMARY KEY  (IDfrekwencji)
) TYPE=MyISAM;

--
-- Dumping data for table 'frekwencje'
--

/*!40000 ALTER TABLE frekwencje DISABLE KEYS */;
LOCK TABLES frekwencje WRITE;
UNLOCK TABLES;
/*!40000 ALTER TABLE frekwencje ENABLE KEYS */;

--
-- Table structure for table 'klasa'
--

DROP TABLE IF EXISTS klasa;
CREATE TABLE klasa (
  IDklasy int(11) NOT NULL auto_increment,
  Nazwa char(25) default NULL,
  Rok int(11) default NULL,
  PRIMARY KEY  (IDklasy)
) TYPE=MyISAM;

--
-- Dumping data for table 'klasa'
--

/*!40000 ALTER TABLE klasa DISABLE KEYS */;
LOCK TABLES klasa WRITE;
INSERT INTO klasa (IDklasy, Nazwa, Rok) VALUES (1,'IIe',2002);
UNLOCK TABLES;
/*!40000 ALTER TABLE klasa ENABLE KEYS */;

--
-- Table structure for table 'ocena'
--

DROP TABLE IF EXISTS ocena;
CREATE TABLE ocena (
  IDoceny int(11) NOT NULL auto_increment,
  Wartosc int(11) default NULL,
  Zachowanie char(25) default NULL,
  IDucznia int(11) default NULL,
  IDprzedmiotu int(11) default NULL,
  PRIMARY KEY  (IDoceny)
) TYPE=MyISAM;

--
-- Dumping data for table 'ocena'
--

/*!40000 ALTER TABLE ocena DISABLE KEYS */;
LOCK TABLES ocena WRITE;
UNLOCK TABLES;
/*!40000 ALTER TABLE ocena ENABLE KEYS */;

--
-- Table structure for table 'przedmiot'
--

DROP TABLE IF EXISTS przedmiot;
CREATE TABLE przedmiot (
  IDprzedmiotu int(11) NOT NULL auto_increment,
  Nazwa char(75) default NULL,
  CzyLiczony tinyint(4) default NULL,
  PRIMARY KEY  (IDprzedmiotu)
) TYPE=MyISAM;

--
-- Dumping data for table 'przedmiot'
--

/*!40000 ALTER TABLE przedmiot DISABLE KEYS */;
LOCK TABLES przedmiot WRITE;
INSERT INTO przedmiot (IDprzedmiotu, Nazwa, CzyLiczony) VALUES (1,'matematyka',1),(2,'jezyk polski',1),(3,'historia',1),(4,'wiedza o spoleczenstwie',1),(5,'jezyk rosyjski',1),(6,'jezyk francuski',0),(7,'jezyk angielski',1),(8,'jezyk niemiecki',0),(9,'fizyka',1),(10,'chemia',1),(11,'biologia',1),(12,'geografia',1),(13,'plastyka',1),(14,'muzyka',1),(15,'technika',1),(16,'religia',0),(17,'informatyka',1),(18,'W-F',1);
UNLOCK TABLES;
/*!40000 ALTER TABLE przedmiot ENABLE KEYS */;

--
-- Table structure for table 'uczen'
--

DROP TABLE IF EXISTS uczen;
CREATE TABLE uczen (
  IDucznia int(11) NOT NULL auto_increment,
  Imie char(40) default NULL,
  Nazwisko char(75) default NULL,
  Numer int(11) default NULL,
  IDklasy int(11) default NULL,
  PRIMARY KEY  (IDucznia)
) TYPE=MyISAM;

--
-- Dumping data for table 'uczen'
--

/*!40000 ALTER TABLE uczen DISABLE KEYS */;
LOCK TABLES uczen WRITE;
UNLOCK TABLES;
/*!40000 ALTER TABLE uczen ENABLE KEYS */;

