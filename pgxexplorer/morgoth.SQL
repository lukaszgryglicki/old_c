--
-- PostgreSQL database dump
--

SET client_encoding = 'SQL_ASCII';
SET check_function_bodies = false;

SET SESSION AUTHORIZATION 'morgoth';

--
-- TOC entry 4 (OID 2200)
-- Name: public; Type: ACL; Schema: -; Owner: morgoth
--

REVOKE ALL ON SCHEMA public FROM PUBLIC;
GRANT ALL ON SCHEMA public TO PUBLIC;


SET search_path = public, pg_catalog;

--
-- TOC entry 5 (OID 17143)
-- Name: klasa_seq; Type: SEQUENCE; Schema: public; Owner: morgoth
--

CREATE SEQUENCE klasa_seq
    INCREMENT BY 1
    NO MAXVALUE
    MINVALUE 0
    CACHE 1;


--
-- TOC entry 7 (OID 17145)
-- Name: przedmiot_seq; Type: SEQUENCE; Schema: public; Owner: morgoth
--

CREATE SEQUENCE przedmiot_seq
    INCREMENT BY 1
    NO MAXVALUE
    MINVALUE 0
    CACHE 1;


--
-- TOC entry 9 (OID 17147)
-- Name: uczen_seq; Type: SEQUENCE; Schema: public; Owner: morgoth
--

CREATE SEQUENCE uczen_seq
    INCREMENT BY 1
    NO MAXVALUE
    MINVALUE 0
    CACHE 1;


--
-- TOC entry 11 (OID 17149)
-- Name: ocena_seq; Type: SEQUENCE; Schema: public; Owner: morgoth
--

CREATE SEQUENCE ocena_seq
    INCREMENT BY 1
    NO MAXVALUE
    MINVALUE 0
    CACHE 1;


--
-- TOC entry 13 (OID 17151)
-- Name: przedmiot; Type: TABLE; Schema: public; Owner: morgoth
--

CREATE TABLE przedmiot (
    nazwa text NOT NULL,
    idprzedmiotu integer DEFAULT nextval('"przedmiot_seq"'::text) NOT NULL
);


--
-- TOC entry 14 (OID 17157)
-- Name: klasa; Type: TABLE; Schema: public; Owner: morgoth
--

CREATE TABLE klasa (
    nazwa text NOT NULL,
    idklasy integer DEFAULT nextval('"klasa_seq"'::text) NOT NULL
);


--
-- TOC entry 15 (OID 17163)
-- Name: uczen; Type: TABLE; Schema: public; Owner: morgoth
--

CREATE TABLE uczen (
    imie text NOT NULL,
    nazwisko text NOT NULL,
    idklasy integer NOT NULL,
    iducznia integer DEFAULT nextval('"uczen_seq"'::text) NOT NULL
);


--
-- TOC entry 16 (OID 17169)
-- Name: ocena; Type: TABLE; Schema: public; Owner: morgoth
--

CREATE TABLE ocena (
    ocena real NOT NULL,
    czyliczona boolean NOT NULL,
    iducznia integer NOT NULL,
    idprzedmiotu integer NOT NULL,
    idoceny integer DEFAULT nextval('"ocena_seq"'::text) NOT NULL
);


--
-- TOC entry 17 (OID 18913)
-- Name: tabelka; Type: TABLE; Schema: public; Owner: morgoth
--

CREATE TABLE tabelka (
    pk bigint NOT NULL,
    fk integer NOT NULL,
    str text,
    flo double precision DEFAULT 3.14 NOT NULL
);


--
-- TOC entry 18 (OID 18933)
-- Name: vsredniel; Type: VIEW; Schema: public; Owner: morgoth
--

CREATE VIEW vsredniel AS
    SELECT klasa.idklasy AS id, klasa.nazwa, avg(ocena.ocena) AS srednia FROM ocena, uczen, klasa WHERE ((ocena.iducznia = uczen.iducznia) AND (uczen.idklasy = klasa.idklasy)) GROUP BY klasa.idklasy, klasa.nazwa ORDER BY klasa.nazwa;


--
-- TOC entry 19 (OID 18936)
-- Name: vavg_mark; Type: VIEW; Schema: public; Owner: morgoth
--

CREATE VIEW vavg_mark AS
    SELECT avg(ocena.ocena) AS average FROM ocena WHERE (ocena.czyliczona = true);


--
-- TOC entry 20 (OID 18939)
-- Name: vsrednie_uczniow; Type: VIEW; Schema: public; Owner: morgoth
--

CREATE VIEW vsrednie_uczniow AS
    SELECT uczen.iducznia, klasa.idklasy, avg(ocena.ocena) AS srednia FROM uczen, ocena, klasa WHERE (((uczen.idklasy = klasa.idklasy) AND (ocena.iducznia = uczen.iducznia)) AND (ocena.czyliczona = true)) GROUP BY uczen.iducznia, klasa.idklasy;


--
-- TOC entry 21 (OID 18942)
-- Name: vsrednie_uczniowl; Type: VIEW; Schema: public; Owner: morgoth
--

CREATE VIEW vsrednie_uczniowl AS
    SELECT uczen.iducznia, klasa.idklasy, avg(ocena.ocena) AS srednia FROM uczen, ocena, klasa WHERE (((uczen.idklasy = klasa.idklasy) AND (ocena.iducznia = uczen.iducznia)) AND (ocena.czyliczona = true)) GROUP BY uczen.iducznia, klasa.idklasy;


--
-- TOC entry 22 (OID 18945)
-- Name: vavg_subjs; Type: VIEW; Schema: public; Owner: morgoth
--

CREATE VIEW vavg_subjs AS
    SELECT klasa.nazwa AS "class", przedmiot.nazwa AS subject, avg(ocena.ocena) AS avg FROM ocena, przedmiot, uczen, klasa WHERE (((ocena.idprzedmiotu = przedmiot.idprzedmiotu) AND (ocena.iducznia = uczen.iducznia)) AND (uczen.idklasy = klasa.idklasy)) GROUP BY przedmiot.nazwa, klasa.nazwa ORDER BY klasa.nazwa, przedmiot.nazwa;


--
-- TOC entry 23 (OID 18948)
-- Name: vcount_cls; Type: VIEW; Schema: public; Owner: morgoth
--

CREATE VIEW vcount_cls AS
    SELECT klasa.idklasy AS id, klasa.nazwa AS class_name, count(uczen.imie) AS count FROM klasa, uczen WHERE (klasa.idklasy = uczen.idklasy) GROUP BY klasa.idklasy, klasa.nazwa ORDER BY klasa.nazwa;


--
-- TOC entry 24 (OID 18951)
-- Name: vdivision; Type: VIEW; Schema: public; Owner: morgoth
--

CREATE VIEW vdivision AS
    SELECT (((SELECT count(ocena.ocena) AS count FROM ocena WHERE (ocena.czyliczona = true)) * 100) / (SELECT count(ocena.ocena) AS count FROM ocena)) AS percent_computed_in_avg;


--
-- TOC entry 25 (OID 18954)
-- Name: vlist; Type: VIEW; Schema: public; Owner: morgoth
--

