# PSn00bSDK example CMake script
# (C) 2021 spicyjpeg - MPL licensed

cmake_minimum_required(VERSION 3.20)

project(
	vecx-psx
	LANGUAGES    C CXX ASM
	VERSION      1.0.0
	DESCRIPTION  "A Vectrex emulator for the PSX"
	HOMEPAGE_URL "https://github.com/wildmonkeydan/vecx-psx"
)

psn00bsdk_add_executable(vecx STATIC ${sourceDir}osint.c ${sourceDir}e6809.c ${sourceDir}e8910.c ${sourceDir}vecx.c)

psn00bsdk_add_cd_image(
	iso      # Target name
	vecx # Output file name (= template.bin + template.cue)
	iso.xml  # Path to config file
	DEPENDS vecx
)
