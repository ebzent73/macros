|----------------------------------------------------------------------------|
|- e3_Classes_Shaman.inc v3.0 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ												-|
|----------------------------------------------------------------------------|
|- Contains Shaman specific functions related to,							-|
|-		Setup and Ini entry information										-|
|-		MainLoop, and AssistLoop hooks										-|
|-		Over-all functionality / flow										-|
|----------------------------------------------------------------------------|



|--------------------------------------------------------|
|- autoCanni											-|
|--------------------------------------------------------|
|- Uses cannibalize spells and AAs when under 95% mana.	-|
|- Spells must be memmed to be used automatically.		-|
|--------------------------------------------------------|
SUB check_Canni
/if (${Debug}) /echo |- autoCanni ==>

	| Check if Auto-Canni is on
	/if (${AutoCanni}) {
	
		| AltAbility Cannibalization.
		/if (${Me.PctHPs} > 65 && ${Me.PctMana} < 65 && ${Me.AltAbilityReady[Cannibalization]}) /call e3_Cast 0 "Cannibalization"

		/if (!${ActionTaken} && !${medBreak}) {
		
			/declare castName string local
			/declare minMana int local
			/declare i int local
			
			/for i 1 to ${canniSpells.Size}
			
				/varset castName ${canniSpells[${i}].Arg[1,/]}
				
				| Get MinMana
				/if (${canniSpells[${i}].Find[/MinMana|]}) {
					/call argueString MinMana "${canniSpells[${i}]}"
					/varset minMana ${Macro.Return}
				} else {
					/varset minMana 90
				}
				
				/if (${Me.PctMana} < ${minMana} && ${Me.PctHPs} > 35) /call e3_Cast 0 "${canniSpells[${i}]}/giveuptimer|${If[${Me.Gem[${castName}]} || ${FindItemCount[=${castName}]},0,80]}"

			/if (!${ActionTaken}) /next i
		}
	}
	
/if (${Debug}) /echo <== autoCanni -|
/RETURN



Sub SHM_Setup
/if (${Debug}) /echo |- SHM_Setup ==>
	
	/call IniToVar "${Character_Ini},Shaman,Auto-Canni (On/Off)" AutoCanni bool outer
	/call IniToArray "${Character_Ini},Shaman,Canni#" canniSpells
	
/if (${Debug}) /echo <== SHM_Setup -|
/return



Sub SHM_Background_Events
/return



SUB SHM_MacroSettings
/if (${Debug}) /echo |- SHM_MacroSettings ==>

	| Adv Settings
	/call WriteToIni "${advSettings_Ini},SHM Functions,SHM Function#1" "check_Debuffs" 1
	/call WriteToIni "${advSettings_Ini},SHM Functions,SHM Function#2" "check_Burns" 1
	/call WriteToIni "${advSettings_Ini},SHM Functions,SHM Function#3" "check_Heals" 1
	/call WriteToIni "${advSettings_Ini},SHM Functions,SHM Function#4" "check_CombatBuffs" 1
	/call WriteToIni "${advSettings_Ini},SHM Functions,SHM Function#5" "check_DoTs" 1
	/call WriteToIni "${advSettings_Ini},SHM Functions,SHM Function#6" "check_petHeal" 1
	/call WriteToIni "${advSettings_Ini},SHM Functions,SHM Function#7" "check_Rain" 1
	/call WriteToIni "${advSettings_Ini},SHM Functions,SHM Function#8" "check_Nukes" 1
	/call WriteToIni "${advSettings_Ini},SHM Functions,SHM Function#9" "check_Buffs" 1
	/call WriteToIni "${advSettings_Ini},SHM Functions,SHM Function#10" "check_Pets" 1
	/call WriteToIni "${advSettings_Ini},SHM Functions,SHM Function#11" "check_Canni" 1

/if (${Debug}) /echo <== SHM_MacroSettings -|
/RETURN



SUB SHM_CharacterSettings
/if (${Debug}) /echo |- SHM_CharacterSettings ==>

	/call WriteToIni "${Character_Ini},Shaman,Auto-Canni (On/Off)"
	/call WriteToIni "${Character_Ini},Shaman,Canni#1"

/if (${Debug}) /echo <== SHM_CharacterSettings -|
/RETURN



Sub SHM_Aliases
/if (${Debug}) /echo |- SHM_Aliases ==>

/if (${Debug}) /echo <== SHM_Aliases -|
/return