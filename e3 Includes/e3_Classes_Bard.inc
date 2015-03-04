|------------------------------------------------------------------------|
|- e3_Classes_Bard.inc v3.0 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ											-|
|------------------------------------------------------------------------|
|- Contains Bard specific functions related to,							-|
|-		Setup and Ini entry information									-|
|-		MainLoop, and AssistLoop hooks									-|
|-		Over-all functionality / flow									-|
|------------------------------------------------------------------------|



#event saveMelody "<#1#> SaveMelody #2# #3#"
#event saveMelody "<#1#> Save Melody #2# #3#"
#event saveMelody "[MQ2] SaveMelody #2# #3#"
#event saveMelody "[MQ2] Save Melody #2# #3#"
SUB EVENT_saveMelody(line, ChatSender, melodyName, songsToSave)
/if (${Debug}) /echo |- EVENT_saveMelody ==>

	| If no ChatSender is defined, make one.	
	/if (!${Defined[ChatSender]}) /declare ChatSender string local ${Me.CleanName}

	| If I'm the ChatSender.
	/if (${ChatSender.Equal[${Me.CleanName}]}) {

		/declare i int local

		| Check if the melody already exists, and should not be overwritten.
		/if (${Ini[${Character_Ini},${melodyName} Melody].Length}) {	
			/if (${Debug}) /echo |- EVENT_saveMelody -| Overwriting [${melodyName}] melody...
			/for i 1 to 5
				/if (${Ini[${Character_Ini},${melodyName} Melody,Song#${i}].Length}) /call WriteToIni "${Character_Ini},${melodyName} Melody,Song#${i}" *PLACEHOLDER 1
			/next i
		}
		
		/echo Saving [${melodyName}] melody...
		
		/varset i 1
	
		:check_nextSong

		| If the gem number is not recognized
		/if (!${Int[${songsToSave.Arg[${i}, ]}]} || ${Int[${songsToSave.Arg[${i}, ]}]} < 1 || ${Int[${songsToSave.Arg[${i}, ]}]} > 9) {
			/if (${songsToSave.Arg[${i}, ].NotEqual[-Overwrite]}) {
				/echo [${songsToSave.Arg[${i}, ]}] is not a valid gem number.  Please use the following syntax when saving melodies, "Save Melody [NAME] [# # # # #] (-Overwrite)".  Ex.  /echo Save Melody Raid 1 2 3 4 5 -Overwrite
			}
		} else {
			/echo Song#${i}=${Me.Gem[${songsToSave.Arg[${i}, ]}]}/gem|${songsToSave.Arg[${i}, ]}
			/call WriteToIni "${Character_Ini},${melodyName} Melody,Song#${i}" "${Me.Gem[${songsToSave.Arg[${i}, ]}]}/gem|${songsToSave.Arg[${i}, ]}" ${If[${Ini[${Character_Ini},${melodyName} Melody,Song#${i}].Length},1,0]}
		}
		/varcalc i ${i} + 1
		/if (${songsToSave.Arg[${i}, ].Length}) /goto :check_nextSong
	}

/if (${Debug}) /echo <== EVENT_saveMelody -|
/return



#EVENT startMelody "<#1#> Melody #2#"
#EVENT startMelody "[#1#] Melody #2#"
#EVENT startMelody "#1# tells you, 'Melody #2#'"
#EVENT startMelody "#1# tells the group, 'Melody #2#'"
SUB EVENT_startMelody(line, ChatSender, melodyName)
/if (${Debug}) /echo |- EVENT_startMelody ==>

	| -Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|    |--------Included Bot-----------|    |-----------In Zone-------------|    |------------In Range------------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]} && ${Bool[${Macro.Return.Arg[5,_]}]}) {

		/if (${melodyName.Find[ /]}) /varset melodyName ${melodyName.Arg[1, /]}

		/if (${Me.Class.ShortName.Equal[BRD]}) {
			
			| If I don't have the melodyName listed in my ini.
			/if (!${Ini[${Character_Ini},${melodyName} Melody,Song#1].Length}) {
				
				| If Stop, end, or pause were used, stop the melody.
				/if (${Select[${melodyName},Stop,End,Pause]}) {
					/varset playingMelody FALSE
					/varset songSet
					/docommand ${ChatToggle} Ending melody.
					/twist end
				} else {
					/docommand ${ChatToggle} [${melodyName}] is not a known melody.
				}
			} else {
			
				/declare songList string local
				/declare retryTimer timer local 100
				/declare spellName string local
				/declare i int local
				
				| For 1 to 5 (maximum melody size)
				/for i 1 to 5
				
				/if (${Ini[${Character_Ini},${melodyName} Melody,Song#${i}].Length} && ${Ini[${Character_Ini},${melodyName} Melody,Song#${i}].NotEqual[PLACEHOLDER]}) {
					
					/varset spellName ${Ini[${Character_Ini},${melodyName} Melody,Song#${i}].Arg[1,/]}
					
					| Check that the song is in my book
					/if (!${Me.Book[${spellName}]}) {	
						/echo I don't have [${spellName}] in my spellbook, playing the melody without it.
					} else {
					
						| If the song is not memmed, use e3_Casting.inc functions to mem it.
						/if (!${Me.Gem[${spellName}]}) {
							/call argueString gem "${Ini[${Character_Ini},${melodyName} Melody,Song#${i}]}"
							/varset pendingCast ${spellName}
							/varset giveUpTimer 100
							/call memSpell "${Macro.Return}"
						}
						
						| Update songList with the corresponding gem#
						/varset songList ${songList} ${Me.Gem[${spellName}]}
					}
					/next i
				}
				
				| If I have a songList, start singing the melody
				/if (${Bool[${songList}]}) {
					
					| Twist the newly built songList.
					/varset playingMelody TRUE
					/varset songSet ${songList}
					/docommand ${ChatToggle} Playing [${melodyName}] melody...
					/twist ${songSet}
				}
			}
		}
	}
	
/if (${Debug}) /echo <== EVENT_startMelody -|
/return



SUB restart_Melody
/if (${Debug}) /echo |- check_Melody ==>

	/if (!${Me.Casting.ID}) /twist ${Twist.List}

/if (${Debug}) /echo <== check_Melody -|
/RETURN



Sub BRD_Setup
/if (${Debug}) /echo |- BRD_Setup ==>

	/declare returnTwist bool outer 0
	
	/if (!${Defined[playingMelody]}) /declare playingMelody bool global
	/if (!${Defined[songSet]}) /declare songSet string global
	
	/call InitoVar "${Character_Ini},Bard,Auto-Melody" autoMelody string outer
	
	| Start Auto-Melody at start-up.  Currently disabled to check auto start in check_Zone sub.
	| /if (${Defined[autoMelody]} && !${songSet.Length}) /startmelody ${autoMelody} /only|ME
	
/if (${Debug}) /echo <== BRD_Setup -|
/return



Sub BRD_Background_Events
	
	/doevents saveMelody
	/doevents startMelody
	
	| Check melody
	/if (${playingMelody} && !${Twist.Twisting} && !${Bool[${Me.Casting}]}) /call restart_Melody
	
	| Check Auto-Melody
	| The call for Auto-Melody is located in e3 Background.inc -> check_Zone.  This is so Auto-Melody will only be checked when you zone, or load the macro.

/return



SUB BRD_MacroSettings
/if (${Debug}) /echo |- BRD_MacroSettings ==>

	| Adv Settings
	/call WriteToIni "${advSettings_Ini},BRD Functions,BRD Function#1" "check_Burns" 1
	/call WriteToIni "${advSettings_Ini},BRD Functions,BRD Function#2" "check_CombatBuffs" 1
	/call WriteToIni "${advSettings_Ini},BRD Functions,BRD Function#3" "check_Buffs" 1

/if (${Debug}) /echo <== BRD_MacroSettings -|
/RETURN



SUB BRD_CharacterSettings
/if (${Debug}) /echo |- BRD_CharacterSettings ==>

	/call WriteToIni "${Character_Ini},Bard,Auto-Melody"

/if (${Debug}) /echo <== BRD_CharacterSettings -|
/RETURN



Sub BRD_Aliases
/if (${Debug}) /echo |- BRD_Aliases ==>

	/squelch /alias /SaveMelody /echo Save Melody
	/squelch /alias /PlayMelody /bc  Melody

/if (${Debug}) /echo <== BRD_Aliases -|
/return