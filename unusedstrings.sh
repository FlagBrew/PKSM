#!/bin/bash

STRINGS="./assets/romfs/i18n/en/gui.json"

while IFS= read -r line
do
    if [[ "$line" != "{" && "$line" != "}" ]];
    then
        KEY=`echo "$line" | cut -d'"' -f 2`
        if [[ $(grep -RIn "$KEY" ./3ds/source | wc -c) -eq 0 ]];
        then
            echo "$KEY is not used"
        fi
    fi
done < "$STRINGS"