/*
 * SQL skript do predmetu IDS
 *
 * Projekt 4.cast
 *
 * Autori :
 * David Kozak      : xkozak15
 * Peter Miklanek   : xmikla10
 */

-- kvoli ruseniu predchadzajucich tabuliek
DROP INDEX index_novy;
DROP TABLE pobocka_krevni_banky CASCADE CONSTRAINTS;
DROP TABLE klient CASCADE CONSTRAINTS;
DROP TABLE rezervace CASCADE CONSTRAINTS;
DROP TABLE davka_krve CASCADE CONSTRAINTS;
DROP TABLE test_krve CASCADE CONSTRAINTS;
DROP TABLE pozvanka CASCADE CONSTRAINTS;
DROP TABLE zamestnanec CASCADE CONSTRAINTS;
DROP TABLE darce CASCADE CONSTRAINTS;
DROP TABLE test_darce CASCADE CONSTRAINTS;

DROP SEQUENCE pobocka_id;
DROP SEQUENCE davka_krve_id;
DROP SEQUENCE test_krve_id;
DROP SEQUENCE pozvanka_id;
DROP SEQUENCE test_darce_id;
DROP SEQUENCE rezervace_id;
DROP SEQUENCE osoba_num;

CREATE SEQUENCE osoba_num START WITH 1 INCREMENT BY 1 NOCYCLE;
CREATE SEQUENCE pozvanka_id START WITH 1 INCREMENT BY 1 NOCYCLE;
CREATE SEQUENCE pobocka_id START WITH 1 INCREMENT BY 1 NOCYCLE;
CREATE SEQUENCE davka_krve_id START WITH 1 INCREMENT BY 1 NOCYCLE;
CREATE SEQUENCE test_krve_id START WITH 1 INCREMENT BY 1 NOCYCLE;
CREATE SEQUENCE test_darce_id START WITH 1 INCREMENT BY 1 NOCYCLE;
CREATE SEQUENCE rezervace_id START WITH 1 INCREMENT BY 1 NOCYCLE;
--tabulka reprezentuje jednu pobocku
CREATE TABLE Pobocka_krevni_banky
(
    ID_pobocky INT DEFAULT pobocka_id.nextval NOT NULL,
    mesto VARCHAR(50) NOT NULL,
    ulice VARCHAR(50) NOT NULL,
    post_code VARCHAR(50) NOT NULL,
    CHECK(ID_pobocky > 0)
);
    ALTER TABLE pobocka_krevni_banky ADD (PRIMARY KEY (ID_pobocky));
--------------------------------------------------------------------
-- Jeden klient(zakaznik) banky
CREATE TABLE Klient
(
    login VARCHAR(50) NOT NULL,
    heslo VARCHAR(50) NOT NULL,
    jmeno VARCHAR(50) NOT NULL,
    prijmeni VARCHAR(50) NOT NULL,
    mesto VARCHAR(50) NOT NULL,
    ulice VARCHAR(50) NOT NULL,
    post_code VARCHAR(50) NOT NULL,
    CHECK(LENGTH(login) = 8) --loginy maji vzdy specifickou delku
);
    ALTER TABLE klient ADD (PRIMARY KEY (login));
--------------------------------------------------------------------
--jedna davka krve odebrana darci
CREATE TABLE Davka_krve
(
    ID_davky_krve INT DEFAULT davka_krve_id.nextval NOT NULL,
    datum_odebrani DATE NOT NULL,

  -- dalsi tri id jsou cizi klice do jinych tabulek
  ID_rezervace INT NOT NULL,
  ID_pobocky INT NOT NULL,
  login VARCHAR(50) NOT NULL,
  CHECK(ID_rezervace > 0 AND ID_pobocky > 0)
);
    ALTER TABLE davka_krve ADD (PRIMARY KEY (ID_davky_krve));
--------------------------------------------------------------------

