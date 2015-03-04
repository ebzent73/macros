|------------------------------------------------------------------------------------|
|- e3_Classes_Shadow Knight.inc v3.0 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ														-|
|------------------------------------------------------------------------------------|
|- Contains Shadow Knight specific functions related to,							-|
|-		Setup and Ini entry information												-|
|-		MainLoop, and AssistLoop hooks												-|
|-		Over-all functionality / flow												-|
|------------------------------------------------------------------------------------|



Sub SHD_Setup
/if (${Debug}) /echo |- SHD_Setup ==>

	/if (${Ini[${Character_Ini},${Me.Class},LifeTap#1].Length}) /call IniToArray "${Character_Ini},${Me.Class},LifeTap#" lifeTaps

/if (${Debug}) /echo <== SHD_Setup -|
/return



Sub SHD_Background_Events

/return



SUB SHD_MacroSettings
/if (${Debug}) /echo |- SHD_MacroSettings ==>

/if (${Debug}) /echo <== SHD_MacroSettings -|
/RETURN



SUB SHD_CharacterSettings
/if (${Debug}) /echo |- SHD_CharacterSettings ==>

	/call WriteToIni "${Character_Ini},${Me.Class},LifeTap#1"

/if (${Debug}) /echo <== SHD_CharacterSettings -|
/RETURN



Sub SHD_Aliases
/if (${Debug}) /echo |- SHD_Aliases ==>

	| Adv Settings
	/call WriteToIni "${advSettings_Ini},SHD Functions,SHD Function#1" "check_Burns" 1
	/call WriteToIni "${advSettings_Ini},SHD Functions,SHD Function#2" "check_lifeTap" 1
	/call WriteToIni "${advSettings_Ini},SHD Functions,SHD Function#3" "check_Debuffs" 1
	/call WriteToIni "${advSettings_Ini},SHD Functions,SHD Function#4" "check_CombatBuffs" 1
	/call WriteToIni "${advSettings_Ini},SHD Functions,SHD Function#5" "check_DoTs" 1
	/call WriteToIni "${advSettings_Ini},SHD Functions,SHD Function#6" "check_Nukes" 1
	/call WriteToIni "${advSettings_Ini},SHD Functions,SHD Function#7" "check_Buffs" 1
	/call WriteToIni "${advSettings_Ini},SHD Functions,SHD Function#8" "check_Pets" 1

/if (${Debug}) /echo <== SHD_Aliases -|
/return