|------------------------------------------------------------------------|
|- e3_Classes_CLR.inc v3.0 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ											-|
|------------------------------------------------------------------------|
|- Contains CLR specific functions related to,							-|
|-		Setup and Ini entry information									-|
|-		MainLoop, and AssistLoop hooks									-|
|-		Over-all functionality / flow									-|
|------------------------------------------------------------------------|



Sub BER_Setup
/if (${Debug}) /echo |- BER_Setup ==>
	
/if (${Debug}) /echo <== BER_Setup -|
/return



Sub BER_Background_Events

/return



SUB BER_MacroSettings
/if (${Debug}) /echo |- BER_MacroSettings ==>

	| Adv Settings
	/call WriteToIni "${advSettings_Ini},BER Functions,BER Function#1" "check_Burns" 1
	/call WriteToIni "${advSettings_Ini},BER Functions,BER Function#2" "check_CombatBuffs" 1
	/call WriteToIni "${advSettings_Ini},BER Functions,BER Function#3" "check_Buffs" 1

/if (${Debug}) /echo <== BER_MacroSettings -|
/RETURN



SUB BER_CharacterSettings
/if (${Debug}) /echo |- BER_CharacterSettings ==>

/if (${Debug}) /echo <== BER_CharacterSettings -|
/RETURN



Sub BER_Aliases
/if (${Debug}) /echo |- BER_Aliases ==>

/if (${Debug}) /echo <== BER_Aliases -|
/return