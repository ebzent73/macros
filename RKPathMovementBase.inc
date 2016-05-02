Sub Init_RKPathMovementBase
	/declare PathNodes[1000,2] float outer | 1st dimension is index, 2nd dimension is only 2 deep - 1st member is X, 2nd is Y
	/declare PathName string outer "Default"
	/declare CurrentPathNodeIndex int outer 0

	/declare LoopPath bool outer TRUE
	/declare MaxAcceptableDistanceFromNode int outer 10

	/declare OnDetour bool outer FALSE
	/declare DetourNodes[100,2] float outer | same as path nodes, but used for divergence from the current path
	/declare CurrentDetourNodeIndex int outer 0	

	/call AddToArray "PreTargetAcquisitionCombatActionSubs" "StopFollowingPath"
	/call AddToArray "NonCombatSubs" "UpdateFollowPath"
/return


Sub BeginFollowPath(string pathName)
	/varset PathName "${pathName}"
	/call LoadPath "${pathName}"
	/call GetClosestNodeIndex "PathNodes"
	/varset CurrentPathNodeIndex ${Macro.Return}
	/if (${CurrentPathNodeIndex} == 0) {
		/echo No node path was found.  The most likely cause is not specifying a valid PathName when calling FollowPath "pathName".  
		/echo Create a path by calling ClearPath, then call AddCurrentLocationToPath at each desired point.
		/echo Finally, save the newly created path by calling SavePath "pathName"
		/call ClearPath
		/return FALSE
	}
/return

Sub BeginDetour
	/varset OnDetour TRUE
	/call ClearTwoDimensionalArray "DetourNodes" 2
	/call AddCurrentLocationToDetour
/return

Sub EndDetour
	/varset OnDetour FALSE
	/call ClearTwoDimensionalArray "DetourNodes" 2
/return

Sub UpdateFollowPath
	/if (${PathName.Equal["Default"]} || ${CurrentPathNodeIndex} == 0) /return

	/if (${OnDetour}) {
		/if (!${Me.Moving}) {
			/call GetDistanceToCurrentDetourNode
			/if (${Macro.Return} && ${Macro.Return} < ${MaxAcceptableDistanceFromNode}) {
				/call GetPreviousDetourNodeIndex
			}

			/if (${CurrentDetourNodeIndex} == 0) {
				/call EndDetour
				/return
			}
			/moveto loc ${DetourNodes[${CurrentDetourNodeIndex}, 1]} ${DetourNodes[${CurrentDetourNodeIndex}, 2]}
		}
	}

	/if (!${Me.Moving}) {
		/call GetDistanceToCurrentPathNode
		/if (${Macro.Return} && ${Macro.Return} < ${MaxAcceptableDistanceFromNode}) {
			/call GetNextPathNodeIndex
		}
		/moveto loc ${PathNodes[${CurrentPathNodeIndex}, 1]} ${PathNodes[${CurrentPathNodeIndex}, 2]}
	}

/return

Sub StopFollowingPath
	/moveto off
/return

Sub GetDistanceToCurrentPathNode
	/call GetDistanceToCurrentNode "PathNodes" "CurrentPathNodeIndex"
/return ${Macro.Return}

Sub GetDistanceToCurrentDetourNode
	/call GetDistanceToCurrentNode "DetourNodes" "CurrentDetourNodeIndex"
/return ${Macro.Return}

Sub GetDistanceToCurrentNode(string arrayName, string indexVariableName)
	/if (${${indexVariableName}} != 0) {
		/return ${Math.Distance[${${arrayName}[${${indexVariableName}},1]}, ${${arrayName}[${${indexVariableName}},2]}:${Me.X}, ${Me.Y}]}
	}
	/echo No current node was found.  The most likely cause is that the array "${arrayName}" was not populated correctly
/return


Sub GetClosestNodeIndex(string arrayName)
	/declare closestIndex int local 0
	/declare closestDistance float local 10000
	/declare closestX float local
	/declare closestY float local
	/declare currentNodeDistance float local
	/declare i int local

	/for i 1 to ${${arrayName}.Dimensions}
		/varset currentNodeDistance ${Math.Distance[${${arrayName}[${i},1]}, ${${arrayName}[${i},2]}:${Me.X}, ${Me.Y}]}

		/if (${currentNodeDistance} < ${closestDistance}) {
			/varset closestIndex ${i}
			/varset closestDistance ${currentNodeDistance}
		}
	/next i
/return ${closestIndex}

Sub GetNextPathNodeIndex
	/call GetNextNodeIndex "PathNodes" "CurrentPathNodeIndex" ${LoopPath}
/return

Sub GetPreviousDetourNodeIndex
	/call GetPreviousNodeIndex "DetourNodes" "CurrentDetourNodeIndex" FALSE
/return

Sub GetNextNodeIndex(string arrayName, string indexVariableName, bool doLoop)
	/varcalc ${indexVariableName} ${Math.Calc[${${indexVariableName}} + 1]}
	/if (${${indexVariableName}} > ${${arrayName}.Dimensions}) {
		/if (${${doLoop}}) {
			/varset ${indexVariableName} 1
		} else {
			/varset ${indexVariableName} 0
		}
	}
/return


