
/*  FOR THE GLORY OF ALL   *** UNIX ***   SYSTEMS  */


#define ATTRIB        11
#define S_ATTRIB      11
#define MAX_ATT       16
#define COMMON        2
#define MAX_TYPE      250
#define TRUE          1
#define FALSE         0
#define MAX_SKI       550
#define MULTIPLE      0
#define ATTRIBUTE     1
#define MOD           2
#define DIFF          3
#define CATEG         4
#define MAGIC         5
#define MAX_CATEG     15
#define MAX_LEVELS    35
#define MAX_EKW       100
#define MAX_EKW_P     50
#define DANE_POSTACI  15
#define IMIE          0
#define NAZWISKO      1
#define RASA          2
#define PROFESJA      3
#define CHARAKTER     4
#define OCZY          5
#define WLOSY         6
#define CERA          7
#define WZROST        8
#define WAGA          9
#define PLEC          10
#define WIEK           11
#define MAX_UM        125
#define ALL           2
#define DIR_EXEC      0777
#define MAX_LEV_S     25
#define MAX_SPELL     40
#define SHSTR         40
#define MEDSTR        40
#define USER          -1
#define ASK	      -2
#define COST          0x200
#define REQ           0x201
#define MAX_SP        125
#define BIG           200
#define SECRET        0xfade
#define HIDDEN        0xfada

struct DATA
{
 DATA();
 ~DATA();
 int ilosc[ATTRIB];
 int mod[COMMON][MAX_TYPE][MAX_ATT];
 char name[COMMON][MAX_TYPE][STRING];
 int value_att[MAX_ATT];
 char name_att[MAX_ATT][STRING];
 char s_name[MAX_SKI][STRING];
 int s_mod[MAX_SKI][S_ATTRIB];
 char c_name[MAX_CATEG][STRING];
 int cost[MAX_LEVELS];
 int pd_cost[MAX_LEVELS];
 int sv;
 int st;
 int p_mod[MAX_ATT];
 int lr;
 int mm;
 char dbname[STRING];
 int ilosc_ekw;
 char ekw[MAX_EKW][STRING];
 int c_ekw[MAX_EKW];
 int def_cash;

 int ilosc_poziomow;
 int ilosc_czarow[MAX_LEV_S];
 int spell_cost[MAX_LEV_S][MAX_SPELL];
 char req_magic[MAX_LEV_S][MAX_SPELL][SHSTR];
 char zaklecie[MAX_LEV_S][MAX_SPELL][MEDSTR];
};



