#
# Copyright (c) 2015 Pavlo Lavrenenko
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#

cmake_minimum_required(VERSION 3.9)
project (SVDLL)

set (SVDLL_LIBRARY server)

if(NOT MSVC)
	add_compile_options(-fno-exceptions) # GCC/Clang flag
	add_compile_options(-fno-rtti) # GCC/Clang flag
	add_compile_options(-Wno-invalid-offsetof) # GCC/Clang flag
	add_compile_options(-fvisibility=hidden) # GCC/Clang flag
	add_definitions(-D_LINUX) # It seems enough for all non-Win32 systems
	add_definitions(-Dstricmp=strcasecmp -Dstrnicmp=strncasecmp -D_snprintf=snprintf -D_vsnprintf=vsnprintf )
	if(CMAKE_BUILD_TYPE MATCHES "Release"
		OR (CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo"
		AND CMAKE_SYSTEM_NAME STREQUAL "Android"))
		add_compile_options(-fno-unwind-tables -fno-asynchronous-unwind-tables) # GCC/Clang flag
		add_compile_options(-fomit-frame-pointer) # GCC/Clang flag
		add_compile_options(-ftree-vectorize) # GCC/Clang flag
		add_compile_options(-funsafe-math-optimizations) # GCC/Clang flag
	endif()
else()
	add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE)
endif()

set (SVDLL_SOURCES
	# aflock.cpp
	# agrunt.cpp
	# airtank.cpp
	animating.cpp
	animation.cpp
	aura.cpp	# ESF ALPHA 2.0
	# apache.cpp
	# barnacle.cpp
	# barney.cpp
	# bigmomma.cpp
	# bloater.cpp
	bigbang.cpp	# ESF ALPHA 2.0
	bmodels.cpp
	# bullsquid.cpp
	burningattack.cpp	# ESF ALPHA 2.0
	buttons.cpp
	buu.cpp	# ESF ALPHA 2.0
	capturethedragonballs_gamerules.cpp	# ESF ALPHA 2.0
	cbase.cpp
	cell.cpp	# ESF ALPHA 2.0
	classes.cpp	# ESF ALPHA 2.0
	client.cpp
	combat.cpp
	controlledweapon.cpp	# ESF ALPHA 2.0
	controller.cpp
	# crossbow.cpp
	# crowbar.cpp
	credits.cpp	# ESF ALPHA 2.0
	deathball.cpp	# ESF ALPHA 2.0
	defaultai.cpp
	destructodisc.cpp	# ESF ALPHA 2.0
	doors.cpp
	dragonball.cpp	# ESF ALPHA 2.0
	effects.cpp
	# egon.cpp
	envmodel.cpp	# ESF ALPHA 2.0
	explode.cpp
	eyelaser.cpp	# ESF ALPHA 2.0
	finalflash.cpp	# ESF ALPHA 2.0
	fingerlaser.cpp	# ESF ALPHA 2.0
	flyingmonster.cpp
	frieza.cpp	# ESF ALPHA 2.0
	friezadisc.cpp	# ESF ALPHA 2.0
	func_break.cpp
	# func_tank.cpp
	gallitgun.cpp	# ESF ALPHA 2.0
	game.cpp
	gamerules.cpp
	# gargantua.cpp
	# gauss.cpp
	genericmonster.cpp
	# ggrenade.cpp
	globals.cpp
	# glock.cpp
	# gman.cpp
	gohan.cpp	# ESF ALPHA 2.0
	goku.cpp	# ESF ALPHA 2.0
	# handgrenade.cpp
	# hassassin.cpp
	# headcrab.cpp
	# healthkit.cpp
	# hgrunt.cpp
	# hornet.cpp
	# hornetgun.cpp
	# houndeye.cpp
	h_ai.cpp
	# h_battery.cpp
	# h_cine.cpp
	h_cycler.cpp
	h_export.cpp
	# ichthyosaur.cpp
	# islave.cpp
	items.cpp
	kamehameha.cpp	# ESF ALPHA 2.0
	kiblast.cpp	# ESF ALPHA 2.0
	krillin.cpp	# ESF ALPHA 2.0
	# leech.cpp
	lights.cpp
	magicattack.cpp	# ESF ALPHA 2.0
	maprules.cpp
	masenko.cpp	# ESF ALPHA 2.0
	melee.cpp	# ESF ALPHA 2.0
	monstermaker.cpp
	monsters.cpp
	monsterstate.cpp
	# mortar.cpp
	# mp5.cpp
	multiplay_gamerules.cpp
	# nihilanth.cpp
	nodes.cpp
	observer.cpp
	# osprey.cpp
	pathcorner.cpp
	piccolo.cpp	# ESF ALPHA 2.0
	plane.cpp
	plats.cpp
	player.cpp
	# python.cpp
	# rat.cpp
	# roach.cpp
	# rpg.cpp
	# satchel.cpp
	schedule.cpp
	# scientist.cpp
	scripted.cpp
	sensubean.cpp	# ESF ALPHA 2.0
	senzubean.cpp	# ESF ALPHA 2.0
	# shotgun.cpp
	singleplay_gamerules.cpp
	skill.cpp
	solarflare.cpp	# ESF ALPHA 2.0
	sound.cpp
	soundent.cpp
	specialbeamcannon.cpp	# ESF ALPHA 2.0
	spectator.cpp
	spiritbomb.cpp	# ESF ALPHA 2.0
	squadmonster.cpp
	# squeakgrenade.cpp
	subs.cpp
	talkmonster.cpp
	teamplay_gamerules.cpp
	tempmonster.cpp
	# tentacle.cpp
	trail.cpp	# ESF ALPHA 2.0
	triggers.cpp
	# tripmine.cpp
	trunks.cpp	# ESF ALPHA 2.0
	# turret.cpp
	util.cpp
	vegeta.cpp	# ESF ALPHA 2.0
	# vehicle.cpp
	weapons.cpp
	world.cpp
	xen.cpp
	# zombie.cpp
	../pm_shared/pm_debug.c
	../pm_shared/pm_math.c
	../pm_shared/pm_shared.c
	../public/safe_snprintf.c
)