CREATE VIEW vlist AS
    SELECT uczen.imie AS stud_name, uczen.nazwisko AS stud_surname, klasa.nazwa AS "class" FROM uczen, klasa WHERE (klasa.idklasy = uczen.idklasy) ORDER BY uczen.nazwisko;


--
-- TOC entry 26 (OID 18957)
-- Name: vsrednie_klas; Type: VIEW; Schema: public; Owner: morgoth
--

CREATE VIEW vsrednie_klas AS
    SELECT klasa.idklasy, klasa.nazwa AS klasa, avg(vsrednie_uczniow.srednia) AS srednia FROM vsrednie_uczniow, klasa WHERE (vsrednie_uczniow.idklasy = klasa.idklasy) GROUP BY klasa.idklasy, klasa.nazwa ORDER BY klasa.nazwa;


--
-- TOC entry 27 (OID 18960)
-- Name: vsrednie_kl; Type: VIEW; Schema: public; Owner: morgoth
--

CREATE VIEW vsrednie_kl AS
    SELECT klasa.idklasy AS id, klasa.nazwa AS "class", avg(vsrednie_uczniow.srednia) AS avg FROM vsrednie_uczniow, klasa WHERE (vsrednie_uczniow.idklasy = klasa.idklasy) GROUP BY klasa.idklasy, klasa.nazwa ORDER BY klasa.nazwa;


--
-- Data for TOC entry 33 (OID 17151)
-- Name: przedmiot; Type: TABLE DATA; Schema: public; Owner: morgoth
--

COPY przedmiot (nazwa, idprzedmiotu) FROM stdin;
matematyka	8
jezyk polski	9
jezyk angielski	10
jezyk rosyjski	11
biologia	12
chemia	13
fizyka	14
w-f	15
religia	16
plastyka	17
technika	18
historia	19
WOS	20
sztuka	21
bla	24
\.


--
-- Data for TOC entry 34 (OID 17157)
-- Name: klasa; Type: TABLE DATA; Schema: public; Owner: morgoth
--

COPY klasa (nazwa, idklasy) FROM stdin;
Id	7
IIc	8
IIIb	9
Ve	11
VIf	12
IIci	13
VIIc	14
IVa	10
\.


--
-- Data for TOC entry 35 (OID 17163)
-- Name: uczen; Type: TABLE DATA; Schema: public; Owner: morgoth
--

COPY uczen (imie, nazwisko, idklasy, iducznia) FROM stdin;
Arek	Zaborowski	7	17
Bartek	Burdach	7	18
Pawel	Burdach	7	19
Jarek	Grzegzolka	7	20
Aleksander	Piotrowski	7	21
Darek	Jawoszek	7	23
Stefan	Kwas	7	24
Malgorzata	Antosik	7	26
Lukasz	Bainski	7	27
Tomek	Jazwinski	7	28
Aleksander	Bartnicki	7	29
Radek	Zamojski	7	30
Alojzy	Klopotek	7	31
Eugeniusz	Ajsztajn	7	32
Anna	Gajowniczek	7	22
Ela	Gajewska	7	25
Darek	Burdach	8	33
Daniel	Wisniewski	8	34
Alojzy	Kleks	8	35
Wieslaw	Ozygala	8	36
Rozalia	Ozygala	8	37
Justyna	Steczkowska	8	38
Kaja	Gregowicz	8	39
Anita	Lipnicka	8	40
Jezus	Chrystus	8	41
Swiety	Podciety	8	42
Stefan	Zajac	8	43
Lukasz	Gryglicki	8	44
Renata	Burdach	8	45
Andrzej	Burdach	8	46
Ryszard	Gawronski	8	47
Grzegoz	Brzeczyszczykiewicz	8	48
Alojzy	Klopotek	8	49
Jan	Kowalski	13	50
Marek	Kaska	13	51
Jarek	Kaska	13	52
Alojzy	Klopotek	13	53
Ewa	Michalska	13	54
Radek	Dzyndzalski	13	55
Xena	Puszczalska	13	56
Dorota	Puszczalska	13	57
Ela	Zbanistrzela	13	58
Majka	Dupodajka	13	59
Porucznik	Borowicz	13	60
Porucznik	Borewicz	13	61
Bolek	Amperlolek	13	62
Zbigniew	Krzaczasty	13	63
Uegeniusz	Czarzasty	13	64
Aleksander	Kwasniewski	13	65
Leszek	Miller	13	66
Bartek	Burdach	9	67
Alek	Zacharek	9	68
Tadeusz	Rydzyk	9	69
Smirnoff	Molotow	9	70
Gologfow	Kasparow	9	71
Benito	Mussolini	9	73
Marek	Kaska	9	74
Adolf	Hitler	9	72
Jakub	Sadlowski	9	75
Michal	Pniewski	9	76
Michal	Chylinski	9	77
Agnieszka	Gajowniczek	9	78
Ola	Krasicka	9	79
Michal	Marciniak	9	80
Marcin	Borkowski	9	81
Marcin	Wiacek	9	82
Wawrzyniec	Pniewski	10	83
Ola	Pniewska	10	84
Radek	Burdach	10	85
Hubert	Ozygala	10	86
Marian	Urbanski	10	87
Mirek	Urbansski	10	88
Hubert	Urbanski	10	89
Bartek	Zanussi	10	90
Bill	Gates	10	91
Linus	Torwards	10	92
Alojzy	Klopotek	10	93
Henryka	Gryglicka	10	94
Henryk	Gryglicki	10	95
Lukasz	Gryglicki	10	96
Rafal	Wojciechowski	10	97
Magda	Wojciechowska	10	98
Arkadiusz	Chojarski	11	99
Barkadek	Zamachowski	11	100
Eugeniusz	Rawinski	11	101
Tadzio	Rutkowski	11	102
Romek	Atomek	11	103
Napletek	Orkietek	11	104
Arnold	Szwarcen	11	105
Arek	Orgetek	11	106
Stasiek	Ciemieniowski	11	107
Tomasz	Ciolkowski	11	108
Franek	Frankowski	11	109
Marek	Kaska	11	110
Darek	Rozalski	11	111
Lukasz	Wiercioch	11	112
Albrecht	Albrechcinski	11	113
Alojzy	Klopotek	11	114
Roman	Kostrzewski	11	115
Piotr	Kuzmicz	11	116
Alojzy	Klopotliwy	12	117
Alojzy	Klopoteczny	12	118
Albrecht	Klopotek	12	119
Ele	Witchcroft	12	120
Lara	Croft	12	121
Pamela	Anderson	12	122
Cindy	Crawford	12	123
Teresa	Orlowski	12	124
Lexus	Locklenar	12	125
Peter	North	12	126
Wanda	Zadwabrowary	12	127
Mariola	Lapidzida	12	128
Ewelina	Hopsrazdoobuzi	12	129
Hela	Robiloda	12	130
Lucifer	Mephisto	12	131
Dark	Hellhammer	12	132
Wlodzimierz	Polawsky	12	133
Roman	Spuszczecisienatwarz	12	134
Arkadiusz	Domanski	12	135
Morgoth	Cursed	12	136
\.


--
-- Data for TOC entry 36 (OID 17169)
-- Name: ocena; Type: TABLE DATA; Schema: public; Owner: morgoth
--

