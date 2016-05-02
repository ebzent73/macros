| 
| cleanpacks.inc
| cleanpacks Include File needed for macros
| Version 1.1
| Date: August 7, 2004
|
| MAR 26 2006 : Modified some code to speed up the process (TKS A_Druid_00)
|
| MARCH 06 2006: Added /call ClearCursor to sub
|
| JAN 15 2006: Fixed String issues
|
| Please be sure to get the lastest updates at http://www.soc-music.com/mq2/
|
| These file MUST be in the common directory in your macros directory.
|
| Disclaimer:
| Code was used from other programers also from the MQ Forums.
| 


sub cleanPack(string PackName)
    /echo Please Wait ...... Cleaning out ${PackName} container
    /declare iSlot int local    
    /declare iCount int local    
    /declare PackNumber string local
    
    /call ClearCursor

    /if (${PackName.Equal[Enviro]}) { 
        /varset PackNumber e 
    } else { 
        /varset PackNumber ${FindItem[=${PackName}].InvSlot.ID} 
        /if (${PackNumber}) { 
            /varcalc PackNumber ${PackNumber}-21 
            /varset PackNumber ${PackNumber.Arg[0,.]} 
        } else { 
            /echo Could not find container in your inventory
            /echo Contanier name = ${PackName} 
            /endmacro
        } 
    }
    /if (${PackNumber.Equal[e]}) { 
        /for iSlot 1 to 10 
            /nomodkey /shiftkey /itemnotify enviro${iSlot} leftmouseup
            /delay 1s
	    /autoinv
        /next iSlot 
    
    } else { 
        
        /if (${Me.Inventory[Pack${PackNumber}].Container}==0) { 
            /return 
        } 

        /for iSlot 1 to ${Me.Inventory[Pack${PackNumber}].Container} 
            | Quit if the pack is empty 
            /if (${Me.Inventory[Pack${PackNumber}].Container}==0) { 
                /return 
            } 

            /if (${Me.Inventory[Pack${PackNumber}].Item[${iSlot}].ID}!=NULL) { 
                /varset iCount ${Me.Inventory[Pack${PackNumber}].Items} 
                /nomodkey /shiftkey /itemnotify in pack${PackNumber} ${iSlot} leftmouseup
    /delay 5s ${Cursor.ID} 
    /timed 5 /autoinv 
    /delay 5s !${Cursor.ID}
                /if (${Me.Inventory[Pack${PackNumber}].Items}==${iCount}) { 
                    /echo Unable to empty container. 
                    /endmacro 
                } 
            } 
        /next iSlot 
    } 
    /echo Done Cleaning ${PackName} Container. Thank you for waiting!
/return