--zaznam o provededenm testu krve
-- generalizace/specializace vyresena vyuzitim dvou separatnich tabulek, spolecne atributy z generickeho testu jsou uvedeny v obou techto tabulkach
CREATE TABLE Test_krve
(
    ID_testu INT DEFAULT test_krve_id.nextval NOT NULL,
    pocet_cervenych_krviniek INT NOT NULL, --vyjadrene jako cislo ve statisicich
    hladina_cholesterolu INT NOT NULL, --hladina vyjarena nasledovne: dvojmistne cislo, ktere se pred pouzitim vynasobi 0.1: 50== 5,0 mmol/l.
    mnozstvi_skodlivych_latek VARCHAR(50) NOT NULL, --jedna se o velice variabilni oblast, pro kazdy test specificky, proto pouze slovni popis
    chut VARCHAR(50) NOT NULL,  --opet specificke pro kazdou davku
    datum DATE NOT NULL,
    ID_davky_krve int NOT NULL,  --cizi klic do tabulky davka_krve
    login_zamestnance VARCHAR(50) NOT NULL, --cizi klic do tabulky zamestnanec
    CHECK(ID_testu > 0 AND ID_davky_krve > 0 AND pocet_cervenych_krviniek > 0 )
);
    ALTER TABLE test_krve ADD (PRIMARY KEY (ID_testu));
--------------------------------------------------------------------

CREATE TABLE Pozvanka
(
    ID_pozvanky INT DEFAULT pozvanka_id.nextval NOT NULL,
    datum DATE NOT NULL,
    ID_pobocky INT NOT NULL,
    login_zamestnance VARCHAR(50) NOT NULL,
    login VARCHAR(50) NOT NULL, --
    CHECK(ID_pozvanky > 0 AND ID_pobocky > 0)
);
    ALTER TABLE pozvanka ADD (PRIMARY KEY (ID_pozvanky));
--------------------------------------------------------------------
CREATE TABLE Zamestnanec
(
    login VARCHAR(50) NOT NULL,
    heslo VARCHAR(50) NOT NULL,
    jmeno VARCHAR(50) NOT NULL,
    prijmeni VARCHAR(50) NOT NULL,
    datum_narozeni DATE NOT NULL,
    mesto VARCHAR(50) NOT NULL,
    ulice VARCHAR(50) NOT NULL,
    post_code VARCHAR(50) NOT NULL,
    pozice VARCHAR(50) NOT NULL, -- typ enum
    ID_pobocky INT NOT NULL, -- cizi klic do tabulky pobocka
    CHECK(ID_pobocky > 0 ),
    CONSTRAINT overeni_pozice CHECK(pozice = 'IT' OR pozice = 'vedouci' OR pozice = 'recepcni' OR pozice ='skladnik' or pozice = 'brigadnik')
);
    ALTER TABLE zamestnanec ADD (PRIMARY KEY (login));

--------------------------------------------------------------------
CREATE TABLE Darce
(
    login VARCHAR(50) NOT NULL,
    jmeno VARCHAR(50) NOT NULL,
    prijmeni VARCHAR(50) NOT NULL,
    datum_narozeni DATE NOT NULL,
    mesto VARCHAR(50) NOT NULL,
    ulice VARCHAR(50) NOT NULL,
    post_code VARCHAR(50) NOT NULL,
    krevni_skupina VARCHAR(3) NOT NULL,

    CHECK(datum_narozeni < datum_posledniho_odberu), --omezujici podminka zajistujici verohodnost dat
    -- krevni skupina nemoze byt nic jine
    CHECK( krevni_skupina = 'A+' OR krevni_skupina = 'A-' OR krevni_skupina = 'B+'
    OR krevni_skupina = 'B-' OR krevni_skupina = 'AB+' OR krevni_skupina = 'AB-'
    OR krevni_skupina = '0+' OR krevni_skupina = '0-')
);
    ALTER TABLE darce ADD (PRIMARY KEY (login));
--------------------------------------------------------------------

CREATE TABLE Test_darce
(
    ID_testu INT DEFAULT test_darce_id.nextval NOT NULL,
    datum DATE NOT NULL,
    metoda_testovani VARCHAR(50) NOT NULL, -- typ enum
    hodnoceni VARCHAR(50) NOT NULL, -- slovni popsani vysledku

    --nasleduji cizi klice
    login VARCHAR(50) NOT NULL,
    login_zamestnance VARCHAR(50) NOT NULL,
    CHECK(ID_testu > 0),
    CHECK(metoda_testovani = 'psychotest' OR metoda_testovani = 'test vykonnosti' OR metoda_testovani = 'zdravotni test')
);
    ALTER TABLE Test_darce ADD (PRIMARY KEY (ID_testu));