COPY ocena (ocena, czyliczona, iducznia, idprzedmiotu, idoceny) FROM stdin;
4	t	58	12	3
3.5	t	58	13	4
4.5	t	58	14	5
4	t	58	19	6
4	t	58	10	7
4.5	t	58	9	8
3	t	58	11	9
4.5999999	t	58	8	10
4	f	58	17	11
2	f	58	16	12
5	f	58	21	13
6	t	58	18	14
1	f	58	15	15
5	f	27	20	22
3	f	26	20	24
4	f	29	20	25
5	f	18	20	26
6	f	19	20	27
5	f	25	20	28
4	f	22	20	29
3	f	20	20	30
3	t	32	12	32
4	t	26	12	33
5	t	27	12	34
4	t	29	12	35
3	t	18	12	36
4	t	19	12	37
5	t	25	12	38
4.5	t	22	12	39
3.5	t	20	12	40
2	t	23	12	41
3	t	28	12	42
4	t	31	12	43
5	t	24	12	44
4	t	21	12	45
3	t	17	12	46
4	t	30	12	47
3	f	23	20	48
4	f	28	20	49
5	f	31	20	50
4	f	24	20	51
3	f	21	20	52
2	f	17	20	53
3	f	30	20	54
4.5	t	32	13	55
3	t	26	13	56
4.5999999	t	27	13	57
5	t	29	13	58
4	t	18	13	59
4	t	19	13	60
4	t	25	13	61
3	t	22	13	62
4	t	20	13	63
5	t	23	13	64
4	t	28	13	65
4	t	31	13	66
3	t	24	13	67
3	t	21	13	68
3	t	17	13	69
4	t	30	13	70
3	t	32	14	71
3	t	26	14	72
2	t	27	14	73
3	t	29	14	74
2	t	18	14	75
4	t	19	14	76
3	t	25	14	77
2	t	22	14	78
3	t	20	14	79
4	t	23	14	80
3	t	28	14	81
5	t	31	14	82
4	t	24	14	83
4.5999999	t	21	14	84
4	t	17	14	85
5	t	30	14	86
3	t	32	19	87
4	t	26	19	88
5	t	27	19	89
4	t	29	19	90
3	t	18	19	91
3	t	19	19	92
3	t	25	19	93
4	t	22	19	94
4	t	20	19	95
5	t	23	19	96
4	t	28	19	97
5	t	31	19	98
4	t	24	19	99
3	t	21	19	100
4	t	17	19	101
5	t	30	19	102
4	t	32	10	103
3	t	26	10	104
4	t	29	10	105
5	t	18	10	106
3	t	19	10	107
4	t	20	10	108
2	t	23	10	109
4	t	28	10	110
3	t	24	10	111
5	t	21	10	112
3	t	30	10	113
4	t	32	9	114
3	t	26	9	115
4	t	27	9	116
5	t	29	9	117
4	t	18	9	118
3	t	19	9	119
4	t	25	9	120
4	t	22	9	121
4	t	20	9	122
5	t	23	9	123
2	t	28	9	124
2	t	31	9	125
1	t	24	9	126
4	t	21	9	127
4	t	17	9	128
1	t	30	9	129
2	f	32	11	130
3	f	26	11	131
2	f	27	11	132
1	f	29	11	133
3	f	18	11	134
4	f	19	11	135
2	f	25	11	136
5	f	22	11	137
4	f	20	11	138
3	f	23	11	139
2	f	24	11	140
2	f	17	11	141
5	f	30	11	142
5	f	32	17	143
5	f	26	17	144
5	f	27	17	145
5	f	29	17	146
4	f	18	17	147
5	f	19	17	148
5	f	25	17	149
5	f	22	17	150
4	f	20	17	151
5	f	23	17	152
5	f	28	17	153
4	f	31	17	154
5	f	24	17	155
4	f	21	17	156
4	f	17	17	157
6	f	32	16	158
5	f	26	16	159
6	f	27	16	160
5	f	29	16	161
5	f	18	16	162
6	f	19	16	163
5	f	25	16	164
6	f	22	16	165
5	f	31	16	166
6	f	21	16	167
6	f	17	16	168
5	t	32	21	169
4	t	26	21	170
5	t	27	21	171
6	t	29	21	172
5	t	18	21	173
5	t	19	21	174
5	t	25	21	175
6	t	22	21	176
6	t	20	21	177
4	t	23	21	178
5	t	31	21	179
4	t	24	21	180
5	t	21	21	181
6	t	17	21	182
4	t	32	18	183
5	t	26	18	184
4	t	27	18	185
5	t	29	18	186
6	t	18	18	187
5	t	20	18	188
4	t	23	18	189
5	t	28	18	190
6	t	31	18	191
5	t	24	18	192
4	t	21	18	193
5	t	17	18	194
6	t	30	18	195
5	t	32	15	196
4	t	26	15	197
5	t	27	15	198
5	t	19	15	199
4	t	25	15	200
5	t	22	15	201
5	t	20	15	202
4	t	24	15	203
5	t	21	15	204
4	t	17	15	205
3	t	32	8	206
2	t	26	8	207
1	t	27	8	208
1	t	29	8	209
1	t	18	8	210
2	t	25	8	211
3	t	22	8	212
2	t	20	8	213
3	t	23	8	214
4	t	28	8	215
1	t	31	8	216
1	t	24	8	217
6	t	21	8	218
6	t	17	8	219
6	t	30	8	220
1	t	32	20	221
4	t	48	20	222
3	t	33	20	223
2	t	46	20	224
3	t	45	20	225
4	t	41	20	226
4.5	t	47	20	227
4	t	39	20	228
3	t	44	20	229
4	t	35	20	230
5	t	49	20	231
5	t	40	20	232
4	t	36	20	233
3	t	37	20	234
4	t	42	20	235
5	t	38	20	236
4	t	34	20	237
3	t	43	20	238
4	t	46	12	239
3	t	45	12	240
5	t	41	12	241
4	t	47	12	242
3	t	39	12	243
4	t	44	12	244
5	t	35	12	245
4	t	49	12	246
3	t	40	12	247
2	t	36	12	248
3	t	37	12	249
4	t	42	12	250
5	t	38	12	251
4	t	34	12	252
3	t	43	12	253
4	f	48	13	254
3	f	33	13	255
2	f	46	13	256
3	f	45	13	257
4	f	41	13	258
3	f	47	13	259
4	f	39	13	260
4	f	49	13	261
3	f	40	13	262
4	f	36	13	263
5	f	37	13	264
6	f	42	13	265
1	f	38	13	266
4	f	34	13	267
3	f	43	13	268
4	t	33	14	269
5	t	46	14	270
4	t	45	14	271
3	t	41	14	272
3	t	47	14	273
4	t	39	14	274
5	t	44	14	275
4	t	35	14	276
3	t	49	14	277
2	t	40	14	278
3	t	36	14	279
4	t	37	14	280
5	t	42	14	281
4	t	38	14	282
3	t	34	14	283
4	t	43	14	284
2	t	48	19	285
3	t	33	19	286
2	t	46	19	287
3	t	45	19	288
2	t	47	19	289
3	t	39	19	290
4	t	44	19	291
3	t	35	19	292
4	t	49	19	293
5	t	40	19	294
4	t	36	19	295
3	t	37	19	296
4	t	42	19	297
3	t	34	19	298
4	t	43	19	299
1	f	48	10	300
1	f	33	10	301
2	f	46	10	302
2	f	47	10	303
3	f	39	10	304
4	f	44	10	305
3	f	35	10	306
2	f	49	10	307
5	f	40	10	308
4	f	36	10	309
3	f	37	10	310
3	f	38	10	311
4	f	34	10	312
6	f	43	10	313
3	t	48	9	314
3	t	33	9	315
3	t	46	9	316
3	t	45	9	317
3	t	41	9	318
3	t	47	9	319
4	t	39	9	320
2	t	44	9	321
1	t	49	9	322
5	t	40	9	323
4	t	36	9	324
5	t	37	9	325
4	t	42	9	326
3	t	38	9	327
2	t	34	9	328
5	t	43	9	329
4	t	48	11	330
5	t	33	11	331
6	t	46	11	332
5	t	45	11	333
4	t	41	11	334
5	t	47	11	335
5	t	39	11	336
6	t	44	11	337
5	t	35	11	338
6	t	49	11	339
5	t	40	11	340
5	t	42	11	341
6	t	38	11	342
5	t	34	11	343
6	t	43	11	344
2	t	48	8	345
3	t	33	8	346
2	t	46	8	347
1	t	45	8	348
2	t	41	8	349
3	t	47	8	350
2	t	39	8	351
1	t	44	8	352
1	t	49	8	353
1	t	40	8	354
2	t	36	8	355
1	t	37	8	356
2	t	42	8	357
3	t	38	8	358
2	t	34	8	359
2	t	43	8	360
6	t	48	17	361
6	t	33	17	362
6	t	46	17	363
6	t	45	17	364
5	t	41	17	365
6	t	47	17	366
5	t	39	17	367
5	t	44	17	368
5	t	35	17	369
6	t	49	17	370
6	t	40	17	371
5	t	36	17	372
6	t	37	17	373
5	t	38	17	374
6	t	34	17	375
5	t	43	17	376
1	f	48	16	377
1	f	33	16	378
1	f	46	16	379
1	f	45	16	380
1	f	41	16	381
1	f	47	16	382
1	f	39	16	383
1	f	49	16	384
1	f	42	16	385
1	f	38	16	386
5	t	48	21	387
4	t	33	21	388
5	t	46	21	389
6	t	45	21	390
5	t	41	21	391
4	t	47	21	392
5	t	39	21	393
6	t	44	21	394
5	t	35	21	395
4	t	49	21	396
5	t	40	21	397
6	t	36	21	398
5	t	37	21	399
5	t	42	21	400
5	t	38	21	401
5	t	34	21	402
3	t	44	18	403
2	t	35	18	404
2	t	36	18	405
4	t	37	18	406
6	t	42	18	407
1	t	38	18	408
5	t	43	18	409
2	f	48	15	410
3	f	33	15	411
4	f	46	15	412
3	f	45	15	413
3	f	41	15	414
4	f	47	15	415
4	f	39	15	416
5	f	44	15	417
4	f	35	15	418
3	f	49	15	419
2	f	40	15	420
3	f	36	15	421
4	f	37	15	422
4	f	42	15	423
6	f	38	15	424
1	f	34	15	425
5	f	62	20	426
4	f	61	20	427
5	f	60	20	428
4	f	64	20	429
5	f	59	20	430
6	f	55	20	431
5	f	52	20	432
4	f	51	20	433
3	f	53	20	434
4	f	50	20	435
5	f	63	20	436
4	f	54	20	437
5	f	66	20	438
4	f	57	20	439
5	f	56	20	440
4	t	62	12	441
5	t	61	12	442
4	t	60	12	443
3	t	64	12	444
4	t	59	12	445
4	t	55	12	446
4	t	52	12	447
4	t	51	12	448
3	t	53	12	449
4	t	50	12	450
5	t	63	12	451
4	t	65	12	452
3	t	54	12	453
3	t	66	12	454
4	t	57	12	455
5	t	56	12	456
4	t	62	13	457
5	t	61	13	458
4	t	60	13	459
3	t	64	13	460
3	t	59	13	461
3	t	55	13	462
3	t	52	13	463
4	t	51	13	464
3	t	53	13	465
2	t	50	13	466
3	t	63	13	467
4	t	65	13	468
3	t	54	13	469
2	t	66	13	470
1	t	57	13	471
5	t	56	13	472
3	t	62	14	473
2	t	61	14	474
1	t	60	14	475
2	t	64	14	476
3	t	59	14	477
4	t	55	14	478
5	t	52	14	479
6	t	51	14	480
5	t	53	14	481
5	t	50	14	482
5	t	63	14	483
5	t	65	14	484
3	t	54	14	485
3	t	66	14	486
3	t	56	14	487
6	t	62	19	488
5	t	61	19	489
4	t	60	19	490
3	t	64	19	491
4	t	59	19	492
5	t	55	19	493
6	t	52	19	494
4	t	51	19	495
5	t	53	19	496
4	t	50	19	497
3	t	63	19	498
4	t	65	19	499
5	t	54	19	500
4	t	66	19	501
4	t	57	19	502
5	t	56	19	503
3	t	62	10	504
3	t	61	10	505
2	t	60	10	506
3	t	64	10	507
5	t	59	10	508
6	t	55	10	509
5	t	52	10	510
4	t	51	10	511
5	t	53	10	512
6	t	50	10	513
2	t	65	10	514
1	t	54	10	515
2	t	66	10	516
3	t	57	10	517
3	t	62	9	518
3	t	61	9	519
3	t	60	9	520
3	t	64	9	521
3	t	59	9	522
3	t	55	9	523
1	t	52	9	524
1	t	51	9	525
1	t	53	9	526
4	t	63	9	527
3	t	65	9	528
3	t	54	9	529
3	t	66	9	530
2	t	57	9	531
1	t	56	9	532
4	t	62	11	533
3	t	61	11	534
4	t	60	11	535
5	t	64	11	536
4	t	59	11	537
3	t	55	11	538
2	t	52	11	539
3	t	51	11	540
4	t	53	11	541
5	t	50	11	542
4	t	63	11	543
3	t	65	11	544
4	t	54	11	545
5	t	66	11	546
4	t	57	11	547
5	t	56	11	548
2	t	61	8	549
3	t	60	8	550
4	t	64	8	551
3	t	59	8	552
2	t	55	8	553
3	t	52	8	554
3	t	51	8	555
3	t	53	8	556
2	t	50	8	557
3	t	63	8	558
4	t	65	8	559
1	t	54	8	560
1	t	66	8	561
6	t	57	8	562
3	t	62	17	563
4	t	61	17	564
3	t	60	17	565
6	t	64	17	566
5	t	59	17	567
6	t	55	17	568
5	t	52	17	569
6	t	51	17	570
5	t	53	17	571
5	t	63	17	572
6	t	65	17	573
5	t	54	17	574
6	t	66	17	575
5	t	57	17	576
6	t	56	17	577
6	t	62	16	578
6	t	61	16	579
6	t	60	16	580
6	t	64	16	581
6	t	59	16	582
5	t	55	16	583
6	t	52	16	584
6	t	51	16	585
5	t	63	16	586
6	t	54	16	587
5	t	66	16	588
4	t	57	16	589
5	t	56	16	590
6	t	62	21	591
5	t	61	21	592
4	t	60	21	593
5	t	64	21	594
6	t	59	21	595
5	t	55	21	596
4	t	52	21	597
5	t	51	21	598
6	t	53	21	599
5	t	50	21	600
4	t	63	21	601
3	t	65	21	602
4	t	54	21	603
5	t	66	21	604
6	t	57	21	605
5	t	56	21	606
5	t	62	18	607
4	t	61	18	608
3	t	60	18	609
4	t	64	18	610
5	t	59	18	611
6	t	55	18	612
5	t	52	18	613
4	t	51	18	614
3	t	53	18	615
4	t	50	18	616
5	t	63	18	617
6	t	65	18	618
5	t	54	18	619
4	t	66	18	620
5	t	57	18	621
6	t	56	18	622
4	t	62	15	623
3	t	61	15	624
4	t	60	15	625
5	t	64	15	626
3	t	52	15	627
2	t	53	15	628
4	t	63	15	629
2	t	54	15	630
2	t	57	15	631
4	f	81	20	632
3	f	67	20	633
3	f	77	20	634
4	f	78	20	635
5	f	72	20	636
4	f	74	20	637
5	f	71	20	638
4	f	79	20	639
3	f	80	20	640
4	f	70	20	641
5	f	73	20	642
6	f	76	20	643
5	f	69	20	644
4	f	75	20	645
3	f	82	20	646
4	f	68	20	647
5	t	81	12	648
4	t	67	12	649
3	t	77	12	650
4	t	78	12	651
5	t	72	12	652
5	t	74	12	653
5	t	71	12	654
6	t	79	12	655
5	t	80	12	656
2	t	70	12	657
3	t	73	12	658
4	t	76	12	659
5	t	69	12	660
4	t	75	12	661
5	t	82	12	662
4	t	68	12	663
5	t	81	13	664
4	t	67	13	665
3	t	77	13	666
4	t	78	13	667
5	t	72	13	668
4	t	74	13	669
3	t	71	13	670
4	t	79	13	671
5	t	80	13	672
4	t	70	13	673
4	t	69	13	674
3	t	75	13	675
4	t	82	13	676
5	t	68	13	677
3	t	67	14	678
4	t	77	14	679
5	t	78	14	680
4	t	72	14	681
3	t	74	14	682
2	t	71	14	683
3	t	79	14	684
4	t	80	14	685
5	t	70	14	686
6	t	73	14	687
5	t	76	14	688
4	t	69	14	689
3	t	75	14	690
4	t	82	14	691
5	t	68	14	692
4	t	81	19	693
4	t	67	19	694
4	t	77	19	695
4	t	78	19	696
5	t	72	19	697
3	t	74	19	698
4	t	71	19	699
5	t	79	19	700
4	t	80	19	701
3	t	70	19	702
4	t	73	19	703
5	t	76	19	704
3	t	75	19	705
4	t	82	19	706
5	t	68	19	707
5	t	81	10	708
4	t	67	10	709
3	t	77	10	710
4	t	78	10	711
5	t	72	10	712
6	t	74	10	713
5	t	71	10	714
4	t	79	10	715
3	t	80	10	716
4	t	70	10	717
5	t	73	10	718
1	t	69	10	719
2	t	75	10	720
3	t	82	10	721
4	t	68	10	722
3	t	81	9	723
2	t	67	9	724
1	t	77	9	725
2	t	78	9	726
3	t	72	9	727
4	t	74	9	728
5	t	71	9	729
2	t	79	9	730
1	t	80	9	731
2	t	70	9	732
3	t	73	9	733
4	t	76	9	734
3	t	69	9	735
4	t	75	9	736
5	t	82	9	737
4	t	68	9	738
3	f	81	11	739
4	f	67	11	740
2	f	77	11	741
2	f	74	11	742
2	f	79	11	743
1	f	80	11	744
4	f	70	11	745
5	f	73	11	746
6	f	76	11	747
3	f	75	11	748
4	f	82	11	749
4	t	81	8	750
3	t	67	8	751
2	t	77	8	752
5	t	78	8	753
6	t	72	8	754
4	t	74	8	755
3	t	71	8	756
5	t	79	8	757
4	t	80	8	758
3	t	70	8	759
4	t	73	8	760
5	t	76	8	761
6	t	69	8	762
2	t	75	8	763
3	t	82	8	764
4	t	68	8	765
5	f	81	17	766
5	f	67	17	767
5	f	77	17	768
5	f	78	17	769
5	f	72	17	770
5	f	74	17	771
5	f	71	17	772
6	f	79	17	773
5	f	70	17	774
5	f	73	17	775
5	f	76	17	776
4	f	69	17	777
6	f	75	17	778
5	f	82	17	779
5	f	68	17	780
5	t	81	16	781
5	t	67	16	782
5	t	77	16	783
5	t	78	16	784
5	t	72	16	785
5	t	74	16	786
5	t	71	16	787
5	t	79	16	788
5	t	80	16	789
5	t	70	16	790
5	t	73	16	791
5	t	76	16	792
5	t	69	16	793
1	t	75	16	794
5	t	82	16	795
3	t	81	21	796
4	t	67	21	797
5	t	77	21	798
4	t	78	21	799
3	t	72	21	800
2	t	74	21	801
4	t	71	21	802
5	t	79	21	803
6	t	80	21	804
5	t	70	21	805
4	t	73	21	806
3	t	76	21	807
5	t	69	21	808
6	t	75	21	809
5	t	82	21	810
6	t	68	21	811
5	t	81	18	812
5	t	67	18	813
5	t	77	18	814
6	t	78	18	815
5	t	72	18	816
4	t	74	18	817
4	t	79	18	818
3	t	80	18	819
4	t	70	18	820
5	t	73	18	821
6	t	76	18	822
3	t	75	18	823
4	t	82	18	824
5	t	68	18	825
5	f	81	15	826
4	f	67	15	827
5	f	77	15	828
6	f	78	15	829
6	f	72	15	830
6	f	74	15	831
6	f	71	15	832
6	f	79	15	833
6	f	80	15	834
6	f	70	15	835
5	f	73	15	836
6	f	76	15	837
5	f	69	15	838
4	f	75	15	839
5	f	82	15	840
6	f	68	15	841
3	t	85	20	842
4	t	91	20	843
3	t	94	20	844
2	t	96	20	845
3	t	95	20	846
4	t	93	20	847
5	t	86	20	848
4	t	84	20	849
3	t	83	20	850
4	t	92	20	851
5	t	87	20	852
6	t	89	20	853
5	t	88	20	854
4	t	98	20	855
5	t	97	20	856
6	t	90	20	857
4	t	85	12	858
5	t	91	12	859
3	t	94	12	860
4	t	96	12	861
5	t	95	12	862
6	t	93	12	863
5	t	86	12	864
4	t	84	12	865
4.5	t	83	12	866
3.142	t	92	12	867
4	t	87	12	868
3	t	89	12	869
2	t	88	12	870
3	t	98	12	871
4	t	97	12	872
5	t	90	12	873
4	t	85	13	874
3	t	91	13	875
4	t	94	13	876
5	t	96	13	877
6	t	95	13	878
4	t	93	13	879
3	t	86	13	880
4	t	84	13	881
5	t	83	13	882
6	t	92	13	883
5	t	87	13	884
4	t	89	13	885
3	t	88	13	886
2	t	98	13	887
3	t	97	13	888
3	t	90	13	889
4	t	85	14	890
4	t	91	14	891
4	t	94	14	892
3	t	96	14	893
4	t	95	14	894
5	t	93	14	895
4	t	86	14	896
3	t	84	14	897
2	t	83	14	898
3	t	92	14	899
4	t	87	14	900
5	t	89	14	901
4	t	88	14	902
4	t	98	14	903
4	t	97	14	904
3	t	90	14	905
4	t	85	19	906
4	t	91	19	907
4	t	94	19	908
4	t	96	19	909
4	t	95	19	910
4	t	93	19	911
3	t	86	19	912
2	t	84	19	913
1	t	83	19	914
1	t	92	19	915
6	t	87	19	916
5	t	89	19	917
4	t	88	19	918
3	t	98	19	919
2	t	97	19	920
3	t	90	19	921
4	t	85	10	922
3	t	91	10	923
4	t	94	10	924
5	t	96	10	925
4	t	95	10	926
3	t	93	10	927
2	t	86	10	928
3	t	84	10	929
4	t	83	10	930
5	t	92	10	931
4	t	87	10	932
3	t	89	10	933
4	t	88	10	934
3	t	98	10	935
2	t	97	10	936
3	t	90	10	937
4	t	85	9	938
4	t	91	9	939
4	t	94	9	940
3	t	96	9	941
2	t	95	9	942
3	t	93	9	943
4	t	86	9	944
5	t	84	9	945
4	t	83	9	946
3	t	92	9	947
3	t	87	9	948
3	t	89	9	949
3	t	88	9	950
5	t	98	9	951
4	t	97	9	952
6	t	90	9	953
3	f	96	11	954
2	f	95	11	955
1	f	93	11	956
2	f	86	11	957
2	f	84	11	958
4	f	83	11	959
5	f	92	11	960
6	f	87	11	961
3	f	98	11	962
2	f	97	11	963
1	f	90	11	964
4	t	85	8	965
4	t	91	8	966
3	t	94	8	967
1	t	96	8	968
1	t	95	8	969
2	t	93	8	970
1	t	86	8	971
3	t	84	8	972
4	t	83	8	973
2	t	92	8	974
1	t	87	8	975
6	t	89	8	976
5	t	88	8	977
1	t	98	8	978
2	t	97	8	979
3	f	85	17	980
2	f	91	17	981
1	f	94	17	982
3	f	96	17	983
4	f	95	17	984
6	f	93	17	985
5	f	86	17	986
6	f	84	17	987
5	f	83	17	988
4	f	92	17	989
5	f	87	17	990
5	f	89	17	991
5	f	88	17	992
6	f	98	17	993
5	f	97	17	994
4	f	90	17	995
6	t	85	16	996
6	t	91	16	997
6	t	94	16	998
6	t	96	16	999
5	t	95	16	1000
4	t	93	16	1001
5	t	86	16	1002
5	t	84	16	1003
5	t	83	16	1004
6	t	92	16	1005
5	t	87	16	1006
6	t	89	16	1007
6	t	88	16	1008
6	t	98	16	1009
6	t	97	16	1010
5	t	90	16	1011
4	f	95	21	1012
5	f	93	21	1013
3	f	92	21	1014
6	f	88	21	1015
4	t	85	18	1016
3	t	91	18	1017
4	t	94	18	1018
5	t	96	18	1019
4	t	95	18	1020
3	t	93	18	1021
4	t	86	18	1022
5	t	84	18	1023
6	t	83	18	1024
5	t	92	18	1025
4	t	87	18	1026
5	t	89	18	1027
1	t	88	18	1028
6	t	97	18	1029
4	t	90	18	1030
5	t	85	15	1031
4	t	91	15	1032
3	t	94	15	1033
4	t	96	15	1034
5	t	95	15	1035
5	t	93	15	1036
5	t	86	15	1037
6	t	84	15	1038
1	t	92	15	1039
6	t	87	15	1040
6	t	89	15	1041
6	t	88	15	1042
5	t	98	15	1043
6	t	97	15	1044
4	t	113	20	1045
3	t	103	20	1046
2	t	99	20	1047
3	t	107	20	1048
4	t	108	20	1049
5	t	109	20	1050
4	t	110	20	1051
3	t	114	20	1052
4	t	115	20	1053
5	t	116	20	1054
6	t	106	20	1055
5	t	104	20	1056
4	t	101	20	1057
3	t	111	20	1058
4	t	102	20	1059
4	t	105	20	1060
4	t	112	20	1061
4	t	100	20	1062
5	t	113	12	1063
5	t	103	12	1064
5	t	99	12	1065
4	t	107	12	1066
5	t	108	12	1067
6	t	109	12	1068
5	t	110	12	1069
4	t	114	12	1070
3	t	115	12	1071
4	t	116	12	1072
5	t	106	12	1073
4	t	104	12	1074
3	t	101	12	1075
4	t	111	12	1076
5	t	102	12	1077
5	t	105	12	1078
5	t	112	12	1079
5	t	100	12	1080
5	f	113	13	1081
5	f	103	13	1082
5	f	99	13	1083
4	f	107	13	1084
3	f	108	13	1085
4	f	109	13	1086
1	f	104	13	1087
5	f	101	13	1088
4	f	111	13	1089
5	f	102	13	1090
6	f	105	13	1091
5	f	112	13	1092
6	f	100	13	1093
4	t	113	14	1094
4	t	103	14	1095
4	t	99	14	1096
5	t	107	14	1097
4	t	108	14	1098
3	t	109	14	1099
4	t	110	14	1100
5	t	114	14	1101
4	t	115	14	1102
3	t	116	14	1103
4	t	106	14	1104
5	t	104	14	1105
5	t	101	14	1106
5	t	111	14	1107
1	t	105	14	1108
1	t	112	14	1109
2	t	100	14	1110
2	f	113	19	1111
3	f	103	19	1112
2	f	99	19	1113
3	f	107	19	1114
4	f	108	19	1115
3	f	109	19	1116
2	f	110	19	1117
3	f	114	19	1118
2	f	115	19	1119
1	f	116	19	1120
2	f	106	19	1121
3	f	104	19	1122
4	f	101	19	1123
3	f	111	19	1124
3	f	102	19	1125
4	f	105	19	1126
5	f	112	19	1127
4	f	100	19	1128
4	t	113	10	1129
3	t	103	10	1130
2	t	99	10	1131
3	t	107	10	1132
4	t	108	10	1133
5	t	109	10	1134
6	t	110	10	1135
5	t	114	10	1136
4	t	115	10	1137
3	t	116	10	1138
4	t	106	10	1139
4	t	104	10	1140
4	t	101	10	1141
5	t	111	10	1142
6	t	102	10	1143
1	t	105	10	1144
4	t	100	10	1145
4	t	113	9	1146
5	t	103	9	1147
4	t	99	9	1148
3	t	107	9	1149
4	t	108	9	1150
5	t	109	9	1151
5	t	110	9	1152
4	t	114	9	1153
3	t	115	9	1154
3	t	116	9	1155
3	t	106	9	1156
4	t	104	9	1157
3	t	101	9	1158
2	t	111	9	1159
3	t	102	9	1160
4	t	105	9	1161
3	t	112	9	1162
2	t	100	9	1163
5	f	113	11	1164
4	f	103	11	1165
3	f	99	11	1166
2	f	107	11	1167
4	f	108	11	1168
3	f	114	11	1169
4	f	115	11	1170
1	f	106	11	1171
2	f	104	11	1172
3	f	101	11	1173
4	f	111	11	1174
4	f	102	11	1175
4	f	105	11	1176
5	f	112	11	1177
5	f	100	11	1178
3	t	113	8	1179
3	t	103	8	1180
2	t	99	8	1181
4	t	107	8	1182
5	t	108	8	1183
4	t	109	8	1184
3	t	110	8	1185
2	t	114	8	1186
3	t	115	8	1187
4	t	116	8	1188
3	t	106	8	1189
3	t	104	8	1190
4	t	101	8	1191
5	t	111	8	1192
4	t	102	8	1193
5	t	105	8	1194
6	t	112	8	1195
5	t	100	8	1196
4	f	113	17	1197
4	f	103	17	1198
3	f	99	17	1199
4	f	107	17	1200
3	f	108	17	1201
2	f	109	17	1202
3	f	110	17	1203
6	f	114	17	1204
6	f	115	17	1205
6	f	116	17	1206
5	f	106	17	1207
6	f	104	17	1208
5	f	101	17	1209
5	f	102	17	1210
5	f	105	17	1211
5	f	112	17	1212
5	f	100	17	1213
6	f	109	16	1214
6	f	110	16	1215
6	f	114	16	1216
6	f	115	16	1217
1	f	116	16	1218
6	f	102	16	1219
6	f	105	16	1220
6	f	112	16	1221
6	f	100	16	1222
5	f	113	21	1223
5	f	103	21	1224
5	f	99	21	1225
5	f	107	21	1226
5	f	108	21	1227
4	f	109	21	1228
5	f	110	21	1229
5	f	114	21	1230
5	f	115	21	1231
5	f	116	21	1232
5	f	106	21	1233
4	f	104	21	1234
6	f	105	21	1235
5	f	112	21	1236
5	f	100	21	1237
5	f	113	18	1238
4	f	103	18	1239
3	f	99	18	1240
4	f	107	18	1241
5	f	108	18	1242
6	f	109	18	1243
5	f	110	18	1244
4	f	114	18	1245
5	f	115	18	1246
5	f	116	18	1247
5	f	106	18	1248
5	f	112	18	1249
4	f	100	18	1250
5	f	113	15	1251
4	f	103	15	1252
5	f	99	15	1253
4	f	107	15	1254
5	f	108	15	1255
5	f	109	15	1256
5	f	110	15	1257
5	f	114	15	1258
5	f	115	15	1259
5	f	116	15	1260
5	f	106	15	1261
5	f	104	15	1262
5	f	101	15	1263
5	f	111	15	1264
5	f	102	15	1265
4	f	105	15	1266
1	f	100	15	1267
4	t	122	20	1268
3	t	123	20	1269
4	t	121	20	1270
4	t	136	20	1271
4	t	135	20	1272
4	t	132	20	1273
3	t	129	20	1274
4	t	118	20	1275
5	t	119	20	1276
4	t	117	20	1277
3	t	128	20	1278
4	t	125	20	1279
5	t	131	20	1280
4	t	126	20	1281
5	t	124	20	1282
6	t	133	20	1283
5	t	130	20	1284
4	t	134	20	1285
3	t	120	20	1286
4	t	127	20	1287
4	t	122	12	1288
4	t	123	12	1289
5	t	121	12	1290
4	t	136	12	1291
3	t	135	12	1292
4	t	132	12	1293
5	t	129	12	1294
4	t	118	12	1295
3	t	119	12	1296
4	t	117	12	1297
5	t	128	12	1298
6	t	125	12	1299
3	t	131	12	1300
4	t	126	12	1301
4	t	124	12	1302
5	t	133	12	1303
6	t	130	12	1304
3	t	134	12	1305
4	t	120	12	1306
5	t	127	12	1307
5	t	122	13	1308
4	t	123	13	1309
3	t	121	13	1310
4	t	136	13	1311
5	t	135	13	1312
3	t	132	13	1313
2	t	129	13	1314
1	t	118	13	1315
2	t	119	13	1316
4	t	117	13	1317
2	t	128	13	1318
3	t	125	13	1319
3	t	131	13	1320
4	t	126	13	1321
5	t	124	13	1322
4	t	133	13	1323
3	t	130	13	1324
3	t	134	13	1325
3	t	120	13	1326
3	t	127	13	1327
4	t	122	14	1328
3	t	123	14	1329
2	t	121	14	1330
3	t	136	14	1331
4	t	135	14	1332
3	t	132	14	1333
2	t	129	14	1334
2	t	118	14	1335
3	t	119	14	1336
4	t	117	14	1337
3	t	131	14	1338
4	t	126	14	1339
3	t	124	14	1340
2	t	133	14	1341
3	t	130	14	1342
4	t	134	14	1343
5	t	120	14	1344
4	t	127	14	1345
4	t	122	19	1346
3	t	123	19	1347
2	t	121	19	1348
3	t	136	19	1349
3	t	135	19	1350
3	t	132	19	1351
4	t	129	19	1352
3	t	118	19	1353
2	t	119	19	1354
3	t	117	19	1355
4	t	128	19	1356
5	t	125	19	1357
5	t	131	19	1358
5	t	126	19	1359
5	t	124	19	1360
4	t	133	19	1361
3	t	130	19	1362
3	t	134	19	1363
2	t	120	19	1364
2	t	127	19	1365
4	t	122	10	1366
4	t	123	10	1367
4	t	121	10	1368
5	t	136	10	1369
6	t	135	10	1370
5	t	132	10	1371
4	t	129	10	1372
3	t	118	10	1373
3	t	119	10	1374
4	t	117	10	1375
5	t	128	10	1376
4	t	125	10	1377
3	t	131	10	1378
4	t	126	10	1379
4	t	124	10	1380
4	t	133	10	1381
4	t	130	10	1382
5	t	134	10	1383
5	t	120	10	1384
5	t	127	10	1385
5	t	122	9	1386
5	t	123	9	1387
5	t	121	9	1388
4	t	136	9	1389
3	t	135	9	1390
2	t	132	9	1391
2	t	129	9	1392
2	t	118	9	1393
1	t	119	9	1394
2	t	117	9	1395
2	t	128	9	1396
3	t	125	9	1397
4	t	131	9	1398
4	t	126	9	1399
4	t	124	9	1400
3	t	133	9	1401
2	t	130	9	1402
2	t	134	9	1403
3	t	120	9	1404
3	t	127	9	1405
4	t	122	11	1406
4	t	123	11	1407
4	t	121	11	1408
3	t	136	11	1409
2	t	135	11	1410
2	t	132	11	1411
3	t	129	11	1412
3	t	118	11	1413
4	t	119	11	1414
4	t	117	11	1415
4	t	128	11	1416
3	t	125	11	1417
4	t	131	11	1418
4	t	126	11	1419
5	t	124	11	1420
4	t	133	11	1421
3	t	130	11	1422
2	t	134	11	1423
2	t	120	11	1424
2	t	127	11	1425
5	t	122	8	1426
5	t	123	8	1427
5	t	121	8	1428
5	t	136	8	1429
5	t	135	8	1430
5	t	132	8	1431
5	t	129	8	1432
3	t	118	8	1433
3	t	119	8	1434
3	t	117	8	1435
3	t	128	8	1436
3	t	125	8	1437
3	t	131	8	1438
3	t	126	8	1439
2	t	124	8	1440
2	t	133	8	1441
2	t	130	8	1442
3	t	134	8	1443
4	t	120	8	1444
4	t	127	8	1445
4	t	122	17	1446
4	t	123	17	1447
4	t	121	17	1448
3	t	136	17	1449
2	t	135	17	1450
3	t	132	17	1451
4	t	129	17	1452
5	t	118	17	1453
5	t	119	17	1454
4	t	131	17	1455
3	t	126	17	1456
2	t	124	17	1457
1	t	133	17	1458
6	t	130	17	1459
5	t	134	17	1460
5	t	120	17	1461
5	t	127	17	1462
5	t	122	16	1463
5	t	123	16	1464
5	t	121	16	1465
5	t	136	16	1466
5	t	135	16	1467
5	t	132	16	1468
4	t	129	16	1469
5	t	118	16	1470
6	t	119	16	1471
6	t	117	16	1472
6	t	130	16	1473
6	t	134	16	1474
6	t	120	16	1475
1	t	127	16	1476
5	f	122	21	1477
5	f	123	21	1478
5	f	121	21	1479
4	f	136	21	1480
3	f	135	21	1481
3	f	132	21	1482
4	f	129	21	1483
5	f	118	21	1484
4	f	119	21	1485
4	f	117	21	1486
6	f	128	21	1487
6	f	125	21	1488
5	f	131	21	1489
4	f	126	21	1490
4	f	124	21	1491
4	f	133	21	1492
4	f	130	21	1493
6	f	134	21	1494
6	f	120	21	1495
6	f	127	21	1496
5	f	122	18	1497
5	f	123	18	1498
5	f	121	18	1499
5	f	136	18	1500
5	f	135	18	1501
5	f	132	18	1502
5	f	129	18	1503
5	f	118	18	1504
4	f	119	18	1505
3	f	117	18	1506
3	f	128	18	1507
3	f	125	18	1508
4	f	131	18	1509
5	f	126	18	1510
4	f	124	18	1511
4	f	133	18	1512
3	f	130	18	1513
2	f	134	18	1514
3	f	120	18	1515
4	f	127	18	1516
5	t	122	15	1517
4	t	123	15	1518
3	t	121	15	1519
4	t	136	15	1520
5	t	135	15	1521
4	t	132	15	1522
3	t	129	15	1523
4	t	118	15	1524
5	t	119	15	1525
3	t	131	15	1526
3	t	126	15	1527
3	t	124	15	1528
3	t	133	15	1529
4	t	130	15	1530
1	t	127	15	1531
3	t	58	20	1532
\.


