#!/bin/sh

echo -e "\tLoading mods!"
mods
read -p "Press enter to continue"

echo -e "\tCreating 12345!"
tool /foo 'ADD\n1\n2\n3\n4\n5\0'
echo -e "Result:"
tool /foo
read -p "Press enter to continue"

echo -e "Searching for 1!"
tool /foo "FIND\n1\0"
echo -e "Result:"
tool /foo
read -p "Press enter to continue"

echo -e "Deleting 0!"
tool /foo "DEL\n0\0"
echo -e "Result:"
tool /foo
read -p "Press enter to continue"

echo -e "Searching for 1!"
tool /foo "FIND\n1\0"
echo -e "Result:"
tool /foo
read -p "Press enter to continue"
