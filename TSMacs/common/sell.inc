| 
| sell.inc
| Selling Include File needed for macros
| Version 1.3
| Date: August 7, 2004
|
| 17 Mar 2006: Further refined by A_Druid_00
|
| 14 MAR 2006: Armysoldier refined code to improve speed
|
| Please be sure to get the lastest updates at http://www.soc-music.com/mq2/
|
| These file MUST be in the common directory in your macros directory.
|
| Disclaimer:
| Code was used from other programers also from the MQ Forums.
| 


Sub Sell(string ItemToSell)

   /declare OldMoney int Inner    
 
   /call ClearCursor

   /if (${ItemToSell.Equal[NULL]})  /return

   :Finding 
      /if (${FindItemCount[=${ItemToSell}]}) { 
      /nomodkey /itemnotify ${FindItem[=${ItemToSell}].InvSlot} leftmouseup 
      /if (${SelectedItem.ID}==${FindItem[=${ItemToSell}].ID}) {
      /delay 1 
      /varset OldMoney ${Me.Cash} 
      /nomodkey /shift /notify MerchantWnd MW_Sell_Button leftmouseup 
      /delay 2s (${Me.Cash}!=${OldMoney})
      }

      /goto :Finding 
      }


      /echo No more ${ItemToSell} to sell!!!      
/return 