DATA :: DATA()
{
#ifdef DEBUG
  printf("DEBUG_DBASE_H: DATA::DATA()\n");
#endif
 ilosc_poziomow = 0;
 for (int i=0;i<MAX_LEV_S;i++)
        {
         ilosc_czarow[i] = 0;
         for (int j=0;j<MAX_SPELL;j++)
                {
                 spell_cost[i][j] =(i+1) *(i+2) + j;
                 strcpy(req_magic[i][j],"RENEGACJA");
                 strcpy(zaklecie[i][j],"SOME_SPELL");
                }
        }
 ilosc[0] = 13;
 ilosc[1] = 10;
 ilosc[2] = 20;
 ilosc[3] = 150;
 ilosc[4] = 6;
 ilosc[5] = 15;
 ilosc[6] = 15;
 ilosc_ekw = 10;
 sv = 0;
 st = 1;
 lr = 0;
 mm = 0;
 strcpy(dbname,"DataBase/default");
 for (int i=0;i<MAX_ATT;i++)
        {
         value_att[i] = 10;
         p_mod[i]     = 0;
         strcpy(name_att[i],"DEFAULT_ATTR");
        }
 for (int i=0;i<MAX_TYPE;i++)
        {
         strcpy(name[0][i],"DEFAULT_RACE");
         strcpy(name[1][i],"DEFAULT_PROF");
         for (int j=0;j<MAX_ATT;j++)
                {
                 mod[0][i][j] = 0;
                 mod[1][i][j] = 0;
                }
        }
 for (int i=0;i<MAX_SKI;i++)
        {
         strcpy(s_name[i],"DEFAULT_SKILL");
         s_mod[i][MULTIPLE] = 3;
         s_mod[i][ATTRIBUTE] = 0;
         s_mod[i][MOD] = 0;
         s_mod[i][DIFF] = 1;
         s_mod[i][CATEG] = 0;
         s_mod[i][MAGIC] = 0;
        }
 for (int i=0;i<MAX_CATEG;i++)
        {
         strcpy(c_name[i],"DEFAULT_CATEG");
        }
 for (int i=0;i<MAX_LEVELS;i++) {cost[i] = 2*i+1;pd_cost[i] = cost[i];}
 for (int i=0;i<MAX_EKW;i++) {strcpy(ekw[i],"DEFAULT_ITEM"); c_ekw[i] = 40;}
 def_cash = 400;
#ifdef DEBUG
  printf("DEBUG_DBASE_H: OK\n");
#endif
}
DATA::~DATA()
{
#ifdef DEBUG
  printf("DEBUG_DBASE_H: DATA::~DATA()\n");
#endif
}
struct RPG
{
 RPG();
 ~RPG();
 int ilosc_danych;
 int ilosc_cech;
 int sv;
 int st;
 int cecha[MAX_ATT];
 char n_cechy[MAX_ATT][STRING];
 char dane[DANE_POSTACI][STRING];
 int il_um;
 char um[MAX_UM][STRING];
 int pozum[MAX_UM];
 int tesum[MAX_UM];
 int trum[MAX_UM];
 int ceum[MAX_UM];
 int mulum[MAX_UM];
 int magum[MAX_UM];
 int modum[MAX_UM];
 int il_ekw;
 int PD;
 int p_c[MAX_ATT];
 int p_u[MAX_UM];
 char ekw[MAX_EKW_P][STRING];
 int rl;
 int stMM;
 int numMM;
 int IsDead;
 int lev;
 int lev_p;
 char pname[STRING];
 int cash;

 int il_spell;
 int sp;
 int spt;
 int max_lev;
 char spell[MAX_SP][MEDSTR];
 int spell_cost[MAX_SP];
 int spell_level[MAX_SP];
 int mage;
 int randomedAtt;
 int modAtt;
 int modSkill;
 int modEkw;
 int modSpell;
 char used_base[STRING];
 int MOC;
};
RPG :: RPG()
{
#ifdef DEBUG
  printf("DEBUG_DBASE_H: RPG::RPG()\n");
#endif
 ilosc_danych = 12;
 ilosc_cech = 13;
 randomedAtt = 0;
 modAtt = 0;
 modSkill = 0;
 modEkw = 0;
 modSpell = 0;
 numMM = 0;
 strcpy(pname,"Characters/default");
 sv = 0;
 st = 1;
 mage = 0;
 cash = 100;
 il_um = 0;
 il_ekw = 0;
 PD = 0;
 rl = 0;
 lev = 0;
 lev_p = 0;
 IsDead = 0;
 sp = 10;
 spt = 2;
 il_spell = 0;
 max_lev = 0;
 MOC = 0;
 strcpy(used_base, "DataBase/morgoth");
 for (int i=0;i<MAX_SP;i++)
        {
         strcpy(spell[i],"SOME_SPELL");
         spell_cost[i] = 5;
         spell_level[i] = 1;
        }
 for (int i=0;i<DANE_POSTACI;i++)
        {
         strcpy(dane[i],"SOME_DATA");
        }
 for (int i=0;i<MAX_ATT;i++)
        {
         cecha[i] = 10;
         p_c[i] = 0;
         strcpy(n_cechy[i],"SOME_DATA");
        }
 stMM = 10;
 for (int i=0;i<MAX_UM;i++)
        {
         strcpy(um[i],"SOME_SKILL");
         tesum[i] = 0;
         pozum[i] = 0;
         trum[i]  = 1;
         mulum[i] = 3;
         ceum[i]  = 0;
         p_u[i]   = 0;
         magum[i] = 0;
         modum[i] = 0;
        }
 for (int i=0;i<MAX_EKW_P;i++)
        {
         strcpy(ekw[i],"SOME_EQUIP");
        }
#ifdef DEBUG
  printf("DEBUG_DBASE_H: OK\n");
#endif
}
RPG::~RPG()
{
#ifdef DEBUG
  printf("DEBUG_DBASE_H: RPG::~RPG()\n");
#endif
}