if (HAVE_STRLCPY)
	add_definitions(-DHAVE_STRLCPY)
else()
	list(APPEND SVDLL_SOURCES
		../external/openbsd/strlcpy.c)
endif()

if (HAVE_STRLCAT)
	add_definitions(-DHAVE_STRLCAT)
else()
	list(APPEND SVDLL_SOURCES
		../external/openbsd/strlcat.c)
endif()

include_directories (. wpn_shared ../common ../engine ../pm_shared ../game_shared ../public)

if(MSVC)
	set(SVDLL_SOURCES
		${SVDLL_SOURCES}
		hl.def)
endif()

if(USE_VOICEMGR)
	set(SVDLL_SOURCES
		${SVDLL_SOURCES}
		../game_shared/voice_gamemgr.cpp)
else()
	add_definitions(-DNO_VOICEGAMEMGR)
endif()

add_library (${SVDLL_LIBRARY} SHARED ${SVDLL_SOURCES})
target_link_libraries(${SVDLL_LIBRARY} vcs_info)

set_target_properties (${SVDLL_LIBRARY} PROPERTIES POSITION_INDEPENDENT_CODE 1)

if(APPLE AND NOT "${SERVER_LIBRARY_NAME_OSX}" STREQUAL "")
	# on Apple platforms we always use gamedll_osx liblist/gameinfo key, if it's set
	set(SVDLL_NAME "${SERVER_LIBRARY_NAME_OSX}")
elseif(NOT WIN32 AND NOT "${SERVER_LIBRARY_NAME_LINUX}" STREQUAL "")
	# on non-Windows platforms we use gamedll_linux liblist/gameinfo key, if it's set
	set(SVDLL_NAME "${SERVER_LIBRARY_NAME_LINUX}")
else()
	# on Windows or if no gamedll_osx or gamedll_linux is set, use default gamedll key
	set(SVDLL_NAME "${SERVER_LIBRARY_NAME}")
endif()

set_target_postfix_with_name(${SVDLL_LIBRARY} ${SVDLL_NAME})

set_property(TARGET ${SVDLL_LIBRARY} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

if(HAVE_LTO)
	set_property(TARGET ${SVDLL_LIBRARY} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

install(TARGETS ${SVDLL_LIBRARY}
	RUNTIME DESTINATION "${GAMEDIR}/${SERVER_INSTALL_DIR}/"
	LIBRARY DESTINATION "${GAMEDIR}/${SERVER_INSTALL_DIR}/"
	PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
		GROUP_READ GROUP_EXECUTE
		WORLD_READ WORLD_EXECUTE)

if(CMAKE_BUILD_TYPE MATCHES "Release" AND NOT WIN32)
	add_custom_command(TARGET ${SVDLL_LIBRARY}
			POST_BUILD DEPENDS ${SVDLL_LIBRARY}
			COMMAND ${CMAKE_STRIP} -s $<TARGET_FILE:${SVDLL_LIBRARY}>)
endif()
