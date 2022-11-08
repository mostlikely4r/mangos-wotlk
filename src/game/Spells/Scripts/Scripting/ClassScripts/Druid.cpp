/*
* This file is part of the CMaNGOS Project. See AUTHORS file for Copyright information
*
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

#include "Spells/Scripts/SpellScript.h"
#include "Spells/SpellAuras.h"

enum
{
    SPELL_BLESSING_OF_THE_CLAW = 28750,
};

struct Regrowth : public AuraScript
{
    void OnPeriodicTickEnd(Aura* aura) const override
    {
        if (Unit* caster = aura->GetCaster())
            if (caster->HasOverrideScript(4537))
                caster->CastSpell(aura->GetTarget(), SPELL_BLESSING_OF_THE_CLAW, TRIGGERED_OLD_TRIGGERED);
    }
};

struct FormScalingAttackPowerAuras : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        if (data.spellProto->EffectApplyAuraName[data.effIdx] == SPELL_AURA_MOD_ATTACK_POWER && data.target->IsPlayer())
        {
            Player* player = static_cast<Player*>(data.target);
            // Predatory Strikes
            Aura* predatoryStrikes = player->GetKnownTalentRankAuraById(803, EFFECT_INDEX_0);
            if (predatoryStrikes)
                value += data.target->GetLevel() * predatoryStrikes->GetAmount() / 100;
        }
        return value;
    }
};

struct ForceOfNatureSummon : public SpellScript, public AuraScript
{
    void OnSummon(Spell* /*spell*/, Creature* summon) const override
    {
        summon->CastSpell(nullptr, 37846, TRIGGERED_NONE);
    }

    void OnHolderInit(SpellAuraHolder* holder, WorldObject* /*caster*/) const
    {
        holder->SetAuraDuration(2000);
    }

    void OnPeriodicDummy(Aura* aura) const override
    {
        Unit* target = aura->GetTarget();
        target->CastSpell(nullptr, 41929, TRIGGERED_OLD_TRIGGERED);
    }
};

struct GuardianAggroSpell : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* target = spell->GetUnitTarget();
        Unit* caster = spell->GetCaster();
        if (target->GetEntry() == 1964) // Force of Nature treant
        {
            if (target->CanAttack(caster))
            {
                if (target->IsVisibleForOrDetect(caster, caster, true))
                    target->AI()->AttackStart(caster);
            }
        }
    }
};

struct WildGrowth : public SpellScript
{
    void OnInit(Spell* spell) const override
    {
        Unit* caster = spell->GetCaster();
        // stored in dummy effect, affected by mods
        spell->SetMaxAffectedTargets(spell->CalculateSpellEffectValue(EFFECT_INDEX_2, caster)); 
        spell->SetFilteringScheme(EFFECT_INDEX_0, true, SCHEME_PRIORITIZE_HEALTH);
    }

    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        return spell->GetCaster()->IsInGroup(target);
    }
};

struct Brambles : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (aura->GetEffIndex() != EFFECT_INDEX_0)
            return;

        aura->GetTarget()->RegisterScriptedLocationAura(aura, SCRIPT_LOCATION_SPELL_DAMAGE_DONE, apply);
    }
};

// 1079 - Rip
struct ShredDruid : public SpellScript
{
    void OnHit(Spell* spell, SpellMissInfo missInfo) const override
    {
        if (missInfo == SPELL_MISS_NONE)
        {
            if (Aura* glyphOfShred = spell->GetCaster()->GetAura(54815, EFFECT_INDEX_0)) // Glyph of Shred
            {
                Unit* target = spell->GetUnitTarget();
                if (Aura* rip = target->GetAura(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_DRUID, 0x0, 0x00200000, spell->GetCaster()->GetObjectGuid()))
                {
                    int32 increaseAmount = rip->GetAmount();
                    int32 maxIncreaseAmount = spell->GetCaster()->CalculateSpellEffectValue(target, rip->GetSpellProto(), EFFECT_INDEX_1);
                    if (rip->GetScriptValue() >= maxIncreaseAmount)
                        return;
                    SpellAuraHolder* holder = rip->GetHolder();
                    holder->SetAuraMaxDuration(holder->GetAuraMaxDuration() + increaseAmount);
                    holder->SetAuraDuration(holder->GetAuraDuration() + increaseAmount);
                    holder->SendAuraUpdate(false);
                    rip->SetScriptValue(rip->GetScriptValue() + increaseAmount);
                }
            }
        }
    }
};

void LoadDruidScripts()
{
    RegisterSpellScript<Regrowth>("spell_regrowth");
    RegisterSpellScript<FormScalingAttackPowerAuras>("spell_druid_form_scaling_ap_auras");
    RegisterSpellScript<ForceOfNatureSummon>("spell_force_of_nature_summon");
    RegisterSpellScript<GuardianAggroSpell>("spell_guardian_aggro_spell");
    RegisterSpellScript<WildGrowth>("spell_wild_growth");
    RegisterSpellScript<Brambles>("spell_brambles");
    RegisterSpellScript<ShredDruid>("spell_shred_druid");
}