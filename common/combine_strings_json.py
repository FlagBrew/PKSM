import os
import json
import codecs

master_json = {}

languages = ["chs", "cht", "eng", "fre", "ger",
             "ita", "jpn", "kor", "nl", "pt", "ro", "spa"]

# with open("../assets/gui_strings/en/gui.json", "r") as f:
#     gui_en = json.load(f)

for lang in languages:
    for root, _, files in os.walk('../assets/gui_strings/' + lang):
        for file in files:
            if "gui.json" in file:
                continue
            with open(os.path.join(root, file), 'r') as f:
                master_json = {**master_json, **json.load(f)}
        with codecs.open('../assets/gui_strings/' + lang + '/gui.json', 'w', encoding='utf8') as f:
            json.dump(master_json, f, ensure_ascii=True,
                      sort_keys=True, indent=4)
        master_json = {}
