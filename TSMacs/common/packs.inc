| 
| packs.inc
| Open and Close Packs Include File needed for macros
| Version 1.4
| Date: August 7, 2004
|
|
| MAR 26 2006 : Modified some code to speed up the process (TKS A_Druid_00)
|
| Please be sure to get the lastest updates at http://www.soc-music.com/mq2/
|
| These file MUST be in the common directory in your macros directory.
|
| Disclaimer:
| Code was used from other programers also from the MQ Forums.
| 

Sub OpenPacks 
   /declare bag int local 0 
   /if (!${Window[InventoryWindow].Open}) /nomodkey /keypress inventory 
   /delay 3s ${Window[InventoryWindow].Open}  
   /for bag 1 to 8 
   /if (!${Window[Pack${bag}].Open}) /nomodkey /itemnotify pack${bag} rightmouseup 
   /delay 3s ${Window[Pack${bag}].Open} 
   /if (${Window[TradeskillWnd].Open}) /nomodkey /notify TradeskillWnd ExperimentButton leftmouseup 
   /delay 5s !${Window[TradeskillWnd].Open} 
   /next bag 
/return 

Sub ClosePacks 
   /declare bag int local 0 
   /if (!${Window[InventoryWindow].Open}) /nomodkey /keypress inventory 
   /delay 3s ${Window[InventoryWindow].Open}
   /for bag 1 to 8 
      /if (${Window[Pack${bag}].Open}) /nomodkey /itemnotify pack${bag} rightmouseup 
   /delay 3s !${Window[Pack${bag}].Open}
   /next bag 
   /if (${Window[InventoryWindow].Open}) /nomodkey /keypress inventory 
   /delay 3s !${Window[InventoryWindow].Open} 
/return 
