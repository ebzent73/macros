| Ninjadvloot.inc v5.2 Beta 06/07/2015 Redguides.com 
| Author A_Druid_00 12/19/2005
| Based on original looting code of toomanynames
| The following updates were all individually posted and consolidated by Maskoi into the current version.
| Updated: Moeymoejoe added strip out comma in item names when accessing ini file 08/02/2006
| Updated: mystikule added sell option to ini file 01/07/2008
| Updated: drkrain for House of Thule 10/23/2010
| Updated: Maskoi with Sell Routines 11/27/2010
| Updated: Maskoi with /item keep/sell/destroy ini commands 07/21/2011
| Updated: Maskoi with Foraged item handling 08/31/2011
| Updated: Maskoi with Buying item handling 07/31/2012
| Updated: Maskoi with Assign different ini file for item handling 01/01/2013
| Updated: Crystane with Looting while on mount (no overshooting on corpserun), added some conditional checks to skip unecessary delay I found. Announcement routine reworked. 05/13/2014
| Updated: TreeHuginDruid hard coded 1 second delay between loot and corpse item retrieval to resolve loot latency issue.
| Updated: Maskoi added suppot for Advanced Looting system  05/18/2015
|
| Selling Contributions by
|   drkrain - TLInv item check
|   dewey2461 - Add sold items to ini file Event
|
| Requirements: MQ2Moveutils
| Usage: add the following to your macro for looting
|   #include Ninjadvloot.inc
|   /call SetupAdvLootVars
|   /call LootMobs
|
| Assign differnent ini files. Default is Loot.ini
|   /call SetupAdvLootVars "MyToonLoot.ini"
|   /call SetupAdvLootVars "LootMesa.ini"
|
| Commands
| Usage. Grab the item and put it on your cursor then use one of the following commands
|     /lootdestroy - Mark item on cursor to Destroy in Loot.ini file
|     /lootignore - Mark item on cursor to Ignore in Loot.ini file
|     /lootkeep - Mark item on cursor to Keep in Loot.ini file
|     /lootsell - Mark item on cursor to Sell in Loot.ini file
|     /sellstuff - Target a vendor and this command will sell any item in your inventory marked sell. 
|     /depositstuff - Target your guildbanker and this command will deposit any item in your inventory marked keep into the guild bank.

#Event CantLoot         "#*#may not loot this corpse#*#"
#Event Sell             "#*#You receive#*# for the #1#(s)#*#"
#Event EditIniItem      "[MQ2] LootIniItem #1# #2#"
#Event SellStuff        "[MQ2] NinjadvLoot selling items to vendor"
#Event Broke            "#*#you cannot afford#*#" 
#Event Broke            "#*#you can't afford#*#" 
#Event Forage           "Your forage mastery has enabled you to find something else!"
#Event Forage           "You have scrounged up #*#"
#Event InventoryFull    "#*#Your inventory appears full!#*#"
#Event NullSlot         "#*#Invalid item slot 'null#*#"
#Event Lore             "#*#You cannot loot this Lore Item.#*#"

Sub SetupAdvLootVars(string ChangeIniFile)

    /declare CantLootID                         int                 outer 
    /declare NinjadVersion                      string              outer   5.2
    /declare InventoryFull                      int                 outer   0
    /declare NinjaIniFile                       string              outer   Loot.ini
    /declare MasterCorpseList                   string              outer   
    /declare Debugloot                          int                 outer   0
    /declare GlobalCount                        int                 outer   5
    /declare GlobalList[${GlobalCount}]         string              outer  
    /varset GlobalList[1]                       string              outer  empty   
    /declare AutoHater                          int                 outer
    /if (${ChangeIniFile.Length}) {
        /varset NinjaIniFile ${ChangeIniFile}
        /echo Ninja Advance Loot in now writing all info to the ${ChangeIniFile} file.
    }
    /declare NALVersion                           string             outer   ${Ini[${NinjaIniFile},Settings,Version]}
    /if (!${Ini[${NinjaIniFile},Settings,Version].Length}) {
        /call CreateIni
        /ini "${NinjaIniFile}" "Settings" "Version" "0.0"
    }
    /if (${NALVersion.NotEqual[${NinjadVersion}]}) {
        /echo NINJADVLOOT: Version mismatch detected. Writing aliases.
        /noparse /squelch /alias /lootignore    /echo LootIniItem Ignore ${Cursor.Name}
        /noparse /squelch /alias /lootsell      /echo LootIniItem Sell ${Cursor.Name}
        /noparse /squelch /alias /lootkeep      /echo LootIniItem Keep ${Cursor.Name}
        /noparse /squelch /alias /lootdestroy   /echo LootIniItem Destroy ${Cursor.Name}
        /squelch /alias /sellstuff              /echo NinjadvLoot selling items to vendor
        /squelch /alias /depositstuff           /echo NinjadvLoot depositing items to guildbank
        /ini "${NinjaIniFile}" "Settings" "Version" "${NinjadVersion}"
    }

    | Loot ini settings
    /call NALLoadVar Settings AddNewSales        1                   int
    /call NALLoadVar Settings LootForage         1                   int
    /call NALLoadVar Settings LootMobs           TRUE                bool
    /call NALLoadVar Settings CorpseRadius       100                 int
    /call NALLoadVar Settings MobsTooClose       2                  int
