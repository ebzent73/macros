|
| buy.inc
| Buy Include File needed for macros
| Version 1.7b
|
| Date: August 20, 2005
| UPDATED BY OUNVME
|
| 14 MAR 2006: Armysoldier refined code to improve speed
|
| These file MUST be in the common directory in your macros directory.
|
| Usage: /call Buy (ItemToBuy) (Amount Needed)
|
| Use the Buy function for buying if you need a set total amount, like if you have
| 20 of the item on you already and you need a total of 30, it will only buy 10
| of the item.
|
| 20 June 2012: Magicdragon removed /buyitem command and changed to check
| stacksize so you don't buy 100 of an item you when you only wanted 20.
|
| Disclaimer:
| Code was used from other programers also from the MQ Forums.
|




#event Broke1 "#*#You cannot afford#*#"
#event Broke2 "#*#You can't afford#*#"
#event BuyFullInv "#*#Your inventory appears full! How can you buy more?#*#"

Sub Buy(Item ItemToBuy, int amount)
   |/echo Buying ${ItemToBuy}!
   /declare QTY int Inner
   /declare l2 int local
   /declare LastItemCount int local
   /declare CurrentItemCount int local
   /varset LastItemCount ${Window[MerchantWnd].Child[ItemList].Items}
   |---SMALL delay to allow the MERCHANT Window to populate
   /delay ${Math.Calc[${DelayMult}*2]}s
   /varset QTY ${Math.Calc[${amount}-${FindItemCount[=${ItemToBuy}]}]}
   :Filling
   /delay 1s
   /varset CurrentItemCount ${Window[MerchantWnd].Child[ItemList].Items}
   /if (${CurrentItemCount}!=${LastItemCount}) {
      /varset LastItemCount ${Window[MerchantWnd].Child[ItemList].Items}
      /goto :Filling
   }   
   /if (${FindItemCount[=${ItemToBuy}]}>= ${amount})  /return 
   /varset l2 ${Window[MerchantWnd].Child[ItemList].List[=${ItemToBuy},2]}
   /if (!${l2}) {
      /if (${Window[MerchantWnd].Child[MW_UsableButton].Checked}) {
         /notify MerchantWnd MW_UsableButton leftmouseup
         /goto :Filling
         }
      /echo Couldn't find ${ItemToBuy}
      /end
   } else {
      /nomodkey /notify MerchantWnd ItemList listselect ${l2}
      /notify MerchantWnd ItemList leftmouse ${l2}
      /delay 2         
   } 
   /echo Buying ${ItemToBuy} until I get ${amount} 
   :Loop
   /if (${QTY}>=${Merchant.Item[=${ItemToBuy}].StackSize}) {
       /nomodkey /shift /notify MerchantWnd MW_Buy_Button leftmouseup
      /delay 1s
      /doevents
      /varset QTY ${Math.Calc[${amount}-${FindItemCount[=${ItemToBuy}]}]}
      /delay 1s
      /goto :Loop
   } else {
      /if (${QTY}>0 && ${QTY}<${Merchant.Item[=${ItemToBuy}].StackSize}) {
         /declare char string local
         /declare InStr string local
         /declare loopctr int local
       /notify MerchantWnd MW_Buy_Button leftmouseup
         /delay 1m ${Window[QuantityWnd].Open}
      /nomodkey /notify QuantityWnd QTYW_SliderInput leftmouseup
      /delay 1s
         /nomodkey /keypress right chat
         /nomodkey /keypress right chat
         /nomodkey /keypress right chat
         /nomodkey /keypress backspace chat
         /nomodkey /keypress backspace chat
         /nomodkey /keypress backspace chat
         /varset InStr ${QTY}
         /for loopctr 1 to ${InStr.Length}
         /varset char ${InStr.Mid[${loopctr},1]}
            /if (!${char.Length}) {
               /nomodkey /keypress Space chat
            } else {
               /nomodkey /keypress ${char} chat
            }
         /next loopctr
         /nomodkey /notify QuantityWnd QTYW_Accept_Button leftmouseup
         /delay 5s !${Window[QuantityWnd].Open}
         /doevents
         /delay 2s
         }
      }
   /varset QTY ${Math.Calc[${amount}-${FindItemCount[=${ItemToBuy}]}]}
   /if (${QTY}<=0) /return 
   /goto :Loop
   /return
 

Sub Event_Broke1
   /echo You are out of money!
   /beep
   /end
   /return

Sub Event_Broke2
   /echo You are out of money!
   /beep
   /end
   /return

Sub Event_BuyFullInv
   /echo Your inventory is full!
   /beep
   /end
   /return