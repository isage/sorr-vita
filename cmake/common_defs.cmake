add_definitions(-D__MONOLITHIC__)
add_definitions(-DNO_MODCHIPMUNK)
add_definitions(-DNO_MODICONV)
add_definitions(-DNO_MODCHIPMUNK)
add_definitions(-DNO_MODFMODEX)
add_definitions(-DNO_MODCURL)
add_definitions(-DNO_MODMATHI)
add_definitions(-DNO_MODSENSOR)
add_definitions(-DNO_FSOCK)
add_definitions(-DNO_MODIAP)
add_definitions(-DNO_MODIMAGE)
add_definitions(-DVERSION="1.0.0")
# TODO: Make cross-platform
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	add_definitions(-DTARGET_LINUX)
endif()

if (VITA)
	add_definitions(-DTARGET_VITA)
endif()
