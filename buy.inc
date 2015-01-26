|
| buy.inc
| Buy Include File needed for macros
| Version 1.3
| Date: August 1, 2004
|
| Please be sure to get the lastest updates at http://www.soc-music.com/mq2/index.htm
|
| These file MUST be in the common directory in your macros directory.
|
| Disclaimer:
| Code was used from other programers also from the MQ Forums.
|


#event Broke "#*#can't afford to buy them#*#"

Sub Buy(item ItemToBuy, int amount)

/echo Buying ${ItemToBuy}

/declare TotalMItems int Inner
/varset TotalMItems ${Merchant.Items}
/declare x int Inner
/declare ItemSlotNum int Inner
/varset ItemSlotNum 0

/for x 1 to ${TotalMItems}
  /if (${ItemToBuy.Equal[${Merchant.Item[${x}]}]}) {
    /varset ItemSlotNum ${x}
  }
/next x

/if (${ItemSlotNum}==0) {
  /echo Not on Merchant
  /return
}

/itemnotify merchant${ItemSlotNum} leftmouseup


:Buy
   /if (${FindItemCount[=${SelectedItem}]}>=${amount}) {
     /echo Done!
     /return
   }       

:StackLoop
   /if (${Math.Calc[${amount}-${FindItemCount[=${SelectedItem}]}]}<=20) /goto :RegLoop
   /buyitem ${If[${SelectedItem.Stackable},20,1]}
   /delay 5
   /doevents
   /goto :StackLoop

:RegLoop
   /if (${Math.Calc[${amount}-${FindItemCount[=${SelectedItem}]}]}<=0) {
     /echo Done!
     /return
   }       
   /buyitem ${If[${SelectedItem.Stackable},${Math.Calc[${amount}-${FindItemCount[=${SelectedItem}]}]},1]}
   /delay 5
   /doevents
   /goto :RegLoop

/return

Sub Event_Broke
   /echo Error: You are out of money!
   /beep
   /endmacro
/return 