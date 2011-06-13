#!/bin/bash

CPPFILES=$(find src -iname "*.cpp")
HFILES=$(find src -iname "*.h")
UIFILES=$(find src/ui -iname "*.ui")

lupdate -no-obsolete main.cpp $CPPFILES $HFILES $UIFILES -ts src/translations/zarlok_pl.ts