--------------------------------------------------------------------
CREATE TABLE Rezervace
(
  ID_rezervace INT DEFAULT rezervace_id.nextval NOT NULL,
  datum DATE NOT NULL,
  --nasleduji cizi klice
  ID_pobocky INT NOT NULL,
  login_klienta VARCHAR(50) NOT NULL,
  CHECK(ID_rezervace > 0 AND ID_pobocky > 0 )
);
  ALTER TABLE Rezervace ADD (PRIMARY KEY (ID_REZERVACE));
--------------------------------------------------------------------

ALTER TABLE Davka_krve ADD FOREIGN KEY (ID_rezervace) REFERENCES Rezervace(ID_rezervace);
ALTER TABLE Davka_krve ADD FOREIGN KEY (ID_pobocky) REFERENCES Pobocka_krevni_banky(ID_pobocky) ON DELETE CASCADE; -- pokud se smaze pobocka, melo by dojit i k odstraneni davek
ALTER TABLE Davka_krve ADD FOREIGN KEY (login) REFERENCES Darce(login);

ALTER TABLE Test_krve ADD FOREIGN KEY (ID_davky_krve) REFERENCES Davka_krve(ID_davky_krve) ON DELETE CASCADE; --podobne nemuze exitovat test nepatrici k zadne davce
ALTER TABLE Test_krve ADD FOREIGN KEY (login_zamestnance) REFERENCES Zamestnanec(login);

ALTER TABLE Pozvanka ADD FOREIGN KEY (ID_pobocky) REFERENCES Pobocka_krevni_banky(ID_pobocky) ON DELETE CASCADE;
ALTER TABLE Pozvanka ADD FOREIGN KEY (login_zamestnance) REFERENCES Zamestnanec(login);
ALTER TABLE Pozvanka ADD FOREIGN KEY (login) REFERENCES Darce(login);

ALTER TABLE Zamestnanec ADD FOREIGN KEY (ID_pobocky) REFERENCES Pobocka_krevni_banky(ID_pobocky);

ALTER TABLE Test_darce ADD FOREIGN KEY (login) REFERENCES Darce(login) ON DELETE CASCADE;
ALTER TABLE Test_darce ADD FOREIGN KEY (login_zamestnance) REFERENCES Zamestnanec(login);

ALTER TABLE Rezervace ADD FOREIGN KEY (login_klienta) REFERENCES Klient(login) ON DELETE CASCADE;
ALTER TABLE Rezervace ADD FOREIGN KEY (ID_pobocky) REFERENCES Pobocka_krevni_banky(ID_pobocky);


------------------------------------------------------------------------------------------
-- Trigger pro automaticke generovani hodnot primarniho klice
------------------------------------------------------------------------------------------
create or replace trigger t1_trigger
  before insert on Pozvanka
  for each row
begin
    IF(:new.ID_pozvanky IS NULL) THEN
      :new.ID_pozvanky := pozvanka_id.nextval;
    END IF;
end t1_trigger;
/
show errors

-- procedura, ktera vypise ciselne a procentualni zastoupeni daneho vysledku testu krve
SET serveroutput ON;

------------------------------------------------------------------------------------------
-- Procedura pro vypis procentualneho zastoupeni hodnoceni testu darce
------------------------------------------------------------------------------------------

CREATE or REPLACE PROCEDURE krvesaj(arg_vysledek IN VARCHAR)
is
  cursor crs is SELECT * FROM test_darce;
  pocet_vsech_polozek NUMBER;
  pocet_daneho_typu NUMBER;
  jeden_radek test_darce%ROWTYPE;
BEGIN
  pocet_vsech_polozek := 0;
  pocet_daneho_typu := 0;
  open crs;

  LOOP
    FETCH crs INTO jeden_radek;
    EXIT WHEN crs%NOTFOUND;

    pocet_vsech_polozek :=  pocet_vsech_polozek + 1;
    IF (jeden_radek.hodnoceni = arg_vysledek) THEN
      pocet_daneho_typu :=  pocet_daneho_typu + 1;
    END IF;

  END LOOP;
  CLOSE crs;
  dbms_output.put_line('Vysledek ' || arg_vysledek || ' je v databazi ' || pocet_daneho_typu || 'krat a ma procentualni zastoupeni: ' || (pocet_daneho_typu * 100)/pocet_vsech_polozek || '%');
  EXCEPTION
  WHEN ZERO_DIVIDE THEN --ak taky hladany v danom umiestneny neni
    dbms_output.put_line('Vysledek ' || arg_vysledek || ' je v databazi 0krat a ma procentualni zastoupeni: 0%');
  WHEN OTHERS THEN --ina vynimka
    Raise_Application_Error (-20206, 'Nastala chyba!');
