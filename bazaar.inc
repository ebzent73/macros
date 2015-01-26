|
| bazaar.inc -- helper routines for bazaar searching
| v2.1 -- 05/07/07 -- fixed a bug for items with parens in the name
| v2.0 -- 05/05/07 -- better price support
|
| Everything is now based off the item list in the bazaar
| search window, so we are limited to 100 items to mess
| around with (and not the 200 returned from the server).
|
| PriceToIntPrice(string p) -- convert "1p 2g 3s 4c" to 1234
| InspectItemFromListIndex(int n) -- does an inspect on the nth item
| GetQuantityFromListIndex(int n) -- get the number for sale of the nth item
| GetIntPriceFromListIndex(int n) -- get the price in coppers (note: not accurate)
| GetPriceFromListIndex(int n) -- get the displayed price
| GetItemIDFromListIndex(int n) -- get the item ID
| GetItemNameFromListIndex(int n) -- get the item name
| MakeQuery -- press the "Find Items" button
| ResetQuery -- press the "Reset" button
| ResetItemName -- backspace over the item name
| SetItemName(string ItemName) -- set the item name field
| SetTraderByName(string TraderName) -- set the trader name
| SetTraderByNumber(int n) -- set to the nth trader
| SetSlot(string slot) -- set the slot field
| SetStat(string stat) -- set the stat field
| SetRace(string racetoset) -- set the race field
| SetClass(string classtoset) -- set the class field
| SetType(string typetoset) -- set the type field
| SetMinLevel(string leveltoset)
| SetMaxLevel(string leveltoset)
| SetMinPrice(string pricetoset)
| SetMaxPrice(string pricetoset)

|||||||||||||||||||||||||||||||||||||||||||||

sub PriceToIntPrice(string p)
{
/declare plat int local 0
/declare gold int local 0
/declare silv int local 0
/declare copp int local 0
/if (${p.Find[p]}) {
    /varset plat ${p.Left[-${Math.Calc[${p.Length}-${p.Find[p]}+1]}]}
    /varset p ${p.Right[-${Math.Calc[${p.Find[p]}+1]}]}
}
/if (${p.Find[g]}) {
    /varset gold ${p.Left[-${Math.Calc[${p.Length}-${p.Find[g]}+1]}]}
    /varset p ${p.Right[-${Math.Calc[${p.Find[g]}+1]}]}
}
/if (${p.Find[s]}) {
    /varset silv ${p.Left[-${Math.Calc[${p.Length}-${p.Find[s]}+1]}]}
    /varset p ${p.Right[-${Math.Calc[${p.Find[s]}+1]}]}
}
/if (${p.Find[c]}) {
    /varset copp ${p.Left[-${Math.Calc[${p.Length}-${p.Find[c]}+1]}]}
}
| /echo price ${p} is ${Math.Calc[${plat}*1000 + ${gold}*100 + ${silv}*10 + ${copp}]}
/return ${Math.Calc[${plat}*1000 + ${gold}*100 + ${silv}*10 + ${copp}].Int}

}

|||||||||||||||||||||||||||||||||||||||||||||

sub InspectItemFromListIndex(int iifli_index)
/declare local_i int local
:reselect
/nomodkey /notify BazaarSearchWnd BZR_ItemList listselect ${iifli_index}
/delay 2
/nomodkey /notify BazaarSearchWnd BZR_ItemList leftmouse ${iifli_index}
/delay 2
/ireset
/delay  2s ${Window[BazaarSearchWnd].Child[BZR_RequestItemButton].Enabled}
/if (!${Window[BazaarSearchWnd].Child[BZR_RequestItemButton].Enabled}) {
    /nomodkey /notify BazaarSearchWnd BZR_UpdatePlayerButton leftmouseup
    /delay  1s ${Window[BazaarSearchWnd].Child[BZR_RequestItemButton].Enabled}
    /if (!${Window[BazaarSearchWnd].Child[BZR_RequestItemButton].Enabled}) /return -1
    /goto :reselect
}
/nomodkey /notify BazaarSearchWnd BZR_RequestItemButton leftmouseup
/delay 5s ${DisplayItem.ID}
/return 0

|||||||||||||||||||||||||||||||||||||||||||||

sub GetQuantityFromListIndex(int gqfli_index)
/declare local_i int local
/declare local_name string local
/varset local_name ${Window[BazaarSearchWnd].Child[BZR_ItemList].List[${gqfli_index},1]}
/varset local_name ${local_name.Token[2,(].Token[1,)]}
/return  ${local_name}

