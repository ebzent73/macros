| ldon_req.inc
|
| Requests a group adventure at any LDON wayfarer camp. Must be in vicinity of camp
| for this to navigate properly to the quest NPC. Delays have been placed to
| account for lag.
| 
| Usage: /call ReqAdv Risk Type
|
| Risks: 1 - Normal (default)
|        2 - High
|
| Types: 1 - Random (default)
|        2 - Single Boss
|        3 - Mob count
|        4 - Collection
|        5 - Rescue

Sub ReqAdv
{
  /declare AdvRisk int 1
  /declare AdvType int 1

  /echo ReqAdv started:

  /if (${Defined[Param0]}) /varset AdvRisk ${Param0}
  /if (${Defined[Param1]}) /varset AdvType ${Param1}

  /echo Selected Risk: ${AdvRisk}
  /echo Selected Type: ${AdvType}

  /target clear
  /delay 5

  /if (${Zone.ShortName.Equal[Everfrost]}) /target Mannis McGuyett
  /if (${Zone.ShortName.Equal[Butcher]}) /target Xyzelauna Tu`Valzir
  /if (${Zone.ShortName.Equal[Sro]}) /target Kallei Ribblok
  /if (${Zone.ShortName.Equal[Nro]}) /target Escon Quickbow
  /if (${Zone.ShortName.Equal[Ecommons]}) /target Periac Windfell
  /delay 5

  /call MoveToNPC
  /face

  /click right target
  /delay 1s

  /notify AdventureRequestWnd AdvRqst_RiskCombobox listselect ${AdvRisk}
  /delay 5
  /notify AdventureRequestWnd AdvRqst_TypeCombobox listselect ${AdvType}
  /delay 5
  /notify AdventureRequestWnd AdvRqst_RequestButton leftmouseup
  /delay 1s
  /notify AdventureReqeustWnd AdvRqst_AcceptButton leftmouseup

  /echo ReqAdv complete.

/return
}

Sub MoveToNPC
{
  :StartMove
  /face nolook loc ${Target.Y},${Target.X}
  /if (${Math.Distance[${Target.Y},${Target.X}]}>10) {
    /nomodkey /keypress forward hold
    /face nolook loc ${Target.Y},${Target.X}
  }
  /if (${Math.Distance[${Target.Y},${Target.X}]}<=10) {
    /nomodkey /keypress forward
    /goto :FinishedMove
  }
  /goto :StartMove
  :FinishedMove
/return
}