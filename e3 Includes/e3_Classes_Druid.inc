|----------------------------------------------------------------------------|
|- e3_Classes_Druid.inc v3.0 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ												-|
|----------------------------------------------------------------------------|
|- Contains Druid specific functions related to,							-|
|-		Setup and Ini entry information										-|
|-		MainLoop, and AssistLoop hooks										-|
|-		Over-all functionality / flow										-|
|----------------------------------------------------------------------------|



Sub DRU_Setup
/if (${Debug}) /echo |- DRU_Setup ==>
	
	/call IniToVar "${Character_Ini},Druid,Evac Spell" Evac_Spell string outer
	
/if (${Debug}) /echo <== DRU_Setup -|
/return



Sub DRU_Background_Events

/return



SUB DRU_MacroSettings
/if (${Debug}) /echo |- DRU_MacroSettings ==>

	| Adv Settings
	/call WriteToIni "${advSettings_Ini},DRU Functions,DRU Function#1" "check_healChain" 1
	/call WriteToIni "${advSettings_Ini},DRU Functions,DRU Function#2" "check_Heals" 1
	/call WriteToIni "${advSettings_Ini},DRU Functions,DRU Function#3" "check_CombatBuffs" 1
	/call WriteToIni "${advSettings_Ini},DRU Functions,DRU Function#4" "check_Burns" 1
	/call WriteToIni "${advSettings_Ini},DRU Functions,DRU Function#5" "check_Debuffs" 1
	/call WriteToIni "${advSettings_Ini},DRU Functions,DRU Function#6" "check_DoTs" 1
	/call WriteToIni "${advSettings_Ini},DRU Functions,DRU Function#7" "check_Rain" 1
	/call WriteToIni "${advSettings_Ini},DRU Functions,DRU Function#8" "check_Nukes" 1
	/call WriteToIni "${advSettings_Ini},DRU Functions,DRU Function#9" "check_Buffs" 1

/if (${Debug}) /echo <== DRU_MacroSettings -|
/RETURN



SUB DRU_CharacterSettings
/if (${Debug}) /echo |- DRU_CharacterSettings ==>

	| Bot Ini
	/call WriteToIni "${Character_Ini},Druid,Evac Spell"

/if (${Debug}) /echo <== DRU_CharacterSettings -|
/RETURN



Sub DRU_Aliases
/if (${Debug}) /echo |- DRU_Aliases ==>

/if (${Debug}) /echo <== DRU_Aliases -|
/return