Sub GetPreviousNodeIndex(string arrayName, string indexVariableName, bool doLoop)
	/varcalc ${indexVariableName} ${Math.Calc[${${indexVariableName}} - 1]}
	/if (${${indexVariableName}} <= 1) {
		/if (${${doLoop}}) {
			/varset ${indexVariableName} ${${arrayName}.Size}
		} else {
			/varset ${indexVariableName} 0
		}
	}
/return

Sub AddCurrentLocationToPath
	/call AddToNodeArray "PathNodes" ${Me.X} ${Me.Y}
/return

Sub AddToPathNodeArray(float valueOne, float valueTwo)
	/call AddToNodeArray "PathNodes" ${valueOne} ${valueTwo}
/return FALSE

Sub AddCurrentLocationToDetour
	/call AddToNodeArray "DetourNodes" ${Me.X} ${Me.Y}
	/varcalc CurrentDetourNodeIndex ${CurrentDetourNodeIndex} + 1
/return

Sub AddToNodeArray(string arrayName, float valueOne, float valueTwo)
	/declare i int local
	/for i 1 to ${${arrayName}.Size}
		/if (!${Bool[${${arrayName}[${i}, 1]}]}) {
			/echo Adding X = ${valueOne}, Y = ${valueTwo} to ${arrayName}
			/varset ${arrayName}[${i},1] ${valueOne}
			/varset ${arrayName}[${i},2] ${valueTwo}
			/return TRUE
		}
	/next i
/return FALSE

Sub ClearPath
	/call ClearTwoDimensionalArray "PathNodes" 2
/return

Sub SavePath(string pathName)
	/ini "${pathName}.ini" "Nodes" "Count" ${PathNodes.Dimensions}
	/declare i int local
	/for i 1 to ${PathNodes.Dimensions}
		/ini "${pathName}.ini" "Node${i}" "X" ${PathNodes[${i},1]}
		/ini "${pathName}.ini" "Node${i}" "Y" ${PathNodes[${i},2]}
	/next i
/return TRUE


Sub LoadPath(string pathName)
	/call ClearPath
	/declare nodeCount int local ${Ini[${pathName}.ini,Nodes,Count]}
	/declare i int local
	/declare x float local
	/declare y float local
	/for i 1 to ${nodeCount}
		/call AddToPathNodeArray ${Ini[${pathName}.ini,Node${i},X]} ${Ini[${pathName}.ini,Node${i},Y]}
	/next i
/return TRUE


|********************************************** BASE CLASS SUBS **********************************************


Sub ClearTwoDimensionalArray(string arrayName, int secondDimensionDepth)
	/declare i int local 1
	/declare j int local 1

	/for i 1 to ${${arrayName}.Size}
		/for j 1 to ${secondDimensionDepth}
			/varset ${arrayName}[${i}, ${j}]
		/next j
	/next i
/return

Sub ClearArray(string arrayName)
	/declare i int local 1
	/for i 1 to ${${arrayName}.Size}
		/varset ${arrayName}[${i}]
	/next i
/return

Sub PushToStack(string stackName, string value)
	/call ShiftArrayRight "${stackName}"
	/varset ${stackName}[1] "${value}"
/return

Sub PopFromStack(string stackName)
	/declare value string local ${${stackName}[1]}
	/call ShiftArrayLeft "${stackName}"
/return ${value}

Sub AddToQueue(string queueName, string value)
	/call CompressArray "${queueName}"
	/call AddToArray "${queueName}" "${value}"
/return

Sub RemoveFromQueue(string queueName)
	/declare value string local "${${queueName}[1]}"
	/call ShiftArrayLeft "${queueName}"
/return "${value}"

Sub ShiftArrayRight(string arrayName)
	/declare i int local
	/for i ${Math.Calc[${${arrayName}.Size} - 1].Int} downto 1
   		/if (${Bool[${${arrayName}[${i}]}]}) {
			/varset ${arrayName}[${Math.Calc[${i}+1].Int}] ${${arrayName}[${i}]}
		}
	/next i
/return

Sub ShiftArrayLeft(string arrayName)
	/call ShiftArrayLeft "${arrayName}" 2 FALSE
/return ${Macro.Return}

Sub ShiftArrayLeft(string arrayName, int startingPoint, bool compressNulls)
	/declare i int local
	/declare shiftDistance int local 1 
	/declare itemsEncountered bool local FALSE
	/for i ${startingPoint} to ${${arrayName}.Size}
   		/if (${Bool[${${arrayName}[${i}]}]}) {
			/varset itemsEncountered TRUE
			/varset ${arrayName}[${Math.Calc[${i} - ${shiftDistance}].Int}] ${${arrayName}[${i}]}
		} else {
			/if (${compressNulls}) /varcalc shiftDistance ${shiftDistance} + 1
		}
	/next i
/return ${itemsEncountered}

Sub CompressArray(string arrayName) | Removes all nulls and starts index with item 1 being populated (array is 1 based, not 0)
	/declare i int local
	/for i 1 to ${${arrayName}.Size}
   		/if (!${Bool[${${arrayName}[${i}]}]}) { | if we hit a null, compress all items past that 1, increasing by 1 with each further null which is hit
			/call ShiftArrayLeft "${arrayName}" ${Math.Calc[${i} + 1].Int} TRUE	
		}
	/next i
/return TRUE




