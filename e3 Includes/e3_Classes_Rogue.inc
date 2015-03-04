|----------------------------------------------------------------------------|
|- e3_Classes_Rogue.inc v3.0 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ												-|
|----------------------------------------------------------------------------|



|--------------------------------------------------------------------------------|
|- rogueStrike																	-|
|--------------------------------------------------------------------------------|
|- Attempts to arm 'Assassin's Strike' type disciplines before using backstab.	-|
|--------------------------------------------------------------------------------|
Sub check_rogStrike(discName)
/if (${Debug}) /echo |- rogueStrike ==>
	
	/if (!${Defined[strikeDisc]}) {
		/declare strikeDisc string outer ${discName}
		/if (${strikeDisc.Equal[Daggerlunge Rk. III]}) {
			/declare MinStrikeEndurance int outer 1270	
		} else /if (${strikeDisc.Equal[Daggerlunge Rk. II]}) {
			/declare MinStrikeEndurance int outer 1270	
		} else /if (${strikeDisc.Equal[Daggerlunge]}) {
			/declare MinStrikeEndurance int outer 1270	
		} else /if (${strikeDisc.Equal[Swiftblade Rk. III]}) {
			/declare MinStrikeEndurance int outer 1016	
		} else /if (${strikeDisc.Equal[Swiftblade Rk. II]}) {
			/declare MinStrikeEndurance int outer 1016
		} else /if (${strikeDisc.Equal[Swiftblade]}) {
			/declare MinStrikeEndurance int outer 1016		
		} else /if (${strikeDisc.Equal[Razorarc]}) {
			/declare MinStrikeEndurance int outer 650	
		} else /if (${strikeDisc.Equal[Daggerfall]}) {
			/declare MinStrikeEndurance int outer 625	
		} else /if (${strikeDisc.Equal[Ancient: Chaos Strike]}) {
			/declare MinStrikeEndurance int outer 575
		} else /if (${strikeDisc.Equal[Kyv Strike]}) {
			/declare MinStrikeEndurance int outer 550
		} else /if (${strikeDisc.Equal[Assassin's Strike]}) {
			/declare MinStrikeEndurance int outer 515
		} else /if (${strikeDisc.Equal[Thief's Vengeance]}) {
			/declare MinStrikeEndurance int outer 425
		} else /if (${strikeDisc.Equal[Sneak Attack]}) {
			/declare MinStrikeEndurance int outer 325
		}
	}

	/if (${Me.Endurance} > ${MinStrikeEndurance} && ${Me.AbilityReady[Hide]} && ${Me.CombatAbilityReady[${strikeDisc}]}) {
	
		| Stop moving.
		/if (${Stick.Active}) {
			/squelch /stick pause
			/delay 2s !${Me.Moving}
		}
		
		| Turn off combat.
		/if (${Me.Combat}) {
			/attack off
			/delay 2s !${Me.Combat}
		}
		
		| Sneak
		/doability Sneak
		/delay 20 ${Me.Sneaking}
		
		| Hide.
		/doability Hide
		/delay 20 ${Me.Invis}
		
		| Use 'Assassin's Strike' type disc.
		/if (${Me.Invis}) {
			/call e3_Cast 0 "${strikeDisc}/Instant"
			/attack on
			/squelch /stick unpause
			/delay 10
		}
	}

/if (${Debug}) /echo <== rogueStrike -|
/return



|------------------------------------------------------------|
|- AutoHide													-|
|------------------------------------------------------------|
|- Automatically engages sneak and hide when not in combat.	-|
|------------------------------------------------------------|
sub AutoHide
/if (${Debug}) /echo |- AutoHide ==>

	| Engage sneak.
	/if (!${Me.Sneaking} && ${Me.AbilityReady[Sneak]}) {
		/doability Sneak
		/delay 2s ${Me.Sneaking}
	}
	
	| Engage hide.
	/if (${Me.Sneaking} && ${Me.AbilityReady[Hide]}) {
		/doability Hide
		/delay 2s ${Me.Invis}
	}

/if (${Debug}) /echo <== AutoHide -|
/return



|------------------------------------------------|
|- AutoEvade									-|
|------------------------------------------------|
|- Automatically engages hide while in combat.	-|
|------------------------------------------------|
sub AutoEvade
/if (${Debug}) /echo |- AutoEvade ==>

	/if (${Me.AbilityReady[Hide]}) {
		
		| Turn off combat.
		/if (${Me.Combat}) {
			/attack off
			/delay 2s !${Me.Combat}
		}
		
		| Hide.
		/doability Hide
		/delay 20 ${Me.Invis}
		
		| Turn combat back on.
		/if (!${Me.Combat}) {
			/attack on
			/delay 2s ${Me.Combat}
		}
	}

/if (${Debug}) /echo <== AutoEvade -|
/return



Sub ROG_Setup
/if (${Debug}) /echo |- ROG_Setup ==>
	
	| Import a list of all rogue backstab disciplines from MacroData_Ini
	/call IniToVar "${MacroData_Ini},Class Specific Info,Rogue Sneak Attack Disciplines" rogueStrikes string outer
	
	| Auto-Hide
	/call IniToVar "${Character_Ini},Rogue,Auto-Hide (On/Off)" AutoHide bool outer
	
	| Auto-Evade
	/call IniToVar "${Character_Ini},Rogue,Auto-Evade (On/Off)" AutoEvade bool outer
	/if (${AutoEvade}) /call BuildArray MyAbilities Evade
	
/if (${Debug}) /echo <== ROG_Setup -|
/return



Sub ROG_Background_Events

	/if (${AutoHide} && !${Me.Invis} && !${Me.Moving} && !${Me.Combat}) /call AutoHide

/return



SUB ROG_MacroSettings
/if (${Debug}) /echo |- ROG_MacroSettings ==>

	| MacroData_Ini settings
	/call WriteToIni "${MacroData_Ini},Class Specific Info,Rogue Sneak Attack Disciplines" "Sneak Attack,Thief's Vengeance,Assassin's Strike,Kyv Strike,Ancient: Chaos Strike,Daggerfall,Razorarc,Swiftblade,Swiftblade Rk. II,Swiftblade Rk. III,Daggerlunge,Daggerlunge Rk. II,Daggerlunge Rk. III,Daggerswipe,Daggerswipe Rk. II,Daggerswipe Rk. III,Daggerstrike,Daggerstrike Rk. II,Daggerstrike Rk. III,Daggerthrust,Daggerthrust Rk. II,Daggerthrust Rk. III" 0

	| Adv Settings
	/call WriteToIni "${advSettings_Ini},ROG Functions,ROG Function#1" "check_Burns" 1
	/call WriteToIni "${advSettings_Ini},ROG Functions,ROG Function#2" "check_CombatBuffs" 1
	/call WriteToIni "${advSettings_Ini},ROG Functions,ROG Function#3" "check_Buffs" 1

/if (${Debug}) /echo <== ROG_MacroSettings -|
/RETURN



SUB ROG_CharacterSettings
/if (${Debug}) /echo |- ROG_CharacterSettings ==>

	/call WriteToIni "${Character_Ini},${Me.Class},Auto-Hide (On/Off)"
	/call WriteToIni "${Character_Ini},${Me.Class},Auto-Evade (On/Off)"

/if (${Debug}) /echo <== ROG_CharacterSettings -|
/RETURN



Sub ROG_Aliases
/if (${Debug}) /echo |- ROG_Aliases ==>

/if (${Debug}) /echo <== ROG_Aliases -|
/return