|    /call NALLoadVar Settings MobsTooClose       40                  int
    /call NALLoadVar Settings ReportLoot         FALSE               bool
    /call NALLoadVar Settings LootChannel        g                   string
    /call NALLoadVar Settings LootForageSpam     1                   int
    /call NALLoadVar Settings GlobalLootOn       1                   int
    /call NALLoadVar Settings LootForageSpam     1                   int
    /call NALLoadVar Settings CombatLooting      0                   int
    /call FindAutoHate
    /echo NINJADVLOOT: Variables are set.

/return

Sub NALLoadVar(IniSection,IniVar,IniValue,VarType)
    /if (!${Defined[${IniVar}]} && ${Defined[VarType]}) /declare ${IniVar} ${VarType} outer
    /declare IniString string local ${Ini[${NinjaIniFile},${IniSection},${IniVar},NOTFOUND]}
    /varset ${IniVar} ${IniString}
    /if (${IniString.Equal["NOTFOUND"]}) {
        /if (${IniString.Equal["NOTFOUND"]}) /varset ${IniVar} ${IniValue}
        /ini "${NinjaIniFile}" "${IniSection}" "${IniVar}" "${${IniVar}}"
    } 
/return

Sub FindAutoHate
    /declare i int local
    /for i 1 to 10
        /if (${Me.XTarget[${i}].TargetType.Equal[Auto Hater]}) {
            /varset AutoHater ${i}
            /echo NINJADVLOOT: AutoHater set to Xtarget${i}
            /return
        }
    /next i
/return

| ****************  Loot Mobs Section ******************** |

Sub LootMobs
    /declare DeadCount int local ${SpawnCount[corpse radius ${CorpseRadius} zradius 50]}
    /if (${Debugloot}) /echo DEBUGLOOT There are ${DeadCount} corpses in range.
    /if (${Debugloot}) /echo DEBUGLOOT !${LootMobs} || ${SpawnCount[npc radius ${MobsTooClose} zradius 50]} || !${DeadCount} || ${Me.Combat} || !${Me.FreeInventory} || ${Me.XTarget[${AutoHater}].ID} && !${CombatLooting})
    /if (!${LootMobs} || ${SpawnCount[npc radius ${MobsTooClose} zradius 50]} || !${DeadCount} || ${Me.Combat} || !${Me.FreeInventory} || ${Me.XTarget[${AutoHater}].ID} && !${CombatLooting}) /return
    /declare i int local
    /declare CorpseList string local 
    /declare CorpseToLoot int local
    /declare ZDist float local ${Math.Distance[${Target.Z}:${Me.Z}]}
    /declare ClosestCorpse int local
    /declare CorpseCount int local 0
    /for i 1 to ${DeadCount}
        /varset ClosestCorpse ${NearestSpawn[${i},corpse radius ${CorpseRadius} zradius 50].ID}
        /if (!${Spawn[${ClosestCorpse}].Deity.ID}) {
            /varset CorpseList ${CorpseList}${ClosestCorpse}|
            /if (!${MasterCorpseList.Find[${ClosestCorpse}]}) /varset MasterCorpseList ${MasterCorpseList}${ClosestCorpse}|
            /varcalc CorpseCount ${CorpseCount}+1
        }
    /next i
    /if (${Debugloot}) /echo DEBUGLOOT Trying to loot ${CorpseCount} corpses. ID's ${CorpseList}
    /for i 1 to ${CorpseCount} 
    /varset CorpseToLoot ${CorpseList.Arg[${i},|]}
    /if (!${Defined[CorpseLocked${CorpseToLoot}]}) /declare CorpseLocked${CorpseToLoot} timer outer 0
        /if (${Spawn[${CorpseToLoot}].Deity.ID} || !${Spawn[${CorpseToLoot}].ID} || !${Spawn[${CorpseToLoot}].LineOfSight} || ${CorpseLocked${CorpseToLoot}} || ${Me.XTarget[${AutoHater}].ID} && !${CombatLooting}) /goto :SkipCorpse
            /if (${Target.ID}!=${CorpseToLoot}) /target id ${CorpseToLoot}
            /if (!${Me.Standing}) /stand
            /delay 3s ${Target.ID}==${CorpseToLoot} && ${Me.Standing}
            /if (${Target.Distance}>5) {
                /if (${Debugloot}) /echo DEBUGLOOT move to corpse
                /moveto mdist 10
                /moveto loc ${Target.Y} ${Target.X} ${Target.Z} 
        /if (${Me.Mount.ID}) {
            /delay 3s ${Math.Distance[${Target.Y},${Target.X}]}<25
            /keypress back
        }
                /delay 3s ${MoveTo.Stopped} && !${Me.Moving}
                /call ZCheck ${ZDist} 8
                /if (${Target.ID}) /face fast
            }
            /if (${Target.Distance}<12) {
                /if (${Debugloot}) /echo DEBUGLOOT /Call LootCorpse
                /call LootCorpse ${Target.ID}
                /call RemoveCorpseID ${CorpseToLoot}
            }
        :SkipCorpse
    /next i
    /if (${Debugloot}) /echo DEBUGLOOT Leave Sub LootMobs 
/return

