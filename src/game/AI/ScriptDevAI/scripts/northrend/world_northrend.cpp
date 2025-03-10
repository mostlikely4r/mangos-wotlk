/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "world_northrend.h"
#include "AI/ScriptDevAI/scripts/world/world_map_scripts.h"

/* *********************************************************
 *                     NORTHREND
 */
struct world_map_northrend : public ScriptedMap
{
    world_map_northrend(Map* pMap) : ScriptedMap(pMap) {}

    void OnCreatureCreate(Creature* pCreature)
    {
        switch (pCreature->GetEntry())
        {
            case NPC_NARGO_SCREWBORE:
            case NPC_HARROWMEISER:
            case NPC_DRENK_SPANNERSPARK:
                m_npcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
                break;
        }
    }

    void SetData(uint32 /*uiType*/, uint32 /*uiData*/) {}

    bool CheckConditionCriteriaMeet(Player const* player, uint32 instanceConditionId, WorldObject const* conditionSource, uint32 conditionSourceType) const override
    {
        script_error_log("world_map_northrend::CheckConditionCriteriaMeet called with unsupported Id %u. Called with param plr %s, src %s, condition source type %u",
            instanceConditionId, player ? player->GetGuidStr().c_str() : "nullptr", conditionSource ? conditionSource->GetGuidStr().c_str() : "nullptr", conditionSourceType);
        return false;
    }
};

InstanceData* GetInstanceData_world_map_northrend(Map* pMap)
{
    return new world_map_northrend(pMap);
}

void AddSC_world_northrend()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "world_map_northrend";
    pNewScript->GetInstanceData = &GetInstanceData_world_map_northrend;
    pNewScript->RegisterSelf();
}