|||||||||||||||||||||||||||||||||||||||||||||

sub GetIntPriceFromListIndex(int gipfli_index)
/declare local_i int local
/declare local_name string local
/call GetItemNameFromListIndex ${gipfli_index}
/varset local_name ${Macro.Return}
/return  ${Bazaar.Item[${local_name}].Price}

|||||||||||||||||||||||||||||||||||||||||||||

sub GetPriceFromListIndex(int gpfli_index)
/return ${Window[BazaarSearchWnd].Child[BZR_ItemList].List[${gpfli_index},2]}

|||||||||||||||||||||||||||||||||||||||||||||

sub GetItemIDFromListIndex(int giifli_index)
/declare local_i int local
/declare local_name string local
/call GetItemNameFromListIndex ${giifli_index}
/varset local_name ${Macro.Return}
/return  ${Bazaar.Item[${local_name}].ItemID}

|||||||||||||||||||||||||||||||||||||||||||||

sub GetItemNameFromListIndex(int ginfli_index)

/declare tmp int local ${Window[BazaarSearchWnd].Child[BZR_ItemList].List[${ginfli_index}].Token[3,(].Length}
/if (${tmp}) {
/return ${Window[BazaarSearchWnd].Child[BZR_ItemList].List[${ginfli_index}].Left[${Math.Calc[-${tmp}-1]}]}
}
/return ${Window[BazaarSearchWnd].Child[BZR_ItemList].List[${ginfli_index},1].Token[1,(]}

|||||||||||||||||||||||||||||||||||||||||||||

sub MakeQuery
/delay  5s ${Window[BazaarSearchWnd].Child[BZR_QueryButton].Enabled}
/if (!${Window[BazaarSearchWnd].Child[BZR_QueryButton].Enabled}) {
    /echo BZR_QueryButton NOT enabled
    /return
}
/breset
/nomodkey /notify BazaarSearchWnd BZR_QueryButton leftmouseup
/delay  5s ${Bazaar.Done}
/return

|||||||||||||||||||||||||||||||||||||||||||||

sub ResetQuery
/notify BazaarSearchWnd BZR_Default leftmouseup
/delay 2
/return

|||||||||||||||||||||||||||||||||||||||||||||

sub ResetItemName
/declare local_i int local
/notify BazaarSearchWnd BZR_ItemNameInput leftmouse
/for local_i 1 to 50
/keypress backspace chat
/keypress delete chat
/next local_i
/return

|||||||||||||||||||||||||||||||||||||||||||||

sub SetItemName(ItemName)
/declare local_i int local

|/notify BazaarSearchWnd BZR_Default leftmouseup
/notify BazaarSearchWnd BZR_ItemNameInput leftmouse
|/keypress backspace chat

/for local_i 1 to ${ItemName.Length}
|/echo /keypress ${ItemName.Mid[${local_i},1]} chat
/if (${ItemName.Mid[${local_i},1].Equal[ ]}) {
/keypress space chat
} else {
/keypress ${ItemName.Mid[${local_i},1]} chat
}
/next local_i
/return


|||||||||||||||||||||||||||||||||||||||||||||

sub SetTraderByName(string TraderName)
/declare local_i int local

/for local_i 1 to ${Window[BazaarSearchWnd].Child[BZR_PlayersCombobox].Items}
|/echo ${Window[BazaarSearchWnd].Child[BZR_PlayersCombobox].List[${local_i},1]}
/if (${TraderName.Equal[${Window[BazaarSearchWnd].Child[BZR_PlayersCombobox].List[${local_i},1]}]}) {
    /notify BazaarSearchWnd BZR_PlayersCombobox listselect ${local_i}
    /return
}
/next local_i
/echo trader not found
/return

|||||||||||||||||||||||||||||||||||||||||||||

sub SetTraderByNumber(int T)

/notify BazaarSearchWnd BZR_PlayersCombobox listselect ${T}
/return

|||||||||||||||||||||||||||||||||||||||||||||

sub SetSlot(string slot)
/declare local_i int local

/for local_i 1 to ${Window[BazaarSearchWnd].Child[BZR_ItemSlotCombobox].Items}
|/echo ${Window[BazaarSearchWnd].Child[BZR_ItemSlotCombobox].List[${local_i},1]}
/if (${slot.Equal[${Window[BazaarSearchWnd].Child[BZR_ItemSlotCombobox].List[${local_i},1]}]}) {
    /notify BazaarSearchWnd BZR_ItemSlotCombobox listselect ${local_i}
    /return
}
/next local_i
/echo slot ${slot} not found
/return

|||||||||||||||||||||||||||||||||||||||||||||

sub SetStat(string stat)
/declare local_i int local

/for local_i 1 to ${Window[BazaarSearchWnd].Child[BZR_StatSlotCombobox].Items}
/if (${stat.Equal[${Window[BazaarSearchWnd].Child[BZR_StatSlotCombobox].List[${local_i},1]}]}) {
    /notify BazaarSearchWnd BZR_StatSlotCombobox listselect ${local_i}
    /return
}
/next local_i
/echo stat not found
/return

|||||||||||||||||||||||||||||||||||||||||||||

sub SetRace(string racetoset)
/declare local_i int local

/for local_i 1 to ${Window[BazaarSearchWnd].Child[BZR_RaceSlotCombobox].Items}
|/echo ${Window[BazaarSearchWnd].Child[BZR_RaceSlotCombobox].List[${local_i},1]}
/if (${racetoset.Equal[${Window[BazaarSearchWnd].Child[BZR_RaceSlotCombobox].List[${local_i},1]}]}) {
    /notify BazaarSearchWnd BZR_RaceSlotCombobox listselect ${local_i}
    /return
}
/next local_i
/echo race ${racetoset} not found
/return

|||||||||||||||||||||||||||||||||||||||||||||

sub SetClass(string classtoset)
/declare local_i int local

/for local_i 1 to ${Window[BazaarSearchWnd].Child[BZR_ClassSlotCombobox].Items}
/if (${classtoset.Equal[${Window[BazaarSearchWnd].Child[BZR_ClassSlotCombobox].List[${local_i},1]}]}) {
    /notify BazaarSearchWnd BZR_ClassSlotCombobox listselect ${local_i}
    /return
}
/next local_i
/echo class ${classtoset} not found
/return

|||||||||||||||||||||||||||||||||||||||||||||

sub SetType(string typetoset)
/declare local_i int local

/for local_i 1 to ${Window[BazaarSearchWnd].Child[BZR_ItemTypeCombobox].Items}
/if (${typetoset.Equal[${Window[BazaarSearchWnd].Child[BZR_ItemTypeCombobox].List[${local_i},1]}]}) {
    /notify BazaarSearchWnd BZR_ItemTypeCombobox listselect ${local_i}
    /return
}
/next local_i
/echo type ${typetoset} not found
/return

|||||||||||||||||||||||||||||||||||||||||||||

sub SetMinLevel(string leveltoset)
/declare local_i int local

/notify BazaarSearchWnd BZR_MinLevelInput leftmouse

/for local_i 1 to 10
/keypress backspace chat
/keypress delete chat
/next local_i

/for local_i 1 to ${leveltoset.Length}
/keypress ${leveltoset.Mid[${local_i},1]} chat
/next local_i
/return

|||||||||||||||||||||||||||||||||||||||||||||

sub SetMaxLevel(string leveltoset)
/declare local_i int local

/notify BazaarSearchWnd BZR_MaxLevelInput leftmouse

/for local_i 1 to 20
/keypress backspace chat
/keypress delete chat
/next local_i

/for local_i 1 to ${leveltoset.Length}
/keypress ${leveltoset.Mid[${local_i},1]} chat
/next local_i
/return

|||||||||||||||||||||||||||||||||||||||||||||

sub SetMinPrice(string pricetoset)
/declare local_i int local

/notify BazaarSearchWnd BZR_MinPriceInput leftmouse

/for local_i 1 to 10
/keypress backspace chat
/keypress delete chat
/next local_i

/for local_i 1 to ${pricetoset.Length}
/keypress ${pricetoset.Mid[${local_i},1]} chat
/next local_i
/return

|||||||||||||||||||||||||||||||||||||||||||||

sub SetMaxPrice(string pricetoset)
/declare local_i int local

/notify BazaarSearchWnd BZR_MaxPriceInput leftmouse

/for local_i 1 to 20
/keypress backspace chat
/keypress delete chat
/next local_i

/for local_i 1 to ${pricetoset.Length}
/keypress ${pricetoset.Mid[${local_i},1]} chat
/next local_i
/return