Sub LootCorpse(CorpseID)
    /if (${Debugloot}) /echo DEBUGLOOT Enter Sub LootCorpse
    /declare i              int    local
    /declare j              int    local
    /declare LootList       string local
    /declare FirstLetter    string local
    /declare GCheck            string local
    /call CheckCursor
    /varset CantLootID 0
    /if (${Debugloot}) /echo DEBUGLOOT /loot
    | Open Loot window
    /loot
    /if (${Debugloot}) /echo DEBUGLOOT /doevents CantLoot
    /delay 3s ${Window[LootWnd]}
    /delay 1s
    /doevents CantLoot
    /delay 3s ${CantLootID} || ${Window[LootWnd]}
    /if (${Target.ID}==${CantLootID} && !${Window[LootWnd]}) {
        /echo NINJADVLOOT: I can't loot ${Target.CleanName} ID:${CorpseID} right now.
        /if (${Defined[CorpseLocked${CorpseID}]}) /varset CorpseLocked${CorpseID} 150s
        /return
    }
    /if (${Debugloot}) /echo DEBUGLOOT Loot Window Open:${Window[LootWnd]} - # Loot Items:${Corpse.Items}
    /if (${Window[LootWnd].Open} && ${Corpse.Items}) {
        /declare loottotal int  local
        :LootLag
        /varset loottotal ${Corpse.Items}
             /if (${Debugloot}) /echo DEBUGLOOT :LootLag ${loottotal}!=${Corpse.Items} LagCount${LagCount}
        /delay 1s ${loottotal}!=${Corpse.Items}
        /if (${loottotal}!=${Corpse.Items}) /goto :LootLag
        /echo NINJADVLOOT: Looting ${Target.CleanName} ID:${CorpseID}.
        /for i 1 to ${loottotal}
            /if (${Corpse.Item[${i}].Lore} && ${FindItem[=${Corpse.Item[${i}]}].ID}) {
                /varset LootList ${LootList}${Corpse.Item[${i}]},
                /goto :LootedInGlobal
            }
            /if (${Debugloot}) /echo DEBUGLOOT Looting ${Corpse.Item[${i}].Name} ${i} of ${loottotal} 
            | Global loot section
            /if (${GlobalLootOn}) {
                /for j 1 to ${GlobalCount}
                    /varset GCheck ${Ini[${NinjaIniFile},"Global","Global${j}"]}
                    | /echo ${GCheck.Arg[1,|]} ${GCheck.Arg[2,|]}
                    /if (${Corpse.Item[${i}].Name.Find[${GCheck.Arg[1,|]}]}) {
                        /if (${Select[${GCheck.Arg[2,|]},sell,keep]} && (!${Corpse.Item[${i}].Lore} || !${FindItem[=${Corpse.Item[${i}]}].ID}) && (${Me.FreeInventory} || (${FindItemCount[=${Corpse.Item[${i}].Name}]} && ${Corpse.Item[${i}].Stackable} && ${Corpse.Item[${i}].FreeStack}))) /call LootItem ${i} Keep Right
                        /if (${GCheck.Arg[2,|].Equal[Ignore]}) /varset LootList ${LootList}${Corpse.Item[${i}]},
                        /if (${GCheck.Arg[2,|].Equal[Destroy]}) /call LootItem ${i} Destroy left
                        /goto :LootedInGlobal
                    }
                /next j
            }
            /varset FirstLetter ${Corpse.Item[${i}].Name.Left[1]}
            /if (${Corpse.Item[${i}].ID} && !${Select[${Ini[${NinjaIniFile},"${FirstLetter}","${Corpse.Item[${i}]}"]},Ignore,Keep,Destroy,Sell]}) {
                /if (${Corpse.Item[${i}].NoDrop})  /ini "${NinjaIniFile}" "${FirstLetter}" "${Corpse.Item[${i}]}" Ignore
                /if (!${Corpse.Item[${i}].NoDrop}) /ini "${NinjaIniFile}" "${FirstLetter}" "${Corpse.Item[${i}]}" Keep
            }  
            /if (${Ini[${NinjaIniFile},"${FirstLetter}","${Corpse.Item[${i}]}"].NotEqual[Destroy]} && !${Me.FreeInventory} && (!${FindItemCount[=${Corpse.Item[${i}].Name}]} || (${FindItemCount[=${Corpse.Item[${i}].Name}]} && ${Corpse.Item[${i}].Stackable} && !${Corpse.Item[${i}].FreeStack})) || (${Corpse.Item[${i}].Lore} && ${FindItem[=${Corpse.Item[${i}]}].ID}) || ${Ini[${NinjaIniFile},"${FirstLetter}","${Corpse.Item[${i}]}"].Equal[Ignore]}) /varset LootList ${LootList}${Corpse.Item[${i}]},
            /if ((${Ini[${NinjaIniFile},"${FirstLetter}","${Corpse.Item[${i}]}"].Equal[Keep]} || ${Ini[${NinjaIniFile},"${FirstLetter}","${Corpse.Item[${i}]}"].Equal[Sell]}) && (!${Corpse.Item[${i}].Lore} || !${FindItem[=${Corpse.Item[${i}]}].ID}) && (${Me.FreeInventory} || (${FindItemCount[=${Corpse.Item[${i}].Name}]} && ${Corpse.Item[${i}].Stackable} && ${Corpse.Item[${i}].FreeStack}))) /call LootItem ${i} Keep Right
            /if (${Ini[${NinjaIniFile},"${FirstLetter}","${Corpse.Item[${i}]}"].Equal[Destroy]}) /call LootItem ${i} Destroy left
            :LootedInGlobal
        /next i
        /if (${Corpse.Items}) {
            /if (${ReportLoot} && ${LootList.Length}) {
                /keypress /
                /delay 3
                /call Type "${LootChannel}"
                /keypress Space chat
                /notify LootWND LW_BroadcastButton leftmouseup 
                /delay 3
                /call Type " ,left on corpse."
                /keypress Enter chat
                /if (${Defined[CorpseLocked${CorpseID}]}) /varset CorpseLocked${CorpseID} 440s
            }
        }
    }
    :clickdoneffs
        /nomodkey /notify LootWnd LW_DoneButton leftmouseup
        /if (${Debugloot}) /echo DEBUGLOOT :clickdoneffs 
        /delay 5s !${Window[LootWnd].Open}
        /if (${Debugloot}) /echo DEBUGLOOT :clickdoneffs LootWin Open:${Window[LootWnd]}
    /if (${Window[LootWnd].Open}) /goto :clickdoneffs
    /call RemoveCorpseID ${CorpseID}
    /if (${Debugloot}) /echo DEBUGLOOT Leave Sub LootCorpse
