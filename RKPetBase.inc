Sub Init_RKPetBase
	/declare PetBuffs[${DynamicArraySize}] string outer
	
	/declare PetSpellName string outer

	/call AddToArray "NonCombatSubs" "CheckPet"
	/call AddToArray "CombatActionSubs" "ExecutePetCombatActions"
	
	/call AddToArray "CombatActionSubs" "ExecutePetCombatActions"
	/call AddToArray "AssistCalledSubs" "AssistWithPet"
	/call AddToArray "PreTargetAcquisitionCombatActionSubs" "CheckPetBuffs"
	
	/declare PetAttackGHold string outer "on"
	/declare PetAttackFocus string outer "on"
	/declare PetAttackSpellHold string outer "on"
	/declare PetAttackTaunt string outer "off"

	/echo Done initializing pet base!
/return

Sub CheckPet
	/if (!${Me.Pet.ID} && ${Bool[${PetSpellName}]}) {
		/echo casting pet spell: ${PetSpellName}
		/call RKMemCast "${PetSpellName}" ${Me.ID}
		/return TRUE
	}
/return FALSE

Sub ExecutePetCombatActions
	/if (!${Me.Pet.ID}) /return FALSE
	/if (!${Bool[${Me.Pet.Following}]}) /call AssistWithPet
/return FALSE

Sub PetAttackMode
	/call SetPetAttackMode
/return FALSE

Sub AssistWithPet
	/if (!${Target.Type.Equal[NPC]} || !${Me.Pet.ID}) /return FALSE
	/pet attack
	/call SetPetAttackMode
/return FALSE

Sub SetPetAttackMode
	/if (!${Me.Pet.ID}) /return
	/pet ghold ${PetAttackGHold}
	/pet focus ${PetAttackFocus}
	/pet spellhold ${PetAttackSpellHold}
	/pet taunt ${PetAttackTaunt}
/return FALSE

Sub RKPetBuff(string spellName)
	/if (${Me.PetBuff[${spellName}]}) /return FALSE
	/call WillPetBuffStack "${spellName}"
	/if (!${Macro.Return}) /return FALSE
	/call RKMemCast "${spellName}" ${Me.Pet.ID}
/return ${Macro.Return}

Sub WillPetBuffStack(string spellName)
	/declare i int local
	/for i 1 to 42
		/if (${Bool[${Me.PetBuff[${i}]}]} && !${Spell[${spellName}].WillStack[${Me.PetBuff[${i}]}]}) /return FALSE
	/next i
/return TRUE

Sub CheckPetBuffs
	/if (!${Me.Pet.ID}) /return
	/declare i int local
	/for i 1 to ${PetBuffs.Size}
		/if (${Bool[${PetBuffs[${i}]}]}) {
			/call RKPetBuff "${PetBuffs[${i}]}" ${Me.ID}
			/if (${Macro.Return}) /return TRUE
		}
	/next i
/return FALSE
