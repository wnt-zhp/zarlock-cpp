#!/bin/sh
cat <<EOF
[Desktop Entry]
Version=1.0
Type=Application
Name=Zarlok
GenericName=Database manager
Comment=Quatermaster manager
TryExec=$1/bin/zarlok
Exec=$1/bin/zarlok &
Categories=Application;Office;
Icon=$1/share/icons/zarlok.png
EOF