/return

Sub Event_CantLoot
   /if (${Debugloot}) /echo DEBUGLOOT Enter Sub Event_CantLoot
   /varset CantLootID ${Target.ID}
   /if (${Debugloot}) /echo DEBUGLOOT  Event_CantLoot Can't loot ${Target.CleanName} ${Target.ID}
   /if (${Debugloot}) /echo DEBUGLOOT Leave Sub Event_CantLoot
/return

Sub RemoveCorpseID(CListID)
    /if (${Defined[CorpseLocked${CListID}]}) {
        /if (!${CorpseLocked${CListID}} && ${MasterCorpseList.Find[${CListID}]}) {
            /deletevar CorpseLocked${CListID}
            /varset MasterCorpseList ${MasterCorpseList.Replace[${CListID}|,]}
        }
    }
/return

Sub LootItem(int i,DoWhat,WhichButton)
    /if (${Debugloot}) /echo DEBUGLOOT Enter Sub LootItem ${i} ${DoWhat} ${WhichButton}
    /declare CorpseItemID int local ${Corpse.Item[${i}].ID}
    /nomodkey /shift /itemnotify ${Corpse.Item[${i}].InvSlot} ${WhichButton}mouseup
    /delay 5s ${Window[ConfirmationDialogBox].Open} || !${Corpse.Item[${i}].NoDrop}
    /if (${Window[ConfirmationDialogBox].Open}) /nomodkey /notify ConfirmationDialogBox Yes_Button leftmouseup
    /delay 5s ${Cursor.ID} || ${WhichButton.NotEqual[left]}
    /if (${DoWhat.Equal[Destroy]} && ${Cursor.ID}==${CorpseItemID}) /destroy
    /delay 5s !${Corpse.Item[${i}].ID}
    /if (${Cursor.ID}) /call CheckCursor
    /if (${Debugloot}) /echo DEBUGLOOT Leave Sub LootItem
/return

Sub CheckCursor
    /if (${Debugloot}) /echo DEBUGLOOT Enter Sub CheckCursor
    :ALauto_inv
    /if (${Cursor.ID}) {
        /if (${Cursor.NoDrop} && !${Me.FreeInventory}) /return
        /timed 5 /autoinventory
        /delay 2s !${Cursor.ID}
        /goto :ALauto_inv
    }
    /if (${Debugloot}) /echo DEBUGLOOT Leave Sub CheckCursor
/return

| **************** Foraged Items Section ******************** |
Sub Event_Forage
    /if (!${LootForage}) /return
    /declare ForagedItem string local
    :MoreToAutoInventory
        /if (${Cursor.ID}) {
            /varset ForagedItem ${Cursor.Name}
            /if (!${Select[${Ini[${NinjaIniFile},"${Cursor.Name.Left[1]}","${Cursor.Name}"]},Ignore,Keep,Destroy,Sell]}) {
                /if (${Cursor.ID}) /ini "${NinjaIniFile}" "${Cursor.Name.Left[1]}" "${Cursor.Name}" Keep
                /if (${LootForageSpam}) /echo New foraged item >> ${Cursor.Name} << added to ini file. 
            }
            /if (${Ini[${NinjaIniFile},"${Cursor.Name.Left[1]}","${Cursor.Name}"].Equal[Destroy]} || ${Ini[${NinjaIniFile},"${Cursor.Name.Left[1]}","${Cursor.Name}"].Equal[Ignore]}) {
                /if (${Cursor.Name.Equal[${ForagedItem}]})     /destroy
                /delay 5 !${Cursor.ID}
                /if (${LootForageSpam}) /echo NINJADVLOOT --  Destroyed foraged ${ForagedItem}.
            } else /if ((${Ini[${NinjaIniFile},"${Cursor.Name.Left[1]}","${Cursor.Name}"].Equal[Keep]} || ${Ini[${NinjaIniFile},"${Cursor.Name.Left[1]}","${Cursor.Name}"].Equal[Sell]}) && (!${Cursor.Lore} || !${FindItem[=${Cursor.Name}].ID}) && (${Me.FreeInventory} || (${FindItemCount[=${Cursor.Name}]} && ${Cursor.Stackable} && ${Cursor.FreeStack}))) {
                /if (${LootForageSpam}) /echo NINJADVLOOT ++ Kept foraged ${ForagedItem}.
                /autoinventory
                /delay 5 !${Cursor.ID}
            }
        }
    /if (${Cursor.ID}) /goto :MoreToAutoInventory
