# Options

PKSM supports user configuration through the options menu
item. Currently configuration is done through the [hex
editor](hex.md).

## Configuration options

| Byte      | Option name                      | Description  |
| --------- | -------------------------------- | -----------  |
| 0x00      | PKSM interface language          | The language to be used for PKSM (0=JP, 1=EN, 2=FR, 3=DE, 4=IT, 5-ES, 6=ZH, 7=KO, 8=NL, 9=PT, A=TW) |
| 0x01      | Perform automatic backup on load | Whether to automatically backup the game when loading |
| 0x02-0x03 | PKSM Storage size                | The number of boxes available for [Storage](bank.md) |
| 0x04-0x05 | Default TID                      | The default trainer ID to set when generating a Pokémon |
| 0x06-0x07 | Default SID                      | The default secret ID to set when generating a Pokémon |
| 0x08-0x1F | Default OT Name                  | The default OT name (in [unicode](https://unicode-table.com/en/#basic-latin)) |
| 0x20      | Default Nationality              | The default OT nationality |
| 0x21      | Edit during transfers            | Whether to modify current holder and similar values when transferring (keep the default when you are not sure) |
| 0x22      | Default day                      | The default encounter day of the month (1-31) |
| 0x23      | Default month                    | The default encounter month of the year          (1-12) |
| 0x24      | Default year                     | The default encounter year (2000 + value) |
| 0x25      | Fix storage bad sectors on exit  | Whether to automatically fix bad sectors in the storage file on exit |
