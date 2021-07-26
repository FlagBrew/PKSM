import os
import json
import codecs

gui_en = {}

with open("../assets/romfs/i18n/en/gui.json", "r") as f:
    gui_en = json.load(f)

for root, _, files in os.walk('../assets/romfs/i18n'):
    for file in files:
        if 'gui.json' in file and 'en' not in root:
            current_dict = {}
            with codecs.open(os.path.join(root, file), 'r', encoding='utf8') as f:
                current_dict = json.load(f)
            if 'zh' not in root and 'jp' not in root and 'ko' not in root:
                for key in gui_en:
                    if key not in current_dict:
                        current_dict[key] = gui_en[key]
                with codecs.open(os.path.join(root, file), 'w', encoding='utf8') as f:
                    f.write(json.dumps(current_dict, indent=4, sort_keys=True))
            else:
                for key in gui_en:
                    if key not in current_dict:
                        current_dict[key] = gui_en[key]
                with codecs.open(os.path.join(root, file), 'w', encoding='utf8') as f:
                    json.dump(current_dict, f, ensure_ascii=False, sort_keys=True, indent=4)