/return

| **************** Search Loot Section ******************** |
Sub DoLootStuff(LootAction)
    /declare i int local
    /declare j int local
    /declare k int local
    /declare LootItem string
   /call NPC "${Target.CleanName}"
   | Top level inv has its own loop since ${FindItem[=${ItemToSell}].InvSlot} looks at top lev inv slots first in Sub SellToVendor for the item to sell.
   | Loop through top level inventory slots for sellable items
   /for k 1 to 10
        | Check if top level inv slot is not a container/bag
        /if (${InvSlot[pack${k}].Item.Container}==0) {
            | If top level inv slot is empty check for an item marked sell
            /if (${InvSlot[pack${k}].Item.ID}) {
                /varset LootItem ${InvSlot[pack${k}].Item}
                /if (${LootAction.Equal[sell]})  {
                /if (${Ini[${NinjaIniFile},"${LootItem.Left[1]}","${LootItem}"].Equal[Sell]}) /call SellToVendor "${LootItem}"
                }
                /if (${LootAction.Equal[deposit]})  {
                    /if (${Ini[${NinjaIniFile},"${LootItem.Left[1]}","${LootItem}"].Equal[Keep]}) /call DepositToGuildBank "${LootItem}"
                }
            }
        }
    /next k      
    | Loop through top level inventory slots for containers
    /for i 1 to 10
        | Check if top level inv slot has a bag
        /if (${InvSlot[pack${i}].Item.Container}==0) /next i
        | Set j 1 to number of bag slots and loop through slots looking for items marked sell
        /for j 1 to ${InvSlot[pack${i}].Item.Container}
            /if (${InvSlot[pack${i}].Item.Item[${j}].ID}) {
            /varset LootItem ${InvSlot[pack${i}].Item.Item[${j}].Name}
            /if (${LootAction.Equal[sell]})  {
                /if (${Ini[${NinjaIniFile},"${LootItem.Left[1]}","${LootItem}"].Equal[Sell]}) /call SellToVendor "${LootItem}"
                }
                /if (${LootAction.Equal[deposit]})  {
                    /if (${Ini[${NinjaIniFile},"${LootItem.Left[1]}","${LootItem}"].Equal[Keep]}) /call DepositToGuildBank "${LootItem}"
                }
            }
        /next j
    /next i
    | Prevent spam from Sell event after selling items in Sub SellStuff
    /doevents flush Sell
    /if (${Window[MerchantWnd].Open}) {
        /echo Closing merchant window
        /nomodkey /notify MerchantWnd MW_Done_Button LeftMouseUp
    }
    /if (${Window[GuildBankWnd].Open}) {
        /echo Closing guildbank window
        /keypress esc
    }
/return
   
Sub NPC(string NPCName)
    /if (${NPCName.Equal[null]} && ${Target.Type.Equal[npc]}) /varset NPCName ${Target.CleanName}
    /target npc ${NPCName}
    /delay 10
        /if (!${Select[${Target.Type},npc,pet]} || !${Target.ID} || ${Target.Type.Equal[pet]} && !${Target.CleanName.Find[familiar]}) {
        /echo Please target a vendor or guildbanker
        /return
    }
    /delay 1s
    /echo Doing business with ${NPCName}
    /target id ${Spawn[${NPCName}].ID}
    /delay 3s ${Target.ID}==${Spawn[${NPCName}].ID}
    /if (${Target.Distance}>15) {
        /moveto id  ${Spawn[${NPCName}].ID}
        /delay 250 ${MoveTo.Stopped}
    }
    /face nolook
    /delay 1s
    /echo Opening merchant/guildbank window
    /nomodkey /click right target
    /echo Waiting 5s for merchant/guildbank window to populate
    /delay 5s
/return

| **************** Sell Loot Section ******************** |
Sub Event_Sell(string soldstr, string ItemName)
    /if ( ${AddNewSales} ) {
        /echo Setting ${ItemName} to sell
        /ini "${NinjaIniFile}" "${ItemName.Left[1]}" "${ItemName}" Sell
    }
/return

Sub SellToVendor(ItemToSell)
    /if (${Window[MerchantWnd].Open}) {
    :sell
        /echo Selling ${ItemToSell}
        /if (${InvSlot[pack${Select[${FindItem[=${ItemToSell}].ItemSlot},23,24,25,26,27,28,29,30,31,32]}].Item.Container}) {
            /nomodkey /itemnotify in Pack${Math.Calc[${FindItem[=${ItemToSell}].ItemSlot}-22]} ${Math.Calc[${FindItem[=${ItemToSell}].ItemSlot2}+1]} leftmouseup
        } else {
            /nomodkey /itemnotify ${FindItem[=${ItemToSell}].InvSlot} leftmouseup
        }
        /delay 5
        /nomodkey /shiftkey /notify merchantwnd MW_Sell_Button leftmouseup
        /delay 2s
    /if (${FindItemCount[=${ItemToSell}]}) /goto :sell
    }