--
-- Data for TOC entry 37 (OID 18913)
-- Name: tabelka; Type: TABLE DATA; Schema: public; Owner: morgoth
--

COPY tabelka (pk, fk, str, flo) FROM stdin;
\.


--
-- TOC entry 28 (OID 18866)
-- Name: przedmiot_pkey; Type: CONSTRAINT; Schema: public; Owner: morgoth
--

ALTER TABLE ONLY przedmiot
    ADD CONSTRAINT przedmiot_pkey PRIMARY KEY (idprzedmiotu);


--
-- TOC entry 29 (OID 18868)
-- Name: klasa_pkey; Type: CONSTRAINT; Schema: public; Owner: morgoth
--

ALTER TABLE ONLY klasa
    ADD CONSTRAINT klasa_pkey PRIMARY KEY (idklasy);


--
-- TOC entry 30 (OID 18870)
-- Name: uczen_pkey; Type: CONSTRAINT; Schema: public; Owner: morgoth
--

ALTER TABLE ONLY uczen
    ADD CONSTRAINT uczen_pkey PRIMARY KEY (iducznia);


--
-- TOC entry 31 (OID 18872)
-- Name: ocena_pkey; Type: CONSTRAINT; Schema: public; Owner: morgoth
--

ALTER TABLE ONLY ocena
    ADD CONSTRAINT ocena_pkey PRIMARY KEY (idoceny);