END;
/
show errors

-- to solve the wierd error
ALTER SESSION SET PLSCOPE_SETTINGS = 'IDENTIFIERS:NONE';

------------------------------------------------------------------------------------------
-- Trigger pro automaticke generovani loginu zamestnance
------------------------------------------------------------------------------------------

create or replace trigger zamestanec_login_creator
  before insert on zamestnanec
  for each row
DECLARE
  login varchar(50);
begin
    vytvor_login('y',:new.jmeno,:new.prijmeni,osoba_num.nextval,login);
    :new.login := login;
end zamestanec_login_creator;
/
show errors

------------------------------------------------------------------------------------------
-- Procedura pro vytvoreni loginu zamestnance z jmena a prijmeni
------------------------------------------------------------------------------------------

CREATE OR REPLACE PROCEDURE vytvor_login(prefix in varchar,jmeno in varchar,prijmeni in varchar,cislo in number,ret_login out VARCHAR)
is
  delka NUMBER;
  tmp_prijmeni VARCHAR(50);
  id_str  VARCHAR(2);
  login VARCHAR(50);
BEGIN
  tmp_prijmeni := prijmeni;
  delka := LENGTH(tmp_prijmeni);
  IF(delka < 5) THEN
    tmp_prijmeni := tmp_prijmeni || SUBSTR(jmeno,0, 5 - delka);
  END IF;
  tmp_prijmeni := SUBSTR(tmp_prijmeni,0,5);

  IF(cislo < 10) THEN
    id_str := '0' || cislo;
  ELSE
    id_str := cislo;
  END IF;

  login := LOWER(prefix || tmp_prijmeni || id_str);

  dbms_output.put_line(login);
  ret_login := login;
END;
/
show errors

------------------------------------------------------------------------------------------

-- ku kazdemu bude treba tak 3 priklady

-- SELECT

--vypise meno a priezvisko klienta, ktory je zaroven darce a datum jeho posledneho odberu
SELECT
    klient.jmeno,
    klient.prijmeni
FROM
    klient JOIN darce ON (klient.login=darce.login);

--vypise informace o vsech rezervacich(klient,datum,krevni skupina)  v databazi
SELECT
    rezervace.login_klienta,
    rezervace.datum,
    darce.krevni_skupina
FROM
    rezervace JOIN davka_krve ON (rezervace.id_rezervace=davka_krve.id_rezervace) JOIN darce on (davka_krve.login=darce.login);


-- vypise informace o vsech rezervacich klienta se jmenem Pavel a prijmenim Kocar
SELECT
    klient.jmeno,klient.prijmeni,
    pobocka_krevni_banky.adresa,
    rezervace.datum
FROM
    pobocka_krevni_banky JOIN
    rezervace ON (pobocka_krevni_banky.id_pobocky=rezervace.id_pobocky) JOIN
    klient ON (rezervace.login_klienta=klient.login)
WHERE
    klient.jmeno='Pavel' AND klient.prijmeni='Kocar';

-- vypise vsechny testy(info o darci a vysledku testu) vykonane zamestnancem s loginem ymokry01
SELECT
    darce.jmeno,
    darce.prijmeni,
    test_darce.metoda_testovani,
    test_darce.hodnoceni
FROM
    darce JOIN test_darce USING(login) JOIN zamestnanec ON (login_zamestnance=login)
WHERE
    zamestnanec.login='ymokry01';

-- vypise seznam darcu, kteri prosli alespon dvema testy serazene sestupne podle poctu testu
-- group by
SELECT
    darce.login,
    COUNT(*)
FROM
    darce JOIN test_darce ON (darce.login=test_darce.login)
GROUP BY
    darce.login
HAVING COUNT(*) > 1
ORDER BY COUNT(*) DESC;



-- vypise jednotlive pobocky a mnozstvi rezervaci na dane pobocce
SELECT
  pobocka_krevni_banky.adresa,
  COUNT(*)