/return

| **************** Deposit Loot Section ******************** 
Sub DepositToGuildBank(ItemToDeposit)
    /if (${Window[GuildBankWnd].Open}) {
        :deposit
            /if (${Window[GuildBankWnd].Child[GBANK_DepositCountLabel].Text.Arg[2,:]}==0) {
                /echo Guildbank Deposited Items is full.
                /return
            }
            /if (${FindItem[=${ItemToDeposit}].InvSlot}<=22) /return
            /echo Depositing ${ItemToDeposit}
            /if (!${InvSlot[pack${Float[${Math.Calc[${Select[${FindItem[=${ItemToDeposit}].ItemSlot},22,23,24,25,26,27,28,29,30,31,32]}-1]}].Int}].Item.Container}) {
                /nomodkey /shift /itemnotify ${FindItem[=${ItemToDeposit}].InvSlot} leftmouseup
            } else {
                /nomodkey /shift /itemnotify in Pack${Math.Calc[${FindItem[=${ItemToDeposit}].ItemSlot}-22]} ${Math.Calc[${FindItem[=${ItemToDeposit}].ItemSlot2}+1]} leftmouseup
            }
            /delay 5
            /nomodkey /notify GuildBankWnd GBANK_DepositButton leftmouseup
            /delay 2s
        /if (${FindItem[=${ItemToDeposit}].InvSlot}) /goto :deposit
    }
/return

| **************** Item set ini file event ******************** 
    Sub Event_EditIniItem(ItemString,ItemAction,ItemName)
        /if (${Cursor.ID}) {
            /echo -- Setting ${ItemName} to ${ItemAction}
            /ini "${NinjaIniFile}" ${ItemName.Left[1]} "${ItemName}" ${ItemAction}
        } else {
            /echo No item on cursor.
        }
    /return

| **************** Sell Stuff Event ******************** 
    Sub Event_SellStuff
        /call DoLootStuff sell
    /return
    
| **************** Deposit Stuff Event *******************
    Sub Event_DepositStuff
        /call DoLootStuff deposit
    /return

| **************** Broke Event ******************** 
Sub Event_Broke 
    /if (!${Window[MerchantWnd].Open}) /return
    /echo You are out of money! 
    /endmacro 
/return

| **************** Type Stuff  ******************** 
| Lifted from commonsubs.inc by Armysoldier    
Sub Type(InStr) 
    /declare char string local 
    /declare loopctr int local 
    /for loopctr 1 to ${InStr.Length} 
        /varset char ${InStr.Mid[${loopctr},1]} 
        /if (!${char.Length}) { 
            /nomodkey /keypress space chat 
        } else { 
            /nomodkey /keypress ${char} chat 
        } 
    /next loopctr 
/return 

| **************** Buy Stuff ********************  
Sub Buy(string ItemToBuy, int amount)
    /if (!${Select[${Target.Type},npc,pet]} || !${Target.ID} || ${Target.Type.Equal[pet]} && !${Target.CleanName.Find[familiar]}) /return
    /declare i int local
    | /echo Buying ${ItemToBuy}!
    /declare QTY int local
    /declare ListItem int local
    /varcalc QTY ${amount}-${FindItemCount[${ItemToBuy}]}
    /delay 3
    /if (${FindItemCount[${ItemToBuy}]}>=${amount}) {
        /echo I already have ${FindItemCount[${ItemToBuy}]} ${ItemToBuy}.
        /return
    }   
    /varset ListItem ${Window[MerchantWnd].Child[ItemList].List[=${ItemToBuy},2]}
    /if (!${ListItem}) {
        /echo couldn't find ${ItemToBuy}
        /return
    } else {
        /notify MerchantWnd ItemList listselect ${ListItem}
        /delay 5       
    } i
    /echo Buying ${ItemToBuy} Till I get ${amount} 
    :Loop
    /doevents
    /if (${InventoryFull}) /return
    /if (${QTY}>0) {
        /if (${QTY}>19) {
            /Shiftkey /notify merchantwnd MW_Buy_Button leftmouseup
            /delay 5 ${FindItemCount[B${ItemToBuy}]}>=${QTY}
            /echo ${FindItemCount[${ItemToBuy}]}/${amount} ${ItemToBuy} in inventory
            /varcalc QTY ${amount}-${FindItemCount[${ItemToBuy}]} 
            /if (${QTY}<=0) {
                /if (${Window[MerchantWnd].Open}) {
                    /echo Closing merchant window
                    /nomodkey /notify MerchantWnd MW_Done_Button LeftMouseUp
                }
                /return
            }
            /delay 5
            /if (${QTY}>19) /goto :Loop
        }
        /if (${QTY}>0 && ${QTY}<20) {
            /for i 1 to ${QTY} 
                /Ctrlkey /notify merchantwnd MW_Buy_Button leftmouseup
                /delay 1s ${FindItemCount[${ItemToBuy}]}>=${Math.Calc[${FindItemCount[${ItemToBuy}]}+${i}]}
                /echo Buying ${ItemToBuy} ${i} of ${QTY}
                /if (${i}>=${QTY}) {
                    /echo ${FindItemCount[${ItemToBuy}]} ${ItemToBuy} in inventory
                    /if (${Window[MerchantWnd].Open}) {
                        /echo Closing merchant window
                        /nomodkey /notify MerchantWnd MW_Done_Button LeftMouseUp
                    }
                    /return
                }
            /next i
            /varcalc QTY ${amount}-${FindItemCount[${ItemToBuy}]}
            
        }
    }
