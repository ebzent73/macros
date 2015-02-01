| Ninjadvloot.inc v2.1  02/14/2011
| Author A_Druid_00 12/19/2005
| Updated: Moeymoejoe added strip out comma in item names when accessing ini file 08/02/2006
| Updated: mystikule added sell option to ini file 01/07/2008
| Updated: drkrain for House of Thule 10/23/2010
| Updated: Maskoi with Sell Routines 11/27/2010
| Selling Contributions by
|   drkrain - TLInv item check
|   dewey2461 - Add sold items to ini file Event
|
| Requirements: MQ2Moveutils
| Usage: add the following to your macro for looting
|    #include Ninjadvloot.inc
|   /call SetupAdvLootVars
|   /call LootMobs
| Sell Macro example - Cut and paste code between |** **|
|**
| Sellstuff.mac v1.1 by Maskoi 11/27/2010
| Target the vendor you want to sell to and type /mac sellstuff
| Required  Ninjadvloot.inc v2.1 and a loot.ini file with items marked as sell
|#include Ninjadvloot.inc
|Sub Main
|   /call SetupAdvLootVars
|   /call DoLootStuff sell
|/return
**|
| Guild Bank Deposit Macro example - Cut and paste code between |** **|
|**
| Depositstuff.mac v1.0 by Maskoi 11/27/2010
| Target the guild banker and type /mac depositstuff
| Required  Ninjadvloot.inc v2.1 and a loot.ini file with items marked as keep
#include Ninjadvloot.inc
Sub Main
   /call SetupAdvLootVars
   /call DoLootStuff deposit
/return
**|

#Event ALCantLoot       "#*#may not loot this corpse#*#"
#Event Sell          "#*#You receive#*# for the #1#(s)#*#""
#Event EditIniItem     "[MQ2] LootIniItem #1# #2#"

Sub SetupAdvLootVars

   /declare ALDontLoot      int    outer
   /declare CorpseRotTimer  timer  outer
   /call ALLoadVar Settings Version       2.1 int
   /call ALLoadVar Settings LootMobs      TRUE  bool
   /call ALLoadVar Settings CorpseRadius  200   int
   /call ALLoadVar Settings MobsTooClose  10    int
   /call ALLoadVar Settings CorpseRotTime 440s  string
   /call ALLoadVar Settings ReportLoot    FALSE bool
   /call ALLoadVar Settings LootChannel   g  string
   | Sell loot settings
   /call ALLoadVar Settings OpenAllBags   1  int
   /call ALLoadVar Settings AddNewSales   1  int
   
   /if (!${Defined[${AlertList}]}) /declare AlertList int outer 1
   /squelch /alert clear 25
   
   /squelch /alias /lootignore /ignore
   
   
   
/return

Sub ALLoadVar(IniSection,IniVar,IniValue,VarType)

   /if (!${Defined[${IniVar}]} && ${Defined[VarType]}) /declare ${IniVar} ${VarType} outer
   /declare IniString string local ${Ini[Loot.ini,${IniSection},${IniVar},NOTFOUND]}
   /varset ${IniVar} ${IniString}
   /if (${IniString.Equal["NOTFOUND"]}) {
      /if (${IniString.Equal["NOTFOUND"]}) /varset ${IniVar} ${IniValue}
      /ini "Loot.ini" "${IniSection}" "${IniVar}" "${${IniVar}}"
   }
   
/return

| ****************  Loot Mobs Section ******************** |

Sub Event_ALCantLoot
   /varset ALDontLoot ${Target.ID}
/return

Sub LootMobs

   /if (!${LootMobs} || ${SpawnCount[npc radius ${MobsTooClose} zradius 10 noalert ${AlertList}]} || !${SpawnCount[corpse radius ${CorpseRadius} zradius 100 noalert 25]} || ${Me.Combat} || (${Cursor.NoDrop} && !${Me.FreeInventory})) /return
   /if (!${CorpseRotTimer}) /squelch /alert clear 25
   /declare mhjtfi int local
   /declare CorpseList string local |
   /for mhjtfi 1 to ${SpawnCount[corpse radius ${CorpseRadius} zradius 100 noalert 25]}
      /varset CorpseList ${CorpseList}${NearestSpawn[${mhjtfi},corpse radius ${CorpseRadius} noalert 25].ID}|
   /next mhjtfi
   /declare DeadCount int local ${SpawnCount[corpse radius ${CorpseRadius} zradius 100 noalert 25]}
   /if (${Me.Mount.ID}) /dismount
   /for mhjtfi 1 to ${DeadCount}
      /if (${Spawn[${CorpseList.Arg[${mhjtfi},|]}].Deity.ID} && ${Spawn[${CorpseList.Arg[${mhjtfi},|]}].ID}) /squelch /alert add 25 id ${Spawn[${CorpseList.Arg[${mhjtfi},|]}].Deity.ID}
      /if (${Spawn[${CorpseList.Arg[${mhjtfi},|]}].ID} && ${Spawn[${CorpseList.Arg[${mhjtfi},|]}].LineOfSight} && !${Spawn[${CorpseList.Arg[${mhjtfi},|]}].Deity.ID}) {
         /if (${Target.ID}!=${Spawn[${CorpseList.Arg[${mhjtfi},|]}].ID}) /target id ${Spawn[${CorpseList.Arg[${mhjtfi},|]}].ID}
         /if (!${Me.Standing}) /stand
         /delay 2s ${Target.ID}==${Spawn[${CorpseList.Arg[${mhjtfi},|]}].ID} && ${Me.Standing}
         /if (${Spawn[${CorpseList.Arg[${mhjtfi},|]}].Distance}>5) {
         /echo /moveto loc ${Spawn[${CorpseList.Arg[${mhjtfi},|]}].Y} ${Spawn[${CorpseList.Arg[${mhjtfi},|]}].X}
         /moveto loc ${Spawn[${CorpseList.Arg[${mhjtfi},|]}].Y} ${Spawn[${CorpseList.Arg[${mhjtfi},|]}].X}
         }
         /delay 10s ${Spawn[${CorpseList.Arg[${mhjtfi},|]}].Distance}<10
         /if (${Spawn[${CorpseList.Arg[${mhjtfi},|]}].Distance}<10) /call LootCorpse
         |/stick off
      }
   /next mhjtfi
   
