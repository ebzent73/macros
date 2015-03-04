|------------------------------------------------------------------------|
|- e3_Classes_Monk.inc v3.0 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ											-|
|------------------------------------------------------------------------|
|- Contains Monk specific functions related to,							-|
|-		Setup and Ini entry information									-|
|-		MainLoop, and AssistLoop hooks									-|
|-		Over-all functionality / flow									-|
|------------------------------------------------------------------------|



Sub MNK_Setup
/if (${Debug}) /echo |- MNK_Setup ==>
	
/if (${Debug}) /echo <== MNK_Setup -|
/return



Sub MNK_Background_Events

/return



SUB MNK_MacroSettings
/if (${Debug}) /echo |- MNK_MacroSettings ==>

	| Adv Settings
	/call WriteToIni "${advSettings_Ini},MNK Functions,MNK Function#1" "check_Burns" 1
	/call WriteToIni "${advSettings_Ini},MNK Functions,MNK Function#2" "check_CombatBuffs" 1
	/call WriteToIni "${advSettings_Ini},MNK Functions,MNK Function#3" "check_Buffs" 1

/if (${Debug}) /echo <== MNK_MacroSettings -|
/RETURN



SUB MNK_CharacterSettings
/if (${Debug}) /echo |- MNK_CharacterSettings ==>

/if (${Debug}) /echo <== MNK_CharacterSettings -|
/RETURN



Sub MNK_Aliases
/if (${Debug}) /echo |- MNK_Aliases ==>

/if (${Debug}) /echo <== MNK_Aliases -|
/return