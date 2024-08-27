
# TODO: this needs to look for version.txt and parse from that instead of git
# when present.
function(get_version_from_git outputVersion outputCommits outputCommitHash)

	set(${outputVersion} "" PARENT_SCOPE)
	set(${outputCommits} "" PARENT_SCOPE)
	set(${outputCommitHash} "" PARENT_SCOPE)

	find_package(Git)

	if (NOT GIT_FOUND)
		message(FATAL_ERROR "Git must be installed on this system")
		return()
	endif()

	# --long ensures we always get commits + hash
	execute_process(COMMAND ${GIT_EXECUTABLE} "describe" "--tags" "--long" WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} OUTPUT_VARIABLE gitDescribeOutput)
	string(REPLACE "\n" "" gitDescribeOutput ${gitDescribeOutput})
	string(REPLACE "-" ";" gitDescribeOutput ${gitDescribeOutput})
	list(LENGTH gitDescribeOutput outputLength)
	if (${outputLength} GREATER_EQUAL 3) # the tag+tagId+commit+hash
		math(EXPR hIndex "${outputLength} - 1")
		math(EXPR cIndex "${outputLength} - 2")
		math(EXPR tEnd "${outputLength} - 3")

		set(t "")
		list(GET gitDescribeOutput ${cIndex} c)
		list(GET gitDescribeOutput ${hIndex} h)
		string(REPLACE "g" "" h ${h})

		foreach(tIndex RANGE ${tEnd})
			if (${tIndex} GREATER 0)
				set(t "${t}-")
			endif()
			list(GET gitDescribeOutput ${tIndex} tStr)
			set(t "${t}${tStr}")
		endforeach()
		set(${outputVersion} ${t} PARENT_SCOPE)
		set(${outputCommits} ${c} PARENT_SCOPE)
		set(${outputCommitHash} ${h} PARENT_SCOPE)
	else()
		message(FATAL_ERROR "Could not parse results of git describe")
		return()
	endif()

endfunction()

function(get_branch_from_git outputVariable)

	set(${outputVariable} "" PARENT_SCOPE)

	find_package(Git)
	if (NOT GIT_FOUND)
		message(FATAL_ERROR "Git must be installed on this system")
		return()
	endif()

	execute_process(COMMAND ${GIT_EXECUTABLE} "rev-parse" "--abbrev-ref" "HEAD" 
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
		OUTPUT_VARIABLE branchName
		RESULT_VARIABLE result
	)

	string(REPLACE "\n" "" branchName ${branchName})

	if(${result} EQUAL 0)
		set(${outputVariable} ${branchName} PARENT_SCOPE)
	endif()

endfunction()
