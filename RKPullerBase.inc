Sub Init_RKPullerBase
	/if (${Group.Leader.ID} == ${Me.ID}) /call AddPullTargets
	/echo Done initializing puller base!
/return

Sub AddPullTargets
	
	/if (${Zone.ShortName.Equal[brotherisland]}) {
		/call AddToArray "PullTargets" "praetor"
		/call AddToArray "PullTargets" "razorlatch"
		/call AddToArray "PullTargets" "eyepecker"
		/call AddToArray "PullTargets" "squawker"
		/call AddToArray "PullTargets" "furthick"
		/call AddToArray "PullTargets" "pounder"
		/call AddToArray "PullTargets" "tasselvine"
		/call AddToArray "PullTargets" "mulcher"
		/call AddToArray "PullTargets" "wanderer"		
		/call AddToArray "PullTargets" "siren"		
		/call AddToArray "PullTargets" "pincer"
	}

	/if (${Zone.ShortName.Equal[deadhills_errand]}) {
		/call AddToArray "PullTargets" "water mephit"
		/call AddToArray "PullTargets" "scavenger"
		
		|Gribbles 1 - Into the Hills
		/call AddToArray "PullTargets" "undead"
		/call AddToArray "PullTargets" "pusling"		
		/call AddToArray "PullTargets" "bloodthirsty pirate"		
		/call AddToArray "PullTargets" "pile of bile"

		/call AddToArray "PullTargets" "tiny zombie"
		/call AddToArray "PullTargets" "tsetsian"
		/call AddToArray "PullTargets" "filthy"
		/call AddToArray "PullTargets" "hired guard"
		/call AddToArray "PullTargets" "mosquito"
		
		/call AddToArray "PullTargets" "filthy scavenger"
		
		|Gribbles 2 - Scouting Ahead
		/call AddToArray "PullTargets" "monstrosity"
		/call AddToArray "PullTargets" "scout"
		/call AddToArray "PullTargets" "zombie"		
		
		/call AddToArray "PullTargets" "vegerog"
		/call AddToArray "PullTargets" "excavator"			
		/call AddToArray "PullTargets" "xulous elder"	

		|Gribbles 3 - Disrupting the ritual
		/call AddToArray "PullTargets" "living ooze"	
		/call AddToArray "PullTargets" "xulous ritualist"
		/call AddToArray "PullTargets" "xulous warpriest"			
	}

	/if (${Zone.ShortName.Equal[ethernere_errand]}) {
		|/call AddToArray "PullTargets" "water mephit"
	}

	/if (${Zone.ShortName.Equal[arelis]}) { 
		/call AddToArray "PriorityTargets" "Scout VI"	
		/call AddToArray "PriorityTargets" "Scout VII"		
		/call AddToArray "PriorityTargets" "Scout IX"		
		/call AddToArray "PriorityTargets" "Scout XII"
		/call AddToArray "PriorityTargets" "Bloodborn"		
		/call AddToArray "PriorityTargets" "Defector"				

		
		/call AddToArray "PullTargets" "wicked grendlaen"	
		/call AddToArray "PullTargets" "ferocious grendlaen"				
		/call AddToArray "PullTargets" "savage grendlaen"						
		/call AddToArray "LoSPullTargets" "dirty oashim"				
		/call AddToArray "LoSPullTargets" "mature oashim"				
		/call AddToArray "LoSPullTargets" "adult oashim"					
		
		/call AddToArray "LoSPullTargets" "wicked"				
		/call AddToArray "LoSPullTargets" "savage"				
		/call AddToArray "LoSPullTargets" "ferocious"
		/call AddToArray "LoSPullTargets" "huntsman"				
	}

	/if (${Zone.ShortName.Equal[beastsdomain]}) {
		/call AddToArray "PriorityTargets" "Tangledeath"
		/call AddToArray "PriorityTargets" "Night"	
		/call AddToArray "PriorityTargets" "Twisted Strangler"	
		/call AddToArray "PriorityTargets" "Deathsquirm"		
		/call AddToArray "PriorityTargets" "goral striker"			

		/call AddToArray "PullTargets" "tainted ape"		
		/call AddToArray "PullTargets" "a naeya scavenger"		
		/call AddToArray "PullTargets" "a fierce wyvern"

		/call AddToArray "LoSPullTargets" "an okiina vinethrasher"		
		/call AddToArray "LoSPullTargets" "goral scavenger"					
		/call AddToArray "LoSPullTargets" "prowler"		
		/call AddToArray "LoSPullTargets" "braxi"		
		/call AddToArray "LoSPullTargets" "grendlaen"
	} 
	
	/if (${Zone.ShortName.Equal[cityofbronze]}) {
		/call AddToArray "LoSPullTargets" "Guggles"
		/call AddToArray "LoSPullTargets" "wild selyrah"		
		/call AddToArray "LoSPullTargets" "feral grendlaen"
		/call AddToArray "LoSPullTargets" "oashim"		
		/call AddToArray "LoSPullTargets" "strangleweed"		
		/call AddToArray "LoSPullTargets" "wild braxi"

		/call AddToArray "PullTargets" "Guggles"
		/call AddToArray "PullTargets" "wild selyrah"		
		/call AddToArray "PullTargets" "feral grendlaen"
		/call AddToArray "PullTargets" "oashim"		
		/call AddToArray "PullTargets" "strangleweed"		
		/call AddToArray "PullTargets" "wild braxi"
		
	}
	
	/if (${Zone.ShortName.Equal[ethernere]}) {
		|/call AddToArray "PriorityTargets" "bear"
		|/call AddToArray "PullTargets" "gehein"		
		|/call AddToArray "PullTargets" "orafik"				
		|/call AddToArray "LoSPullTargets" "skirth"
	}
	
	/if (${Zone.ShortName.Equal[windsong]}) {
		/call AddToArray "PullTargets" "apparition"
		/call AddToArray "PullTargets" "devotee"
		/call AddToArray "PullTargets" "spirit"
		/call AddToArray "PullTargets" "elemental"
		/call AddToArray "PullTargets" "glider"
		/call AddToArray "PullTargets" "muted"		
		/call AddToArray "PullTargets" "leaper"				
	}

	/if (${Zone.ShortName.Equal[shardslanding]}) {
		/call AddToArray "PullTargets" "slitherer"
		/call AddToArray "PullTargets" "worntooth"
		/call AddToArray "PullTargets" "verdantbeak"		
		/call AddToArray "PullTargets" "ivyscale"		
		/call AddToArray "PullTargets" "slatescale"		
		/call AddToArray "PullTargets" "venomshell"				
	}
	
/return FALSE
