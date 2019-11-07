--
-- PostgreSQL database dump
--

SET SESSION AUTHORIZATION 'morgoth';

--
-- TOC entry 3 (OID 2200)
-- Name: public; Type: ACL; Schema: -; Owner: morgoth
--

REVOKE ALL ON SCHEMA public FROM PUBLIC;
GRANT ALL ON SCHEMA public TO PUBLIC;


SET search_path = public, pg_catalog;

--
-- TOC entry 4 (OID 17492)
-- Name: klasa_seq; Type: SEQUENCE; Schema: public; Owner: morgoth
--

CREATE SEQUENCE klasa_seq
    INCREMENT BY 1
    NO MAXVALUE
    MINVALUE 0
    CACHE 1;


--
-- TOC entry 6 (OID 17494)
-- Name: przedmiot_seq; Type: SEQUENCE; Schema: public; Owner: morgoth
--

CREATE SEQUENCE przedmiot_seq
    INCREMENT BY 1
    NO MAXVALUE
    MINVALUE 0
    CACHE 1;


--
-- TOC entry 8 (OID 17496)
-- Name: uczen_seq; Type: SEQUENCE; Schema: public; Owner: morgoth
--

CREATE SEQUENCE uczen_seq
    INCREMENT BY 1
    NO MAXVALUE
    MINVALUE 0
    CACHE 1;


--
-- TOC entry 10 (OID 17498)
-- Name: ocena_seq; Type: SEQUENCE; Schema: public; Owner: morgoth
--

CREATE SEQUENCE ocena_seq
    START WITH 0
    INCREMENT BY 1
    NO MAXVALUE
    MINVALUE 0
    CACHE 1;


--
-- TOC entry 12 (OID 17513)
-- Name: przedmiot; Type: TABLE; Schema: public; Owner: morgoth
--

CREATE TABLE przedmiot (
    nazwa text NOT NULL,
    idprzedmiotu integer DEFAULT nextval('"przedmiot_seq"'::text) NOT NULL
);


--
-- TOC entry 13 (OID 17521)
-- Name: klasa; Type: TABLE; Schema: public; Owner: morgoth
--

CREATE TABLE klasa (
    nazwa text NOT NULL,
    idklasy integer DEFAULT nextval('"klasa_seq"'::text) NOT NULL
);


--
-- TOC entry 14 (OID 17526)
-- Name: uczen; Type: TABLE; Schema: public; Owner: morgoth
--

CREATE TABLE uczen (
    imie text NOT NULL,
    nazwisko text NOT NULL,
    idklasy integer NOT NULL,
    iducznia integer DEFAULT nextval('"uczen_seq"'::text) NOT NULL
);


--
-- TOC entry 15 (OID 17579)
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
-- Data for TOC entry 20 (OID 17513)
-- Name: przedmiot; Type: TABLE DATA; Schema: public; Owner: morgoth
--

COPY przedmiot (nazwa, idprzedmiotu) FROM stdin;
\.


--
-- Data for TOC entry 21 (OID 17521)
-- Name: klasa; Type: TABLE DATA; Schema: public; Owner: morgoth
--

COPY klasa (nazwa, idklasy) FROM stdin;
\.


--
-- Data for TOC entry 22 (OID 17526)
-- Name: uczen; Type: TABLE DATA; Schema: public; Owner: morgoth
--

COPY uczen (imie, nazwisko, idklasy, iducznia) FROM stdin;
\.


--
-- Data for TOC entry 23 (OID 17579)
-- Name: ocena; Type: TABLE DATA; Schema: public; Owner: morgoth
--

COPY ocena (ocena, czyliczona, iducznia, idprzedmiotu, idoceny) FROM stdin;
\.


--
-- TOC entry 16 (OID 17519)
-- Name: przedmiot_pkey; Type: CONSTRAINT; Schema: public; Owner: morgoth
--

ALTER TABLE ONLY przedmiot
    ADD CONSTRAINT przedmiot_pkey PRIMARY KEY (idprzedmiotu);


--
-- TOC entry 17 (OID 17524)
-- Name: klasa_pkey; Type: CONSTRAINT; Schema: public; Owner: morgoth
--

ALTER TABLE ONLY klasa
    ADD CONSTRAINT klasa_pkey PRIMARY KEY (idklasy);


--
-- TOC entry 18 (OID 17532)
-- Name: uczen_pkey; Type: CONSTRAINT; Schema: public; Owner: morgoth
--

ALTER TABLE ONLY uczen
    ADD CONSTRAINT uczen_pkey PRIMARY KEY (iducznia);


--
-- TOC entry 19 (OID 17582)
-- Name: ocena_pkey; Type: CONSTRAINT; Schema: public; Owner: morgoth
--

ALTER TABLE ONLY ocena
    ADD CONSTRAINT ocena_pkey PRIMARY KEY (idoceny);


--
-- TOC entry 24 (OID 17534)
-- Name: $1; Type: FK CONSTRAINT; Schema: public; Owner: morgoth
--

ALTER TABLE ONLY uczen
    ADD CONSTRAINT "$1" FOREIGN KEY (idklasy) REFERENCES klasa(idklasy) ON DELETE CASCADE;


--
-- TOC entry 25 (OID 17584)
-- Name: $1; Type: FK CONSTRAINT; Schema: public; Owner: morgoth
--

ALTER TABLE ONLY ocena
    ADD CONSTRAINT "$1" FOREIGN KEY (iducznia) REFERENCES uczen(iducznia) ON DELETE CASCADE;


--
-- TOC entry 26 (OID 17588)
-- Name: $2; Type: FK CONSTRAINT; Schema: public; Owner: morgoth
--

ALTER TABLE ONLY ocena
    ADD CONSTRAINT "$2" FOREIGN KEY (idprzedmiotu) REFERENCES przedmiot(idprzedmiotu) ON DELETE CASCADE;


--
-- TOC entry 5 (OID 17492)
-- Name: klasa_seq; Type: SEQUENCE SET; Schema: public; Owner: morgoth
--

SELECT pg_catalog.setval('klasa_seq', 5, true);


--
-- TOC entry 7 (OID 17494)
-- Name: przedmiot_seq; Type: SEQUENCE SET; Schema: public; Owner: morgoth
--

SELECT pg_catalog.setval('przedmiot_seq', 6, true);


--
-- TOC entry 9 (OID 17496)
-- Name: uczen_seq; Type: SEQUENCE SET; Schema: public; Owner: morgoth
--

SELECT pg_catalog.setval('uczen_seq', 14, true);


--
-- TOC entry 11 (OID 17498)
-- Name: ocena_seq; Type: SEQUENCE SET; Schema: public; Owner: morgoth
--

SELECT pg_catalog.setval('ocena_seq', 0, false);


--
-- TOC entry 2 (OID 2200)
-- Name: SCHEMA public; Type: COMMENT; Schema: -; Owner: morgoth
--

COMMENT ON SCHEMA public IS 'Standard public schema';


