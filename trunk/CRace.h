#ifndef H_CRACE
#define H_CRACE

struct SRacePOD
{
	int m_id;
	string m_name;
};

class CRace
{
	CRace() {};
	int onUpdate() { return 0; };
	~CRace() {};
};

#endif