--
-- TOC entry 32 (OID 18919)
-- Name: tabelka_pkey; Type: CONSTRAINT; Schema: public; Owner: morgoth
--

ALTER TABLE ONLY tabelka
    ADD CONSTRAINT tabelka_pkey PRIMARY KEY (pk);


--
-- TOC entry 38 (OID 18874)
-- Name: $1; Type: FK CONSTRAINT; Schema: public; Owner: morgoth
--

ALTER TABLE ONLY uczen
    ADD CONSTRAINT "$1" FOREIGN KEY (idklasy) REFERENCES klasa(idklasy) ON DELETE CASCADE;


--
-- TOC entry 39 (OID 18878)
-- Name: $1; Type: FK CONSTRAINT; Schema: public; Owner: morgoth
--

ALTER TABLE ONLY ocena
    ADD CONSTRAINT "$1" FOREIGN KEY (iducznia) REFERENCES uczen(iducznia) ON DELETE CASCADE;


--
-- TOC entry 40 (OID 18882)
-- Name: $2; Type: FK CONSTRAINT; Schema: public; Owner: morgoth
--

ALTER TABLE ONLY ocena
    ADD CONSTRAINT "$2" FOREIGN KEY (idprzedmiotu) REFERENCES przedmiot(idprzedmiotu) ON DELETE CASCADE;