/return
    
Sub ZCheck(float Zcur, float Zdiff)
    | Move down if Z distance more than Zcheck due to levitation
    /if (${Zcur}>=${Zdiff}) {
        /keypress end hold
        /delay 10
        /keypress end
    }
/return

Sub Event_InventoryFull
    /echo Your Inventory is full. Turning off looting functions.
    /varset LootMobs FALSE
    /varset InventoryFull 1
/return

Sub CreateIni
    /if (${Ini[${NinjaIniFile}].Length}) /return
    /declare a local Settings|Global|A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z
    /echo Creating ini file ${NinjaIniFile}
    /declare i int local 
    /declare j int local 
    /for i 1 to 27
        /ini ${NinjaIniFile} "${a.Arg[${i},|]}" "|===================================" "==================================|"
        /if (${i}==2) {
            /for j 1 to ${GlobalCount}
                /ini ${NinjaIniFile} "Global" "Global${j}" "Item Name|Ignore/Destroy/Keep/Sell"
            /next j
        }
    /next i
/return

Sub Event_NullSlot
        /echo NINJADVLOOT: Opening all bags to correct null slot
        /keypress OPEN_INV_BAGS
    /timed 30 /keypress CLOSE_INV_BAGS
/return

Sub UseAdvLoot
    /if (!${Me.FreeInventory}) /return
    /if (${Debugloot})  /echo Enter UseAdvLoot
    /if (${Me.UseAdvancedLooting}) {
        /if (${Debugloot}) /echo Start Loot Check
        /delay 30 ${Window[AdvancedLootWnd].Open}
        /declare i int local
        /declare j int local
        /declare k int local
        /declare ItemsToProcess string local
        /declare ITSCount int local 0
        /declare ITPCount int local 0
        /declare ITPName string local
        /declare ITPList string local
        /declare IniEntry string local
        | Check Shared loot for Keep,Destroy,Sell and que for Personal List if found
        /if (${AdvLoot.SList[1].ID} && !${Group.MasterLooter.ID}) /echo NINJADVLOOT: Please Assign a player the Master Looter Role in the group window.
        /if (${Group.MasterLooter.ID}==${Me.ID}) {
            /varset ITSCount ${AdvLoot.SCount}
            /for i ${ITSCount} downto 1 
                /varset ITPName ${AdvLoot.SList[${i}].Name}
                /if (${AdvLoot.SList[${i}].ID} && !${Select[${Ini[${NinjaIniFile},"${ITPName.Left[1]}","${ITPName}"]},Ignore,Keep,Destroy,Sell]}) {
                      | Add item to ini if it not in there
                    /if (${AdvLoot.SList[${i}].NoDrop}) {
                        /ini "${NinjaIniFile}" "${ITPName.Left[1]}" "${ITPName}" Ignore
                        /echo NINJADVLOOT: Adding ${ITPName}=Ignore to ini file.
                    } else {
                        /ini "${NinjaIniFile}" "${ITPName.Left[1]}" "${ITPName}" Keep
                        /echo NINJADVLOOT: Adding ${ITPName}=Keep to ini file.
                    }
                }  
                /if (${Select[${Ini[${NinjaIniFile},"${ITPName.Left[1]}","${ITPName}"]},Ignore]}) {
                    /if (${Debugloot}) /echo SharedList: ${ITPName} is set to Ignore. Leaving item. 
                    /advloot shared ${i} leave
                    /delay 5
                    /goto :NextItem
                }
                | skip item if its lore and I have one
                /if (${FindItem[${ITPName}].Lore}) {
                    /if (${Debugloot}) /echo SharedList: ${ITPName} is Lore and I have one. Skipping item.
                    /advloot shared ${i} leave
                    /goto :NextItem
                }
                /if (${AdvLoot.SList[${i}].ID}) {
                    /varset IniEntry ${Ini[${NinjaIniFile},"${ITPName.Left[1]}","${ITPName}"]}
                    /if (${Debugloot}) /echo SharedList: ${i} ${ITPName}=${IniEntry}
                    /if (${Select[${IniEntry},Keep,Destroy,Sell]}) {
                        /advloot shared ${i} giveto ${Me.Name}
                        /delay 10
                        /if (${Debugloot}) /echo SharedList: ${i} ${ITPName} giveto ${Me.Name}
                    }
                }
                :NextItem
            /next i
        }
        | Loot/destroy anything in personal list
        /if (${AdvLoot.PCount}) {
            /varset ITPCount ${AdvLoot.PCount}
            /for k ${ITPCount} downto 1
                /varset ITPName ${AdvLoot.PList[${k}].Name}
                /varset IniEntry ${Ini[${NinjaIniFile},"${ITPName.Left[1]}","${ITPName}"]}
                | Add item to ini if it not in there
                /if (${AdvLoot.PList[${k}].ID} && !${Select[${Ini[${NinjaIniFile},"${ITPName.Left[1]}","${ITPName}"]},Ignore,Keep,Destroy,Sell]}) {
                    /if (${AdvLoot.PList[${k}].NoDrop}) {
                        /ini "${NinjaIniFile}" "${ITPName.Left[1]}" "${ITPName}" Ignore
                        /echo NINJADVLOOT: Adding ${ITPName}=Ignore to ini file.
                        /goto :NextPItem
                    } else {
                        /ini "${NinjaIniFile}" "${ITPName.Left[1]}" "${ITPName}" Keep
                        /echo NINJADVLOOT: Adding ${ITPName}=Keep to ini file.
                    }
                } 
                | skip item if its lore and i have one
                /if (${FindItem[${ITPName}].Lore}) {
                    /if (${Debugloot}) /echo PList: ${ITPName} is Lore and I have one. Skipping item.
                    /advloot personal ${k} leave
                    /goto :NextPItem
                }
                /if (${Select[${Ini[${NinjaIniFile},"${ITPName.Left[1]}","${ITPName}"]},Ignore]}) {
                    /if (${Debugloot}) /echo PList: ${ITPName} is Lore and I have one. Skipping item.
                    /advloot personal ${k} leave
                    /delay 5
                    /goto :NextPItem
                }
                /if (${Select[${Ini[${NinjaIniFile},"${ITPName.Left[1]}","${ITPName}"]},Keep,Destroy,Sell]}) {
                    /if (${Debugloot}) /echo PList: ${k} ${ITPName}=${IniEntry}
                    /advloot personal ${k} loot
                    /delay 10
                    /if (${Window[ConfirmationDialogBox].Open}) {
                        /nomodkey /notify ConfirmationDialogBox Yes_Button leftmouseup
                        /delay 10
                    }
                    /if (${Select[${IniEntry},Destroy]}) {
                        /call HandleItem "${ITPName}" Destroy
                    }
         
                }
                :NextPItem
            /next k 
        }
    }
     /if (${Debugloot})  /echo Leave UseAdvLoot
