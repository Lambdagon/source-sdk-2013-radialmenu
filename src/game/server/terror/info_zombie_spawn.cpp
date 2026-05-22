//========================================================================//
// info_zombie_spawn from left 4 dead 1
// It is used to spawn a zombie manually.
// this is an attempted recreation of it based on the info given on the
// Valve Developer Wiki https://developer.valvesoftware.com/wiki/Info_zombie_spawn
// and the FGD file from Left 4 Dead 1. In a test map using "church" population
// the spawning worked for the specials defined in the population.txt file under the church
// section. Might need more work yet? - Vvis :3 
//========================================================================//
#include "cbase.h"
#include "info_zombie_spawn.h"
#include "filesystem.h"
#include <sstream>
#include <algorithm>

CZombiePopulationManager g_ZombiePopulationManager;

std::string CZombiePopulationManager::Trim(const std::string &s)
{
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end = s.find_last_not_of(" \t\r\n");
    return (start==std::string::npos)?"":s.substr(start,end-start+1);
}

void CZombiePopulationManager::Init(const char *file)
{
    Warning("info_zombie_spawn: loading %s...\n", file);

    FileHandle_t hFile = g_pFullFileSystem->Open(file, "rb", "MOD");
    if (!hFile) { Warning("Population file %s NOT FOUND!\n", file); return; }

    int fileSize = g_pFullFileSystem->Size(hFile);
    CUtlVector<char> buffer;
    buffer.SetCount(fileSize + 1);
    g_pFullFileSystem->Read(buffer.Base(), fileSize, hFile);
    g_pFullFileSystem->Close(hFile);
    buffer[fileSize] = 0;

    std::istringstream f(buffer.Base());
    std::string line;
    while (std::getline(f,line))
    {
        line = Trim(line);
        if (line.empty() || line[0]=='/') continue;
        std::string l=line; std::transform(l.begin(),l.end(),l.begin(),::tolower);
        if (l=="population") { std::getline(f,line); ParseBlock(f,line,nullptr); break; }
    }

    Warning("Population groups loaded:\n");
    for (int i=0;i<groups.Count();i++) Warning(" - %s\n",groups.GetElementName(i));
}

void CZombiePopulationManager::ParseBlock(std::istream &f,std::string &line,PopulationGroup_t *pGroup)
{
    while (true)
    {
        if (line.empty() || line[0]=='/') { if(!std::getline(f,line)) break; line=Trim(line); continue; }
        if (line=="}") return;

        if (!pGroup)
        {
            if (line=="{") { std::getline(f,line); line=Trim(line); continue; }
            std::string group=line;
            std::getline(f,line); line=Trim(line);
            if (line!="{") { Warning("Expected '{' after %s\n",group.c_str()); continue; }
            PopulationGroup_t *grp=new PopulationGroup_t();
            ParseBlock(f,line,grp);
            groups.Insert(group.c_str(),grp);
            if(!std::getline(f,line)) break; line=Trim(line);
        }
        else
        {
            std::istringstream iss(line); std::string n,w;
            if(iss>>n>>w){ PopulationEntry_t e; e.name=n.c_str(); e.weight=atoi(w.c_str()); pGroup->entries.AddToTail(e);}
            if(!std::getline(f,line)) break; line=Trim(line);
        }
    }
}

const char *CZombiePopulationManager::SelectRandom(const char *group)
{
    int idx = groups.Find(group);
    if (idx == groups.InvalidIndex()) { Warning("Group '%s' not found!\n", group); return nullptr; }
    PopulationGroup_t *g = groups.Element(idx);
    int total = 0; for(int i=0;i<g->entries.Count();i++) total += g->entries[i].weight;
    if(total<=0) return nullptr;
    int r = RandomInt(1,total),acc=0;
    for(int i=0;i<g->entries.Count();i++){ acc+=g->entries[i].weight; if(r<=acc){ Warning("Selected: %s\n",g->entries[i].name.String()); return g->entries[i].name.String();}}
    return nullptr;
}

//----------------------------------------
LINK_ENTITY_TO_CLASS(info_zombie_spawn,CInfoZombieSpawn);
BEGIN_DATADESC(CInfoZombieSpawn)
    DEFINE_KEYFIELD(m_iszPopulation,FIELD_STRING,"population"),
    DEFINE_INPUTFUNC(FIELD_VOID,"SpawnZombie",InputSpawnZombie),
END_DATADESC()

void CInfoZombieSpawn::Spawn()
{
    BaseClass::Spawn();
    static bool bLoaded=false; if(!bLoaded){ g_ZombiePopulationManager.Init("scripts/population.txt"); bLoaded=true;}
    SetSolid(SOLID_NONE); AddEffects(EF_NODRAW);
}

void CInfoZombieSpawn::InputSpawnZombie(inputdata_t &inputdata)
{
    const char *cls=g_ZombiePopulationManager.SelectRandom(STRING(m_iszPopulation));
    if(!cls){ Warning("info_zombie_spawn: no valid population group %s\n",STRING(m_iszPopulation)); return; }
    CBaseEntity *ent=CreateEntityByName(cls);
    if(!ent){ Warning("info_zombie_spawn: failed to create %s\n",cls); return; }
    ent->SetAbsOrigin(GetAbsOrigin()); ent->SetAbsAngles(GetAbsAngles());
    DispatchSpawn(ent); ent->Activate();
}