--
-- TOC entry 41 (OID 18921)
-- Name: $1; Type: FK CONSTRAINT; Schema: public; Owner: morgoth
--

ALTER TABLE ONLY tabelka
    ADD CONSTRAINT "$1" FOREIGN KEY (fk) REFERENCES klasa(idklasy);


--
-- TOC entry 6 (OID 17143)
-- Name: klasa_seq; Type: SEQUENCE SET; Schema: public; Owner: morgoth
--

SELECT pg_catalog.setval('klasa_seq', 16, true);


--
-- TOC entry 8 (OID 17145)
-- Name: przedmiot_seq; Type: SEQUENCE SET; Schema: public; Owner: morgoth
--

SELECT pg_catalog.setval('przedmiot_seq', 24, true);


--
-- TOC entry 10 (OID 17147)
-- Name: uczen_seq; Type: SEQUENCE SET; Schema: public; Owner: morgoth
--

SELECT pg_catalog.setval('uczen_seq', 139, true);


--
-- TOC entry 12 (OID 17149)
-- Name: ocena_seq; Type: SEQUENCE SET; Schema: public; Owner: morgoth
--

SELECT pg_catalog.setval('ocena_seq', 1532, true);


--
-- TOC entry 3 (OID 2200)
-- Name: SCHEMA public; Type: COMMENT; Schema: -; Owner: morgoth
--

COMMENT ON SCHEMA public IS 'Standard public schema';