/return

Sub LootCorpse

   /declare zdmry          int    local
   /declare LootList   string local
   /declare CurrentItem string local
   /call CheckCursor
   /loot
   /delay 3s ${Corpse.Open}
   /doevents ALCantLoot
   /if (${Target.ID}==${ALDontLoot} && ${Spawn[${ALDontLoot}].ID}) /squelch /alert add 25 id ${ALDontLoot}
   /delay 3s ${Corpse.Items} || ${Target.ID}==${ALDontLoot}
   /if (${Corpse.Open} && ${Corpse.Items}) {
      /declare loottotal  int    local
      :ALlootlag
         /varset loottotal ${Corpse.Items}
         /delay 1s ${loottotal}!=${Corpse.Items}
      /if (${loottotal}!=${Corpse.Items}) /goto :ALlootlag
      /for zdmry 1 to ${loottotal}
	      /varset CurrentItem ${If[${Corpse.Item[${zdmry}].Name.Find[,]},${Corpse.Item[${zdmry}].Name.Arg[1,,]}${Corpse.Item[${zdmry}].Name.Arg[2,,]},${Corpse.Item[${zdmry}].Name}]}      
	      /if (${Corpse.Item[${zdmry}].ID} && !${Select[${Ini[Loot.ini,"${Corpse.Item[${zdmry}].Name.Left[1]}","${Corpse.Item[${zdmry}]}"]},Ignore,Keep,Destroy,Sell]}) {
	         /if (${Corpse.Item[${zdmry}].NoDrop}) /ini "loot.ini" "${Corpse.Item[${zdmry}].Name.Left[1]}" "${Corpse.Item[${zdmry}]}" Ignore
	         /if (!${Corpse.Item[${zdmry}].NoDrop}) /ini "loot.ini" "${Corpse.Item[${zdmry}].Name.Left[1]}" "${Corpse.Item[${zdmry}]}" Keep
	      }
	      /if (${Ini[Loot.ini,"${Corpse.Item[${zdmry}].Name.Left[1]}","${Corpse.Item[${zdmry}]}"].NotEqual[Destroy]} && !${Me.FreeInventory} && (!${FindItemCount[=${Corpse.Item[${zdmry}].Name}]} || (${FindItemCount[=${Corpse.Item[${zdmry}].Name}]} && ${Corpse.Item[${zdmry}].Stackable} && !${Corpse.Item[${zdmry}].FreeStack})) || (${Corpse.Item[${zdmry}].Lore} && ${FindItem[${Corpse.Item[${zdmry}]}].ID}) || ${Ini[Loot.ini,"${Corpse.Item[${zdmry}].Name.Left[1]}","${Corpse.Item[${zdmry}]}"].Equal[Ignore]}) /varset LootList ${LootList}${Corpse.Item[${zdmry}]},
	      /if ((${Ini[Loot.ini,"${Corpse.Item[${zdmry}].Name.Left[1]}","${Corpse.Item[${zdmry}]}"].Equal[Keep]} || ${Ini[Loot.ini,"${Corpse.Item[${zdmry}].Name.Left[1]}","${Corpse.Item[${zdmry}]}"].Equal[Sell]}) && (!${Corpse.Item[${zdmry}].Lore} || !${FindItem[${Corpse.Item[${zdmry}]}].ID}) && (${Me.FreeInventory} || (${FindItemCount[=${Corpse.Item[${zdmry}].Name}]} && ${Corpse.Item[${zdmry}].Stackable} && ${Corpse.Item[${zdmry}].FreeStack}))) /call LootItem ${zdmry} Keep Right
	      /if (${Ini[Loot.ini,"${Corpse.Item[${zdmry}].Name.Left[1]}","${Corpse.Item[${zdmry}]}"].Equal[Destroy]}) /call LootItem ${zdmry} Destroy left
      /next zdmry
      /if (${Corpse.Items}) {
         /if (${ReportLoot} && ${LootList.Length}) {
            /notify LootWND LW_BroadcastButton leftmouseup
            /keypress enter chat
         }
         /if (${Target.ID}) /squelch /alert add 25 id ${Target.ID}
         /varset CorpseRotTimer ${CorpseRotTime}
      }
   }
   :clickdoneffs
      /nomodkey /notify LootWnd LW_DoneButton leftmouseup
      /delay 5s !${Corpse.Open}
   /if (${Corpse.Open}) /goto :clickdoneffs
   
