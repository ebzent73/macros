|------------------------------------------------------------------------|
|- e3_Macro_Loot.inc v2.1 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ											-|
|------------------------------------------------------------------------|
|- Automatically loots corpses of mobs you have killed.					-|
|------------------------------------------------------------------------|



|------------------------------------------------------------|
|- Automatically engage looting functions when a mob dies.	-|
|------------------------------------------------------------|
#event YourKill "You have slain #*#!"
#event YourKill "You gain experience!!"
#event YourKill "You gain party experience!!"
#event YourKill "You gained raid experience!"
#event YourKill "#*#test loot#*#"
SUB Event_YourKill
/if (${Debug} || ${Debug_Loot}) /echo |- Event_YourKill ==>

	| If I am a looter.
	/if (${Auto_Loot}) {
	
		/declare new_lootedList string local
		
		/declare i int local 1
	
		| If I have a list of looted corpses, check if any of them have Disappeared.  If they have, rebuild the list
		/if (${looted_Corpses.Length}) {
			:check_vanished_Corpses
			/if (${Spawn[id ${looted_Corpses.Arg[${i},,]}].Type.Equal[Corpse]}) /varset new_lootedList ${new_lootedList}${looted_Corpses.Arg[${i},,]},
			/varcalc i ${i} + 1
			/if (${Bool[${looted_Corpses.Arg[${i},,]}]}) /goto :check_vanished_Corpses
			/varset looted_Corpses ${new_lootedList}
		}
		
		| Check all the corpses within the seek_Radius to see if they have been looted.  If not, call loot_It
		/for i 1 to ${SpawnCount[npc corpse radius ${seek_Radius}]}
		
			/if (${Bool[${looted_Corpses}]} && ${Select[${NearestSpawn[${i},corpse radius ${seek_Radius}].ID},${looted_Corpses}]}) /goto :skip_Loot
	
			/call loot_It ${NearestSpawn[${i},corpse radius ${seek_Radius}].ID}
			
			| If I just looted a corpse, wait a moment before moving to the next
			/if (${Macro.Return.Equal[JUST_LOOTED]}) /delay 3
			
			:skip_Loot
			
		/next i
	}

/if (${Debug} || ${Debug_Loot}) /echo <== Event_YourKill -|
/RETURN



