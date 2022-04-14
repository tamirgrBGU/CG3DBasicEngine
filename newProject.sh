#!/bin/bash
currnt_dir=`dirname "$0"`
echo Enter dir name
read name
mkdir -p $currnt_dir/tutorial/$name
echo "" >> $currnt_dir/tutorial/CMakeLists.txt
echo "add_subdirectory("$name")" >> $currnt_dir/tutorial/CMakeLists.txt
sed "s/game/$name/gi" $currnt_dir/tutorial/Game/CMakeLists.txt > $currnt_dir/tutorial/$name/CMakeLists.txt
sed "s/Game/$name/gi" $currnt_dir/tutorial/Game/game.cpp > $currnt_dir/tutorial/$name/$name.cpp
sed "s/Game/$name/gi" $currnt_dir/tutorial/Game/game.h > $currnt_dir/tutorial/$name/$name.h
sed "s/Game/$name/gi" $currnt_dir/tutorial/Game/InputManager.h > $currnt_dir/tutorial/$name/InputManager.h
sed "s/Game/$name/gi" $currnt_dir/tutorial/Game/main.cpp > $currnt_dir/tutorial/$name/main.cpp