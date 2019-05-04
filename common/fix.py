import os
import json

gui_en = {}

with open("../assets/romfs/i18n/en/gui.json", "r") as f:
    gui_en = json.load(f)

for root, _, files in os.walk('../assets/romfs/i18n'):
    for file in files:
        if 'gui.json' in file and 'en' not in root:
            current_dict = {}
            with open(os.path.join(root, file), 'r') as f:
                current_dict = json.load(f)
            for key in gui_en:
                if key not in current_dict:
                    current_dict[key] = gui_en[key]
            with open(os.path.join(root, file), 'w') as f:
                f.write(json.dumps(current_dict, indent=4, sort_keys=True))
            # with io.open(os.path.join(root, file), 'w', encoding='utf8') as f:
            #     f.write(unicode(json.dumps(current_dict, indent=4, sort_keys=True, ensure_ascii=False)))