|------------------------------------------------------------------------|
|- Moves to corpse and begins looting, reads Loot_Ini for loot handles.	-|
|------------------------------------------------------------------------|
|- @param corpseID: ID of the corpse you are trying to loot.			-|
|------------------------------------------------------------------------|
SUB loot_It(int corpseID)
/if (${Debug} || ${Debug_Loot}) /echo |- loot_It ==>

	| Declare local variables
	/declare corpseName string local ${Spawn[id ${corpseID}]}
	/declare lootSetting string local
	/declare lootTotal int local -1
	/declare lootTotal_Timer timer local 5s

	/declare i int local
	
	| Check to see if corpseID belongs to an NPC (NPC corpses have _'s in the name).
	/if (${Spawn[id ${corpseID}].LineOfSight} && ${corpseName.Find[_]}) {
	
		| Target the corpse, and move to it
		/if (${NetAdvPath.Status}==1) /squelch /netfollow pause
		/call TrueTarget ${corpseID}
		/if (${Spawn[id ${corpseID}].Distance} > 15) /call MoveTo -SpawnID|${corpseID}
		
		| If corpseID is in range, loot it.
		/if (${Spawn[id ${corpseID}].Distance} < 15 && ${Spawn[id ${corpseID}].DistanceZ} < 15) {
		
			| If looter is a bard, pause melody.
			/if (${Twist.Twisting}) /call pauseTwist
			
			/loot
		
			| Check to see if the corpse is yours, if it is, wait to open the loot window.
			/doevents NotYourKill
			/delay 20 ${Window[LootWnd].Open} || ${Macro.Return.Equal[SKIP_LOOT]}
			/if (${Macro.Return.Equal[SKIP_LOOT]}) /goto :skip_Loot
		
			| Count items on the corpse, loop until no new items are found.
			:autoxplootlag
			/if (${lootTotal}!=${Corpse.Items}) {
				/varset lootTotal ${Corpse.Items}
				/delay 5
				/if (${lootTotal_Timer}) /goto :autoxplootlag
			}
			
			| If there are items to loot
			/if (${lootTotal}) {
			
				/for i 1 to ${lootTotal}

				| If my inventory is full, and I haven't alerted the user.
				/if (!${Me.FreeInventory}) {
					
					| If I haven't alerted the user
					/if (!${fullInventory_Alert}) {
						
						/declare x int local
						
						/for x 1 to ${Loot_Alert}
							/beep 1
							/delay 3
						/next x
						
						/declare fullInventory_Alert bool outer TRUE
						/docommand ${ChatToggle} My inventory is full!  I will continue to link items on corpses, but cannot loot anything else.
					}
				} else {

					| Wait for an item ID in the loot slot, and set iniEntryVariables
					/delay 10 ${InvSlot[loot${i}].Item.ID}

					/call get_lootSetting "loot${i}"
					/varset lootSetting ${Macro.Return}
					
					| Destroy the item
					/if (${lootSetting.Find[Destroy]}) {
						/echo [Auto-Loot]: Destroying [${itemName}]
						/if (${InvSlot[loot${i}].Item.Lore} && ${FindItemCount[=${itemName}]}) /goto :skip_Loot
						/call loot_Handle ${i} destroy
					
					| Keep the item
					} else /if (${lootSetting.Find[Keep]}) {
					
						| Check for a max stock
						/if (${lootSetting.Find[keep|]}) {
							/call argueString keep| "${lootSetting}"
							/if (${Bool[${Macro.Return}]} && ${Int[${Macro.Return}]} < ${Math.Calc[${FindItemCount[${InvSlot[loot${i}].Item.Name}]} + 1].Int}) {
								/echo [Auto-Loot]: Fully stocked on [${InvSlot[loot${i}].Item.Name}].
								/goto :skip_Item
							}
						}
						
						| If the item is lore, make sure I don't have one.  If I do, skip it.
						/if (${InvSlot[loot${i}].Item.Lore}) {
							/if (${FindItemCount[=${InvSlot[loot${i}].Item.Name}]} || ${FindItemBankCount[=${InvSlot[loot${i}].Item.Name}]}) {
								/echo [Auto-Loot]: [${InvSlot[loot${i}].Item.Name}] is lore, and I already have one${If[${FindItemBankCount[=${InvSlot[loot${i}].Item.Name}]}, in the bank,]}.
								/goto :skip_Item
							}
						}
						
						| Check that I have an inventory space large enough for the item
						/if (${Me.LargestFreeInventory} < ${InvSlot[loot${i}].Item.Size}) {
							/echo [Auto-Loot]: I don't have a free inventory space large enough to hold [${itemName}].
							/goto :skip_Item
						}
						
						| If the item is a container, skip it.
						/if (${InvSlot[loot${i}].Item.Container}) {
							/echo [Auto-Loot]: Skipping [${itemName}], item is a container.
							/goto :skip_Item
						}
						
						| loot the item
						/echo [Auto-Loot]: Looting [${InvSlot[loot${i}].Item.Name}]
						/call loot_Handle ${i} keep	
					}
					
					:skip_Item
					/next i
				}
			}
			
			| If I've finished looting, and there are still items on the corpse, announce in chat.
			/if (${Corpse.Items}) {
			
				| If I'm using MQ2LinkDB
				/if (${use_linkDB}) {
				
					/declare linkList string local ${Corpse.CleanName} [ID:${Corpse.ID}] [Loc:${Spawn[id ${corpseID}].Loc}]:  
					
					/for i 1 to ${lootTotal}
						/if (${InvSlot[loot${i}].Item.ID}) /varset linkList ${linkList}${LinkDB[=${InvSlot[loot${i}].Item.Name}]}, 
					/next i
					
					/docommand ${linkChannel} ${linkList.Left[${Math.Calc[${linkList.Length} - 1].Int}]}

				} else {
					/notify LootWND LW_BroadcastButton leftmouseup
					/keypress enter chat
				}
			}
			
			| Add corpseID to looted_Corpses array.
			:SKIP_LOOT
			/varset looted_Corpses ${looted_Corpses}${corpseID},

			| Click 'Done' and return to group.
			/if (${Corpse.Open}) /notify LootWnd DoneButton leftmouseup 
			/if (${returnTwist}) /call unpauseTwist
			/if (${Following}) /call AquireFollow
		}
	}

/if (${Debug} || ${Debug_Loot}) /echo <== loot_It -|
/RETURN JUST_LOOTED



SUB get_lootSetting(invSlot, itemSlot)
/if (${Debug} || ${Debug_Loot}) /echo |- get_lootSetting invSlot: ${invSlot} ${If[${Defined[itemSlot]},itemSlot: ${itemSlot},]}==>

	/declare itemName string local
	/declare itemValue string local
	/declare iniEntryVariables string local
	/declare lootSetting string local

	| If the item is not in a bag
	/if (!${Defined[itemSlot]}) {
	
		| Reset itemName if it contains a ':'.  ':'s cause errors when reading from the ini, because they act as a delimiter, just like '='s
		/varset itemName ${InvSlot[${invSlot}].Item.Name}
		/if (${itemName.Find[:]}) /varset itemName ${itemName.Replace[:,;]}
		
		| Set item value
		/varset itemValue ${InvSlot[${invSlot}].Item.Value}
		/varset itemValue ${If[${Bool[${itemValue.Left[${Math.Calc[${itemValue.Length} - 3].Int}]}]},${itemValue.Left[${Math.Calc[${itemValue.Length} - 3].Int}]}p,]}${If[${Bool[${itemValue.Mid[${Math.Calc[${itemValue.Length} - 2].Int}]}]},${itemValue.Mid[${Math.Calc[${itemValue.Length} - 2].Int}]}g,]}${If[${Bool[${itemValue.Mid[${Math.Calc[${itemValue.Length} - 1].Int}]}]},${itemValue.Mid[${Math.Calc[${itemValue.Length} - 1].Int}]}s,]}${If[${Bool[${itemValue.Right[1]}]},${itemValue.Right[1]}c,]}
		
		| Set ini variables like stack size, (C), (ND) etc.
		/varset iniEntryVariables ${If[${InvSlot[${invSlot}].Item.Stackable},(${InvSlot[${invSlot}].Item.StackSize}),]}${If[${InvSlot[${invSlot}].Item.NoDrop},(ND),]}${If[${InvSlot[${invSlot}].Item.Lore},(L),]}${If[${InvSlot[${invSlot}].Item.Container},(C),]}
		
		| Check for a Loot_Ini entry
		/echo /if (!{Ini[${Loot_Ini},${itemName.Left[1]},${itemName} ${itemValue}${iniEntryVariables}].Length})
		/if (!${Ini[${Loot_Ini},${itemName.Left[1]},${itemName} ${itemValue}${iniEntryVariables}].Length}) {
			/echo /call WriteToIni "${Loot_Ini},${itemName.Left[1]},${itemName} ${itemValue}${iniEntryVariables}" ${If[${InvSlot[${invSlot}].Item.Container},Container,${If[${InvSlot[${invSlot}].Item.NoDrop},Skip,Keep${If[${InvSlot[${invSlot}].Item.Stackable},|${InvSlot[${invSlot}].Item.StackSize},]}]}]}
			/call WriteToIni "${Loot_Ini},${itemName.Left[1]},${itemName} ${itemValue}${iniEntryVariables}" ${If[${InvSlot[${invSlot}].Item.Container},Container,${If[${InvSlot[${invSlot}].Item.NoDrop},Skip,Keep${If[${InvSlot[${invSlot}].Item.Stackable},|${InvSlot[${invSlot}].Item.StackSize},]}]}]}
			/echo /docommand ${ChatToggle} Added: [${InvSlot[${invSlot}].Item.Name}(${Ini[${Loot_Ini},${itemName.Left[1]},${itemName} ${itemValue}${iniEntryVariables}]})] to [${Loot_Ini}].
			/docommand ${ChatToggle} Added: [${InvSlot[${invSlot}].Item.Name}(${Ini[${Loot_Ini},${itemName.Left[1]},${itemName} ${itemValue}${iniEntryVariables}]})] to [${Loot_Ini}].
			
			| Add scripts to import old loot settings from ninjaloot.ini, and old e3 loot inis
		}
		
	| If the item is in a bag
	} else {
	
		| Reset itemName if it contains a ':'.  ':'s cause errors when reading from the ini, because they act as a delimiter, just like '='s
		/varset itemName ${Me.Inventory[${invSlot}].Item[${itemSlot}].Name}
		/if (${itemName.Find[:]}) /varset itemName ${itemName.Replace[:,;]}
		
		| Set item value
		/varset itemValue ${Me.Inventory[${invSlot}].Item[${itemSlot}].Value}
		/varset itemValue ${If[${Bool[${itemValue.Left[${Math.Calc[${itemValue.Length} - 3].Int}]}]},${itemValue.Left[${Math.Calc[${itemValue.Length} - 3].Int}]}p,]}${If[${Bool[${itemValue.Mid[${Math.Calc[${itemValue.Length} - 2].Int}]}]},${itemValue.Mid[${Math.Calc[${itemValue.Length} - 2].Int}]}g,]}${If[${Bool[${itemValue.Mid[${Math.Calc[${itemValue.Length} - 1].Int}]}]},${itemValue.Mid[${Math.Calc[${itemValue.Length} - 1].Int}]}s,]}${If[${Bool[${itemValue.Right[1]}]},${itemValue.Right[1]}c,]}
		
		| Set ini variables like stack size, (C), (ND) etc.
		/varset iniEntryVariables ${If[${Me.Inventory[${invSlot}].Item[${itemSlot}].Stackable},(${Me.Inventory[${invSlot}].Item[${itemSlot}].StackSize}),]}${If[${Me.Inventory[${invSlot}].Item[${itemSlot}].NoDrop},(ND),]}${If[${Me.Inventory[${invSlot}].Item[${itemSlot}].Lore},(L),]}${If[${Me.Inventory[${invSlot}].Item[${itemSlot}].Container},(C),]}
		
		| Check for a Loot_Ini entry
		/if (!${Ini[${Loot_Ini},${itemName.Left[1]},${itemName} ${itemValue}${iniEntryVariables}].Length}) {
			/call WriteToIni "${Loot_Ini},${itemName.Left[1]},${itemName} ${itemValue}${iniEntryVariables}" ${If[${Me.Inventory[${invSlot}].Item[${itemSlot}].Container},Container,${If[${Me.Inventory[${invSlot}].Item[${itemSlot}].NoDrop},Skip,Keep${If[${Me.Inventory[${invSlot}].Item[${itemSlot}].Stackable},|${Me.Inventory[${invSlot}].Item[${itemSlot}].StackSize},]}]}]}
			/docommand ${ChatToggle} Added: [${Me.Inventory[${invSlot}].Item[${itemSlot}].Name}(${Ini[${Loot_Ini},${itemName.Left[1]},${itemName} ${itemValue}${iniEntryVariables}]})] to [${Loot_Ini}].
			
			| Add scripts to import old loot settings from ninjaloot.ini, and old e3 loot inis
		}
	}
	
	/varset lootSetting ${Ini[${Loot_Ini},${itemName.Left[1]},${itemName} ${itemValue}${iniEntryVariables}]}
	/if (${Debug} || ${Debug_Loot}) /echo |- get_lootSetting -| lootSetting=${lootSetting}
	
/if (${Debug} || ${Debug_Loot}) /echo <== get_lootSetting -|
/RETURN ${lootSetting}



|--------------------------------------------------------------------|
|- Loots items mobs you have killed, to destroy or autoinventory.	-|
|--------------------------------------------------------------------|
|- @param slotNum: The slot number you are looting from.			-|
|- @param handle: What to do with the item once you loot it.		-|
|--------------------------------------------------------------------|
SUB loot_Handle(int slotNum, handle)
/if (${Debug} || ${Debug_Loot}) /echo |- loot_Handle ==>

	/declare lootTimer timer local 50
	
	:retry_Loot
	/if (${Debug} || ${Debug_Loot}) /echo |- loot_Handle -| :retry_Loot
	
	| Try to loot the specified item from the corpse.
	/itemnotify loot${slotNum} leftmouseup
	/delay 5 ${Cursor.ID} || ${Window[ConfirmationDialogBox].Open} || ${Window[QuantityWnd].Open}
	
	| If the item has been looted, decide what to do with it.
	/if (${Cursor.ID}) {
	
		| Destroy the item.
		/if (${handle.Equal[destroy]}) {
			:destroy_Item
			/destroy
			
			| If the destroy attempt was unsuccessful, try again.	
			/if (${Cursor.ID} && ${lootTimer}) /goto :destroy_Item
		
		| Else, keep the item.
		} else {
			/call ClearCursor
		}

	| Else, if loot attempt was unsuccessful, try again.		
	} else {
	
		| Click yes on confirmation box.
		/if (${Window[ConfirmationDialogBox].Open}) {
			/notify ConfirmationDialogBox Yes_Button leftmouseup
		} else /if (${Window[QuantityWnd].Open}) {
			/notify QuantityWnd QTYW_Accept_Button leftmouseup
		}
		/if (${lootTimer}) /goto :retry_Loot
	}
	
/if (${Debug} || ${Debug_Loot}) /echo <== loot_Handle -|
/RETURN	



|----------------------------------------------------------------|
|- Adds corpses that are not yours to the looted corpse list.	-|
|----------------------------------------------------------------|
#EVENT NotYourKill "Someone is already looting that corpse."
#EVENT NotYourKill "You may not loot this Corpse at this time."
#EVENT NotYourKill "You must first target a corpse to loot!"
SUB EVENT_NotYourKill
/if (${Debug} || ${Debug_Loot}) /echo |- EVENT_NotYourKill ==>

	/if (${Defined[looted_Corpses]}) /varset looted_Corpses ${looted_Corpses}${corpseID},
	
/if (${Debug} || ${Debug_Loot}) /echo <== EVENT_NotYourKill -|
/RETURN SKIP_LOOT



|---------------
|- Check Loot Setting
|-----------------
#EVENT checkLootSetting "<#*#> Check Loot Setting for #1#"
#EVENT checkLootSetting "[MQ2] Check Loot Setting for #1#"
#EVENT checkLootSetting "<#*#> Check Loot Entry for #1#"
#EVENT checkLootSetting "[MQ2] Check Loot Entry for #1#"
SUB EVENT_checkLootSetting(line, itemName)
/if (${Debug} || ${Debug_Loot}) /echo |- EVENT_checkLootSetting ==>

	/declare itemToCheck string local ${itemName}
	/if (${itemToCheck.Find[:]}) /varset itemToCheck ${itemToCheck.Replace[:,;]}

	/declare i int local 1
	
	:check_Loot_Ini
	
	/if (!${Ini[${Loot_Ini},${itemToCheck.Left[1]}].Arg[${i},|].Arg[1,${If[${Ini[${Loot_Ini},${itemToCheck.Left[1]}].Arg[${i},|].Find[~]},~,(]}].Equal[${itemToCheck}]}) {
		/varcalc i ${i} + 1
		/if (${Ini[${Loot_Ini},${itemToCheck.Left[1]}].Arg[${i},|].Length}) {
			/goto :check_Loot_Ini
		} else {
			/echo No entry found.
		}
	} else {
		/echo >>> [${itemName}=${Ini[${Loot_Ini},${itemToCheck.Left[1]},${Ini[${Loot_Ini},${itemToCheck.Left[1]}].Arg[${i},|]}]}] <<<
	}
	
/if (${Debug} || ${Debug_Loot}) /echo <== EVENT_checkLootSetting -|
/RETURN



|---------------
|- Update Loot Setting
|-----------------
#EVENT updateLootSetting "<#1#> Set Loot entry for #2# /#3#"
#EVENT updateLootSetting "[MQ2] Set Loot entry for #2# /#3#"
SUB EVENT_updateLootSetting(line, ChatSender, itemName, iniSetting)
/if (${Debug} || ${Debug_Loot}) /echo |- EVENT_updateLootSetting ==>
	
	/if (${ChatSender.Equal[${Me.CleanName}]} || !${Defined[ChatSender]}) {
	
		/declare itemToCheck string local ${itemName}
		/if (${itemToCheck.Find[:]}) /varset itemToCheck ${itemToCheck.Replace[:,;]}
	
		/declare i int local 1
		
		:check_Loot_Ini
		/if (!${Ini[${Loot_Ini},${itemToCheck.Left[1]}].Arg[${i},|].Arg[1,${If[${Ini[${Loot_Ini},${itemToCheck.Left[1]}].Arg[${i},|].Find[~]},~,(]}].Equal[${itemToCheck}]}) {
			/varcalc i ${i} + 1
			/if (${Ini[${Loot_Ini},${itemToCheck.Left[1]}].Arg[${i},|].Length}) {
				/goto :check_Loot_Ini
			} else {
				/echo No entry found.
			}
		} else {
		
			/call WriteToIni "${Loot_Ini},${itemToCheck.Left[1]},${Ini[${Loot_Ini},${itemToCheck.Left[1]}].Arg[${i},|]}" "${iniSetting}" 1
			/echo >>> Loot_Ini entry for [${itemName}] has been changed to [${Ini[${Loot_Ini},${itemToCheck.Left[1]},${Ini[${Loot_Ini},${itemToCheck.Left[1]}].Arg[${i},|]}]}]. <<<
		}
	}
	
/if (${Debug} || ${Debug_Loot}) /echo <== EVENT_updateLootSetting -|
/RETURN



SUB loot_Setup
/call INItoVar "${advSettings_Ini},Debug,Debug Loot (On/Off)" Debug_Loot bool outer
/if (${Debug} || ${Debug_Loot}) /echo |- _Setup ==>

	| Declare looting variables
	/declare looted_Corpses string outer

	| Add Looting File Paths
	/if (!${Ini[${MacroData_Ini},File Paths,Loot Settings File].Length}) /call WriteToIni "${MacroData_Ini},File Paths,Loot Settings File" "e3 Macro Inis\Loot Settings.ini" 1
	
	| Find a filepath for the loot.ini.
	/if (!${Ini[${MacroData_Ini},File Paths,Loot Settings File].Length}) {
		/echo ERROR: Could not find designated file path for [Loot Settings File], please review [e3Data.ini] and restart.
		/endmacro
	} else {
		/declare Loot_Ini string outer ${Ini[${MacroData_Ini},File Paths,Loot Settings File]}
	}

	| Build a loot ini
	/if (!${Ini[${Loot_Ini}].Length}) /call Build_Alphabetized_Ini "${Loot_Ini}"
	
	| Import macro settings
	/call IniToVar "${genSettings_Ini},Loot,Auto-Loot Mode (Auto/Manual)" loot_Mode string outer
	/call IniToVar "${genSettings_Ini},Loot,Use MQ2LinkDB (On/Off)" use_linkDB bool outer
	/call IniToVar "${genSettings_Ini},Loot,Loot Link Channel (Requires MQ2LinkDB)" linkChannel string outer
	/call IniToVar "${genSettings_Ini},Loot,Corpse Seek Radius" seek_Radius int outer

	| Import character settings
	/if (${Bool[${Ini[${Character_Ini},Misc,Auto-Loot (On/Off)]}]}) /call IniToVar "${Character_Ini},Misc,Auto-Loot (On/Off)" Auto_Loot bool outer
	
	| Load MQ2LinkDB
	/if (${use_linkDB} && ${Auto_Loot} && !${Plugin[MQ2LinkDB].Name.Length}) {
		/echo Plugin MQ2LinkDB is not loaded, attempting to resolve...
		/plugin MQ2LinkDB noauto
		/delay 3s ${Plugin[MQ2LinkDB].Name.Length}
		/if (${Plugin[MQ2Exchange].Name.Length}) {
			/link /import
		} else {
			/echo ***WARNING*** Could not load MQ2LinkDB, item links will only be given in say.
			/varset LinkChannel PLUGIN-ERROR
		}
	}
	
/if (${Debug} || ${Debug_Loot}) /echo <== _Setup -|
/RETURN


 
Sub loot_Background_Events

/return



SUB loot_MacroSettings
/if (${Debug} || ${Debug_Loot}) /echo |- _MacroSettings ==>

	/call WriteToIni "${advSettings_Ini},Debug,Debug Loot (On/Off)"

	/call WriteToIni "${genSettings_Ini},Loot,Auto-Loot Mode (Auto/Manual)" Auto
	/call WriteToIni "${genSettings_Ini},Loot,Use MQ2LinkDB (On/Off)" On
	/call WriteToIni "${genSettings_Ini},Loot,Loot Link Channel (Requires MQ2LinkDB)" /say
	/call WriteToIni "${genSettings_Ini},Loot,Corpse Seek Radius" 50

/if (${Debug} || ${Debug_Loot}) /echo <== _MacroSettings -|
/RETURN



SUB loot_CharacterSettings
/if (${Debug} || ${Debug_Loot}) /echo |- _CharacterSettings ==>

	/call WriteToIni "${Character_Ini},Misc,Auto-Loot (On/Off)"

/if (${Debug} || ${Debug_Loot}) /echo <== _CharacterSettings -|
/RETURN



Sub loot_Aliases
/if (${Debug} || ${Debug_Loot}) /echo |- _Aliases ==>

/if (${Debug} || ${Debug_Loot}) /echo <== _Aliases -|
/return