/return

Sub LootItem(int i,DoWhat,WhichButton)

   /declare CorpseItemID int local ${Corpse.Item[${i}].ID}
   /nomodkey /shift /itemnotify  ${Corpse.Item[${i}].InvSlot} ${WhichButton}mouseup
   /delay 5s ${Window[ConfirmationDialogBox].Open} || !${Corpse.Item[${i}].NoDrop}
   /if (${Window[ConfirmationDialogBox].Open}) /nomodkey /notify ConfirmationDialogBox Yes_Button leftmouseup
   /delay 5s ${Cursor.ID} || ${WhichButton.NotEqual[left]}
   /if (${DoWhat.Equal[Destroy]} && ${Cursor.ID}==${CorpseItemID}) /destroy
   /delay 3s !${Corpse.Item[${i}].ID}
   /if (${Cursor.ID}) /call CheckCursor
   
/return

Sub CheckCursor

   :ALauto_inv
   /if (${Cursor.ID}) {
      /if (${Cursor.NoDrop} && !${Me.FreeInventory}) /return
      /timed 5 /autoinventory
      /delay 2s !${Cursor.ID}
      /goto :ALauto_inv
   }
   
/return

| **************** Seach Loot Section ******************** |

Sub DoLootStuff(LootAction)
   /declare i int local
   /declare j int local
   /declare k int local
   /declare LootItem string
   /if (${OpenAllBags}) {
      /echo Opening all bags
      /keypress OPEN_INV_BAGS
   }
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
               /if (${Ini[Loot.ini,"${LootItem.Left[1]}","${LootItem}"].Equal[Sell]}) /call SellToVendor "${LootItem}"
            }
            /if (${LootAction.Equal[deposit]})  {
               /if (${Ini[Loot.ini,"${LootItem.Left[1]}","${LootItem}"].Equal[Keep]}) /call DepositToGuildBank "${LootItem}"
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
               /if (${Ini[Loot.ini,"${LootItem.Left[1]}","${LootItem}"].Equal[Sell]}) /call SellToVendor "${LootItem}"
            }
            /if (${LootAction.Equal[deposit]})  {
               /if (${Ini[Loot.ini,"${LootItem.Left[1]}","${LootItem}"].Equal[Keep]}) /call DepositToGuildBank "${LootItem}"
            }
         }
      /next j
   /next i

   /if (${OpenAllBags}) {
      /echo Closing all bags
      /keypress CLOSE_INV_BAGS
   }
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
   
Sub NPC(NPCName)
   /if (${Target.Type.NotEqual[NPC]} || !${Target.ID}) {
      /echo Please target a vendor or guildbanker
      /return
   }
   /echo Doing business with ${NPCName}
   /target npc ${NPCName}
   /delay 1S
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
      /ini "loot.ini" "${ItemName.Left[1]}" "${ItemName}" Sell
      }
/return

Sub SellToVendor(ItemToSell)
   /if (${Window[MerchantWnd].Open}) {
      :sell
         /echo Selling ${ItemToSell}
         /nomodkey /itemnotify ${FindItem[=${ItemToSell}].InvSlot} leftmouseup
         /delay 5
         /nomodkey /shiftkey /notify merchantwnd MW_Sell_Button leftmouseup
         /delay 2s
      /if (${FindItem[${itemToSell}].InvSlot}) /goto :sell
   }
/return

| **************** Deposit Loot Section ******************** | /notify GuildBankWnd 0 leftmouseup

Sub DepositToGuildBank(ItemToDeposit)
   /if (${Window[GuildBankWnd].Open}) {
      :deposit
         /if (${Window[GuildBankWnd].Child[GBANK_DepositCountLabel].Text.Arg[2,:]}==0) {
            /echo Guildbank Deposited Items is full.
            /return
         }
         /echo Depositing ${ItemToDeposit}
         /nomodkey /shiftkey /itemnotify ${FindItem[=${ItemToDeposit}].InvSlot} leftmouseup
         /delay 5
         /nomodkey /notify GuildBankWnd GBANK_DepositButton leftmouseup
         /delay 2s
      /if (${FindItem[${itemToSell}].InvSlot}) /goto :deposit
   }
/return