FROM
  pobocka_krevni_banky JOIN rezervace ON (pobocka_krevni_banky.id_pobocky=rezervace.id_pobocky)
GROUP BY
  pobocka_krevni_banky.id_pobocky,pobocka_krevni_banky.adresa
ORDER BY
  pobocka_krevni_banky.adresa;

-- vypise id a adresu pobocek, jejichz zamestnanci provedli alespon jeden test krve s hodnocenim mnozstvi skodlivych latek 'mirne'(tzn relativne uspesne testy)
SELECT
  pobocka_krevni_banky.id_pobocky,
  pobocka_krevni_banky.adresa
FROM
  pobocka_krevni_banky
WHERE
    EXISTS(
        SELECT
            zamestnanec.login
        FROM
            zamestnanec JOIN test_krve on (zamestnanec.login=test_krve.login_zamestnance)
        WHERE
            zamestnanec.id_pobocky = pobocka_krevni_banky.id_pobocky AND test_krve.mnozstvi_skodlivych_latek='mirne') ;

-- IN sluzi k zisteniu ci je hodnota vyrazu zhodna s niektorou v zozname
-- vypise seznam darcu, ktere testoval zamestnanec s loginem ymokry01
SELECT
    darce.jmeno,
    darce.prijmeni
FROM
    darce JOIN test_darce ON (darce.login = test_darce.login)
WHERE
    test_darce.login
        IN(
            SELECT
                test_darce.login
            FROM
                test_darce
            WHERE
                test_darce.login_zamestnance = 'ymokry01');

-------------------------------------------------------------------------------
exec krvesaj('vyborne');
-------------------------------------------------------------------------------
-- explain plan pred indexaci
EXPLAIN PLAN FOR
  SELECT darce.jmeno, COUNT(davka_krve.login)
  FROM darce, davka_krve
  WHERE davka_krve.login=darce.login AND darce.jmeno='Matej'
  GROUP BY darce.jmeno;

--Zobrazeni explain planu
SELECT * FROM TABLE(DBMS_XPLAN.display);

-- Vytvoreni indexu
CREATE INDEX index_novy ON davka_krve (login);

-- explain plan po indexaci
EXPLAIN PLAN FOR
  SELECT darce.jmeno, COUNT(davka_krve.login)
  FROM darce, davka_krve
  WHERE davka_krve.login=darce.login AND darce.jmeno='Matej'
  GROUP BY darce.jmeno;

--Zobrazeni explain planu
SELECT * FROM TABLE(DBMS_XPLAN.display);

------------------------------------------------------------------------------------------
-- Prirazeni prav uzivateli xmikla10
------------------------------------------------------------------------------------------

GRANT ALL ON pobocka_krevni_banky TO xmikla10;
GRANT ALL ON klient TO xmikla10;
GRANT ALL ON rezervace TO xmikla10;
GRANT ALL ON davka_krve TO xmikla10;
GRANT ALL ON test_krve TO xmikla10;
GRANT ALL ON pozvanka TO xmikla10;
GRANT ALL ON zamestnanec TO xmikla10;
GRANT ALL ON darce TO xmikla10;
GRANT ALL ON test_darce TO xmikla10;

GRANT EXECUTE ON krvesaj TO xmikla10;
GRANT EXECUTE ON vytvor_login TO xmikla10;

-------------------------------------------------------------------------------
-- Vytvoreni materializovaneho pohledu druhym clenem teamu
-------------------------------------------------------------------------------
DROP MATERIALIZED VIEW testy_darcu;

CREATE MATERIALIZED VIEW LOG ON test_darce WITH PRIMARY KEY INCLUDING NEW VALUES;

CREATE MATERIALIZED VIEW testy_darcu
CACHE
BUILD IMMEDIATE
REFRESH FAST ON COMMIT
AS
SELECT
    *
FROM
    test_darce;

GRANT ALL ON testy_darcu TO xkozak15;

SELECT * FROM xmikla10.testy_darcu;
INSERT INTO test_darce ( ID_testu, datum,metoda_testovani,hodnoceni,login,login_zamestnance) VALUES (DEFAULT,to_date('2016-09-14 08:00:00','yyyy-mm-dd hh-mi-ss'),'psychotest','prumer','xkocar01','yvesel02');
commit;
SELECT * FROM xmikla10.testy_darcu;
