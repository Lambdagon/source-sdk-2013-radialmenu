#pragma once
#include "cbase.h"
#include "utldict.h"
#include "utlvector.h"
#include <string>

struct PopulationEntry_t { CUtlString name; int weight; };
struct PopulationGroup_t { CUtlVector<PopulationEntry_t> entries; };

class CZombiePopulationManager
{
public:
    void Init(const char *file);
    const char *SelectRandom(const char *group);

private:
    CUtlDict<PopulationGroup_t*, int> groups;

    void ParseBlock(std::istream &file, std::string &line, PopulationGroup_t *pGroup=nullptr);
    std::string Trim(const std::string &s);
};

extern CZombiePopulationManager g_ZombiePopulationManager;

class CInfoZombieSpawn : public CBaseEntity
{
public:
    DECLARE_CLASS(CInfoZombieSpawn, CBaseEntity);
    DECLARE_DATADESC();
    void Spawn() override;
    void InputSpawnZombie(inputdata_t &inputdata);

private:
    string_t m_iszPopulation;
};