/return

Sub HandleItem(ItemName, DoWhat)
    /if (${Debugloot}) /echo DEBUGLOOT Enter Sub HandleItem ${ItemName} ${DoWhat}
    /declare HandleTimer timer local 5s
    /if (${DoWhat.Equal[Destroy]} && ${FindItem[=${ItemName}].ID}) {
        :GetItem
            /nomodkey /shift /itemnotify in Pack${Math.Calc[${FindItem[=${ItemName}].ItemSlot}-22]} ${Math.Calc[${FindItem[=${ItemName}].ItemSlot2}+1]} leftmouseup
            /delay 1s ${Cursor.ID}
        /if (!${Cursor.ID} && ${HandleTimer}) /goto :GetItem
        /if (${Cursor.ID}==${FindItem[=${ItemName}].ID}) {
            /echo NINJADVLOOT: Destroying ${Cursor.Name}   
            /destroy 
            /delay 5 
            /if (${Window[ConfirmationDialogBox].Open}) {
                /delay 10
                /nomodkey /notify ConfirmationDialogBox Yes_Button leftmouseup
            }
            /delay 1s !${Cursor.ID}
        }
    }
    /if (${Debugloot}) /echo DEBUGLOOT Leave Sub HandleItem
/return

Sub CheckGlobalList(GItemName,int GListNum, LootSystem)
/declare i int local
/declare j int local
/declare GItem string local
    | Create global list from ini file 
    /if (${GlobalLootOn} && ${GlobalList[1].Equal[empty]}) {
        /for i 1 to ${${GlobalCount}}
            /varset GItem ${Ini[${NinjaIniFile},"Global","Global${i}"]}
            /if (!${GItem.Find[item name]} && !${GItem}) {
                /varset GlobalList[${i}] ${GItem}
            }
        /next i
    }
    | Check if item is in global list /echo ${GCheck.Arg[1,|]} ${GCheck.Arg[2,|]}
    /for j 1 to ${${GlobalCount}}
        /if (${GlobalList[${i}].Arg[1,|].Find[${GItemName}]}) {
            /if (${LootSystem.Equal[SList}}) {
                /if (${GlobalList[${i}].Arg[2,|].Equal[Ignore]}) /advloot shared ${GListNum} leave
                /if (${Select[${GlobalList[${i}].Arg[2,|]},sell,keep,destroy]} ) {
                    /advloot shared ${GListNum} giveto ${Me.Name}
                    /delay 10
                }
            }    
            /if (${LootSystem.Equal[PList}}) {
                /if (${GlobalList[${i}].Arg[2,|].Equal[Ignore]}) /advloot shared ${GListNum} leave
                /if (${Select[${GlobalList[${i}].Arg[2,|]},sell,keep,destroy]} ) {
                    /advloot personal ${GListNum} loot
                    /delay 10
                    /if (${Window[ConfirmationDialogBox].Open}) {
                        /nomodkey /notify ConfirmationDialogBox Yes_Button leftmouseup
                        /delay 10
                    }
                    /if (${Select[${IniEntry},Destroy]}) {
                        /call HandleItem "${GItemName}" Destroy
                    }
                }
            }           
        }
    /next j
/return