/* This file is part of PKSM

Copyright (C) 2016 Bernardo Giordano

>    This program is free software: you can redistribute it and/or modify
>    it under the terms of the GNU General Public License as published by
>    the Free Software Foundation, either version 3 of the License, or
>    (at your option) any later version.
>
>    This program is distributed in the hope that it will be useful,
>    but WITHOUT ANY WARRANTY; without even the implied warranty of
>    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
>    GNU General Public License for more details.
>
>    You should have received a copy of the GNU General Public License
>    along with this program.  If not, see <http://www.gnu.org/licenses/>.
>    See LICENSE for information.
*/

#include <sf2d.h>
#include <sftd.h>
#include <sfil.h>
#include <stdio.h>
#include <time.h>
#include "graphic.h"
#include "fx.h"
#include "editor.h"
#include "http.h"

int lookup[] = {0x0, 0x1, 0x2, 0x4, 0x5, 0x3};

sftd_font *fontBold18, *fontBold15, *fontBold14, *fontBold12, *fontBold11, *fontBold9, *fontFixed; 
sf2d_texture *upperTextGS, *lowerTextGS, *dexBox, *dexN, *hiddenPowerBG, *hiddenPowerButton, *selectBoxButton, *ballsBG, *ballButton, *male, *female, *naturesButton, *naturestx, *movesBottom, *topMovesBG, *editorBar, *editorStatsBG, *subArrow, *backgroundTop, *miniBox, *plusButton, *minusButton, *balls, *typesSheet, *bottomMask, *transferButton, *bankTop, *shinyStar, *normalBar, *LButton, *RButton, *creditsTop, *pokeball, *topBorder, *bottomBorder, *gameSelectorBottom, *gameSelectorTop, *mainMenuBottom, *menuBar, *menuSelectedBar, *darkButton, *eventTop, *left, *lightButton, *redButton, *right, *spritesSmall, *eventMenuBottomBar, *eventMenuTopBarSelected, *eventMenuTopBar, *warningTop, *warningBottom, *boxView, *infoView, *LRBar, *selector, *editorBG, *plus, *minus, *back, *setting, *selectorCloning, *button, *bottomPopUp, *pokemonBufferBox, *cleanTop, *DSBottomBG, *DSTopBG, *DSBarSelected, *DSBar, *DSEventBottom, *DSLangSelected, *DSLang, *DSEventTop, *DSNormalBarL, *DSNormalBarR, *DSSelectedBarL, *DSSelectedBarR, *topSelectedMove, *settings, *item, *alternativeSpritesSmall;

char *gamesList[] = {"X", "Y", "OR", "AS", "S", "M", "D", "P", "PL", "HG", "SS", "B", "W", "B2", "W2"};

static char *abilities[] = {"None", "Stench", "Drizzle", "Speed Boost", "Battle Armor", "Sturdy", "Damp", "Limber", "Sand Veil", "Static", "Volt Absorb", "Water Absorb", "Oblivious", "Cloud Nine", "Compound Eyes", "Insomnia", "Color Change", "Immunity", "Flash Fire", "Shield Dust", "Own Tempo", "Suction Cups", "Intimidate", "Shadow Tag", "Rough Skin", "Wonder Guard", "Levitate", "Effect Spore", "Synchronize", "Clear Body", "Natural Cure", "Lightning Rod", "Serene Grace", "Swift Swim", "Chlorophyll", "Illuminate", "Trace", "Huge Power", "Poison Point", "Inner Focus", "Magma Armor", "Water Veil", "Magnet Pull", "Soundproof", "Rain Dish", "Sand Stream", "Pressure", "Thick Fat", "Early Bird", "Flame Body", "Run Away", "Keen Eye", "Hyper Cutter", "Pickup", "Truant", "Hustle", "Cute Charm", "Plus", "Minus", "Forecast", "Sticky Hold", "Shed Skin", "Guts", "Marvel Scale", "Liquid Ooze", "Overgrow", "Blaze", "Torrent", "Swarm", "Rock Head", "Drought", "Arena Trap", "Vital Spirit", "White Smoke", "Pure Power", "Shell Armor", "Air Lock", "Tangled Feet", "Motor Drive", "Rivalry", "Steadfast", "Snow Cloak", "Gluttony", "Anger Point", "Unburden", "Heatproof", "Simple", "Dry Skin", "Download", "Iron Fist", "Poison Heal", "Adaptability", "Skill Link", "Hydration", "Solar Power", "Quick Feet", "Normalize", "Sniper", "Magic Guard", "No Guard", "Stall", "Technician", "Leaf Guard", "Klutz", "Mold Breaker", "Super Luck", "Aftermath", "Anticipation", "Forewarn", "Unaware", "Tinted Lens", "Filter", "Slow Start", "Scrappy", "Storm Drain", "Ice Body", "Solid Rock", "Snow Warning", "Honey Gather", "Frisk", "Reckless", "Multitype", "Flower Gift", "Bad Dreams", "Pickpocket", "Sheer Force", "Contrary", "Unnerve", "Defiant", "Defeatist", "Cursed Body", "Healer", "Friend Guard", "Weak Armor", "Heavy Metal", "Light Metal", "Multiscale", "Toxic Boost", "Flare Boost", "Harvest", "Telepathy", "Moody", "Overcoat", "Poison Touch", "Regenerator", "Big Pecks", "Sand Rush", "Wonder Skin", "Analytic", "Illusion", "Imposter", "Infiltrator", "Mummy", "Moxie", "Justified", "Rattled", "Magic Bounce", "Sap Sipper", "Prankster", "Sand Force", "Iron Barbs", "Zen Mode", "Victory Star", "Turboblaze", "Teravolt", "Aroma Veil", "Flower Veil", "Cheek Pouch", "Protean", "Fur Coat", "Magician", "Bulletproof", "Competitive", "Strong Jaw", "Refrigerate", "Sweet Veil", "Stance Change", "Gale Wings", "Mega Launcher", "Grass Pelt", "Symbiosis", "Tough Claws", "Pixilate", "Gooey", "Aerilate", "Parental Bond", "Dark Aura", "Fairy Aura", "Aura Break", "Primordial Sea", "Desolate Land", "Delta Stream", "Stamina", "Wimp Out", "Emergency Exit", "Water Compaction", "Merciless", "Shields Down", "Stakeout", "Water Bubble", "Steelworker", "Berserk", "Slush Rush", "Long Reach", "Liquid Voice", "Triage", "Galvanize", "Surge Surfer", "Schooling", "Disguise", "Battle Bond", "Power Construct", "Corrosion", "Comatose", "Queenly Majesty", "Innards Out", "Dancer", "Battery", "Fluffy", "Dazzling", "Soul-Heart", "Tangling Hair", "Receiver", "Power of Alchemy", "Beast Boost", "RKS System", "Electric Surge", "Psychic Surge", "Misty Surge", "Grassy Surge", "Full Metal Body", "Shadow Shield", "Prism Armor"};
static char *moves[] = {"None", "Pound", "Karate Chop", "Double Slap", "Comet Punch", "Mega Punch", "Pay Day", "Fire Punch", "Ice Punch", "Thunder Punch", "Scratch", "Vice Grip", "Guillotine", "Razor Wind", "Swords Dance", "Cut", "Gust", "Wing Attack", "Whirlwind", "Fly", "Bind", "Slam", "Vine Whip", "Stomp", "Double Kick", "Mega Kick", "Jump Kick", "Rolling Kick", "Sand Attack", "Headbutt", "Horn Attack", "Fury Attack", "Horn Drill", "Tackle", "Body Slam", "Wrap", "Take Down", "Thrash", "Double-Edge", "Tail Whip", "Poison Sting", "Twineedle", "Pin Missile", "Leer", "Bite", "Growl", "Roar", "Sing", "Supersonic", "Sonic Boom", "Disable", "Acid", "Ember", "Flamethrower", "Mist", "Water Gun", "Hydro Pump", "Surf", "Ice Beam", "Blizzard", "Psybeam", "Bubble Beam", "Aurora Beam", "Hyper Beam", "Peck", "Drill Peck", "Submission", "Low Kick", "Counter", "Seismic Toss", "Strength", "Absorb", "Mega Drain", "Leech Seed", "Growth", "Razor Leaf", "Solar Beam", "Poison Powder", "Stun Spore", "Sleep Powder", "Petal Dance", "String Shot", "Dragon Rage", "Fire Spin", "Thunder Shock", "Thunderbolt", "Thunder Wave", "Thunder", "Rock Throw", "Earthquake", "Fissure", "Dig", "Toxic", "Confusion", "Psychic", "Hypnosis", "Meditate", "Agility", "Quick Attack", "Rage", "Teleport", "Night Shade", "Mimic", "Screech", "Double Team", "Recover", "Harden", "Minimize", "Smokescreen", "Confuse Ray", "Withdraw", "Defense Curl", "Barrier", "Light Screen", "Haze", "Reflect", "Focus Energy", "Bide", "Metronome", "Mirror Move", "Self-Destruct", "Egg Bomb", "Lick", "Smog", "Sludge", "Bone Club", "Fire Blast", "Waterfall", "Clamp", "Swift", "Skull Bash", "Spike Cannon", "Constrict", "Amnesia", "Kinesis", "Soft-Boiled", "High Jump Kick", "Glare", "Dream Eater", "Poison Gas", "Barrage", "Leech Life", "Lovely Kiss", "Sky Attack", "Transform", "Bubble", "Dizzy Punch", "Spore", "Flash", "Psywave", "Splash", "Acid Armor", "Crabhammer", "Explosion", "Fury Swipes", "Bonemerang", "Rest", "Rock Slide", "Hyper Fang", "Sharpen", "Conversion", "Tri Attack", "Super Fang", "Slash", "Substitute", "Struggle", "Sketch", "Triple Kick", "Thief", "Spider Web", "Mind Reader", "Nightmare", "Flame Wheel", "Snore", "Curse", "Flail", "Conversion 2", "Aeroblast", "Cotton Spore", "Reversal", "Spite", "Powder Snow", "Protect", "Mach Punch", "Scary Face", "Feint Attack", "Sweet Kiss", "Belly Drum", "Sludge Bomb", "Mud-Slap", "Octazooka", "Spikes", "Zap Cannon", "Foresight", "Destiny Bond", "Perish Song", "Icy Wind", "Detect", "Bone Rush", "Lock-On", "Outrage", "Sandstorm", "Giga Drain", "Endure", "Charm", "Rollout", "False Swipe", "Swagger", "Milk Drink", "Spark", "Fury Cutter", "Steel Wing", "Mean Look", "Attract", "Sleep Talk", "Heal Bell", "Return", "Present", "Frustration", "Safeguard", "Pain Split", "Sacred Fire", "Magnitude", "Dynamic Punch", "Megahorn", "Dragon Breath", "Baton Pass", "Encore", "Pursuit", "Rapid Spin", "Sweet Scent", "Iron Tail", "Metal Claw", "Vital Throw", "Morning Sun", "Synthesis", "Moonlight", "Hidden Power", "Cross Chop", "Twister", "Rain Dance", "Sunny Day", "Crunch", "Mirror Coat", "Psych Up", "Extreme Speed", "Ancient Power", "Shadow Ball", "Future Sight", "Rock Smash", "Whirlpool", "Beat Up", "Fake Out", "Uproar", "Stockpile", "Spit Up", "Swallow", "Heat Wave", "Hail", "Torment", "Flatter", "Will-O-Wisp", "Memento", "Facade", "Focus Punch", "Smelling Salts", "Follow Me", "Nature Power", "Charge", "Taunt", "Helping Hand", "Trick", "Role Play", "Wish", "Assist", "Ingrain", "Superpower", "Magic Coat", "Recycle", "Revenge", "Brick Break", "Yawn", "Knock Off", "Endeavor", "Eruption", "Skill Swap", "Imprison", "Refresh", "Grudge", "Snatch", "Secret Power", "Dive", "Arm Thrust", "Camouflage", "Tail Glow", "Luster Purge", "Mist Ball", "Feather Dance", "Teeter Dance", "Blaze Kick", "Mud Sport", "Ice Ball", "Needle Arm", "Slack Off", "Hyper Voice", "Poison Fang", "Crush Claw", "Blast Burn", "Hydro Cannon", "Meteor Mash", "Astonish", "Weather Ball", "Aromatherapy", "Fake Tears", "Air Cutter", "Overheat", "Odor Sleuth", "Rock Tomb", "Silver Wind", "Metal Sound", "Grass Whistle", "Tickle", "Cosmic Power", "Water Spout", "Signal Beam", "Shadow Punch", "Extrasensory", "Sky Uppercut", "Sand Tomb", "Sheer Cold", "Muddy Water", "Bullet Seed", "Aerial Ace", "Icicle Spear", "Iron Defense", "Block", "Howl", "Dragon Claw", "Frenzy Plant", "Bulk Up", "Bounce", "Mud Shot", "Poison Tail", "Covet", "Volt Tackle", "Magical Leaf", "Water Sport", "Calm Mind", "Leaf Blade", "Dragon Dance", "Rock Blast", "Shock Wave", "Water Pulse", "Doom Desire", "Psycho Boost", "Roost", "Gravity", "Miracle Eye", "Wake-Up Slap", "Hammer Arm", "Gyro Ball", "Healing Wish", "Brine", "Natural Gift", "Feint", "Pluck", "Tailwind", "Acupressure", "Metal Burst", "U-turn", "Close Combat", "Payback", "Assurance", "Embargo", "Fling", "Psycho Shift", "Trump Card", "Heal Block", "Wring Out", "Power Trick", "Gastro Acid", "Lucky Chant", "Me First", "Copycat", "Power Swap", "Guard Swap", "Punishment", "Last Resort", "Worry Seed", "Sucker Punch", "Toxic Spikes", "Heart Swap", "Aqua Ring", "Magnet Rise", "Flare Blitz", "Force Palm", "Aura Sphere", "Rock Polish", "Poison Jab", "Dark Pulse", "Night Slash", "Aqua Tail", "Seed Bomb", "Air Slash", "X-Scissor", "Bug Buzz", "Dragon Pulse", "Dragon Rush", "Power Gem", "Drain Punch", "Vacuum Wave", "Focus Blast", "Energy Ball", "Brave Bird", "Earth Power", "Switcheroo", "Giga Impact", "Nasty Plot", "Bullet Punch", "Avalanche", "Ice Shard", "Shadow Claw", "Thunder Fang", "Ice Fang", "Fire Fang", "Shadow Sneak", "Mud Bomb", "Psycho Cut", "Zen Headbutt", "Mirror Shot", "Flash Cannon", "Rock Climb", "Defog", "Trick Room", "Draco Meteor", "Discharge", "Lava Plume", "Leaf Storm", "Power Whip", "Rock Wrecker", "Cross Poison", "Gunk Shot", "Iron Head", "Magnet Bomb", "Stone Edge", "Captivate", "Stealth Rock", "Grass Knot", "Chatter", "Judgment", "Bug Bite", "Charge Beam", "Wood Hammer", "Aqua Jet", "Attack Order", "Defend Order", "Heal Order", "Head Smash", "Double Hit", "Roar of Time", "Spacial Rend", "Lunar Dance", "Crush Grip", "Magma Storm", "Dark Void", "Seed Flare", "Ominous Wind", "Shadow Force", "Hone Claws", "Wide Guard", "Guard Split", "Power Split", "Wonder Room", "Psyshock", "Venoshock", "Autotomize", "Rage Powder", "Telekinesis", "Magic Room", "Smack Down", "Storm Throw", "Flame Burst", "Sludge Wave", "Quiver Dance", "Heavy Slam", "Synchronoise", "Electro Ball", "Soak", "Flame Charge", "Coil", "Low Sweep", "Acid Spray", "Foul Play", "Simple Beam", "Entrainment", "After You", "Round", "Echoed Voice", "Chip Away", "Clear Smog", "Stored Power", "Quick Guard", "Ally Switch", "Scald", "Shell Smash", "Heal Pulse", "Hex", "Sky Drop", "Shift Gear", "Circle Throw", "Incinerate", "Quash", "Acrobatics", "Reflect Type", "Retaliate", "Final Gambit", "Bestow", "Inferno", "Water Pledge", "Fire Pledge", "Grass Pledge", "Volt Switch", "Struggle Bug", "Bulldoze", "Frost Breath", "Dragon Tail", "Work Up", "Electroweb", "Wild Charge", "Drill Run", "Dual Chop", "Heart Stamp", "Horn Leech", "Sacred Sword", "Razor Shell", "Heat Crash", "Leaf Tornado", "Steamroller", "Cotton Guard", "Night Daze", "Psystrike", "Tail Slap", "Hurricane", "Head Charge", "Gear Grind", "Searing Shot", "Techno Blast", "Relic Song", "Secret Sword", "Glaciate", "Bolt Strike", "Blue Flare", "Fiery Dance", "Freeze Shock", "Ice Burn", "Snarl", "Icicle Crash", "V-create", "Fusion Flare", "Fusion Bolt", "Flying Press", "Mat Block", "Belch", "Rototiller", "Sticky Web", "Fell Stinger", "Phantom Force", "Trick-or-Treat", "Noble Roar", "Ion Deluge", "Parabolic Charge", "Forest’s Curse", "Petal Blizzard", "Freeze-Dry", "Disarming Voice", "Parting Shot", "Topsy-Turvy", "Draining Kiss", "Crafty Shield", "Flower Shield", "Grassy Terrain", "Misty Terrain", "Electrify", "Play Rough", "Fairy Wind", "Moonblast", "Boomburst", "Fairy Lock", "King’s Shield", "Play Nice", "Confide", "Diamond Storm", "Steam Eruption", "Hyperspace Hole", "Water Shuriken", "Mystical Fire", "Spiky Shield", "Aromatic Mist", "Eerie Impulse", "Venom Drench", "Powder", "Geomancy", "Magnetic Flux", "Happy Hour", "Electric Terrain", "Dazzling Gleam", "Celebrate", "Hold Hands", "Baby-Doll Eyes", "Nuzzle", "Hold Back", "Infestation", "Power-Up Punch", "Oblivion Wing", "Thousand Arrows", "Thousand Waves", "Land’s Wrath", "Light of Ruin", "Origin Pulse", "Precipice Blades", "Dragon Ascent", "Hyperspace Fury", "Breakneck Blitz", "Breakneck Blitz", "All-Out Pummeling", "All-Out Pummeling", "Supersonic Skystrike", "Supersonic Skystrike", "Acid Downpour", "Acid Downpour", "Tectonic Rage", "Tectonic Rage", "Continental Crush", "Continental Crush", "Savage Spin-Out", "Savage Spin-Out", "Never-Ending Nightmare", "Never-Ending Nightmare", "Corkscrew Crash", "Corkscrew Crash", "Inferno Overdrive", "Inferno Overdrive", "Hydro Vortex", "Hydro Vortex", "Bloom Doom", "Bloom Doom", "Gigavolt Havoc", "Gigavolt Havoc", "Shattered Psyche", "Shattered Psyche", "Subzero Slammer", "Subzero Slammer", "Devastating Drake", "Devastating Drake", "Black Hole Eclipse", "Black Hole Eclipse", "Twinkle Tackle", "Twinkle Tackle", "Catastropika", "Shore Up", "First Impression", "Baneful Bunker", "Spirit Shackle", "Darkest Lariat", "Sparkling Aria", "Ice Hammer", "Floral Healing", "High Horsepower", "Strength Sap", "Solar Blade", "Leafage", "Spotlight", "Toxic Thread", "Laser Focus", "Gear Up", "Throat Chop", "Pollen Puff", "Anchor Shot", "Psychic Terrain", "Lunge", "Fire Lash", "Power Trip", "Burn Up", "Speed Swap", "Smart Strike", "Purify", "Revelation Dance", "Core Enforcer", "Trop Kick", "Instruct", "Beak Blast", "Clanging Scales", "Dragon Hammer", "Brutal Swing", "Aurora Veil", "Sinister Arrow Raid", "Malicious Moonsault", "Oceanic Operetta", "Guardian of Alola", "Soul-Stealing 7-Star Strike", "Stoked Sparksurfer", "Pulverizing Pancake", "Extreme Evoboost", "Genesis Supernova", "Shell Trap", "Fleur Cannon", "Psychic Fangs", "Stomping Tantrum", "Shadow Bone", "Accelerock", "Liquidation", "Prismatic Laser", "Spectral Thief", "Sunsteel Strike", "Moongeist Beam", "Tearful Look", "Zing Zap", "Nature’s Madness", "Multi-Attack", "10,000,000 Volt Thunderbolt"};
static char *movesSorted[] = {"None", "Absorb", "Accelerock", "Acid", "Acid Armor", "Acid Downpour", "Acid Downpour", "Acid Spray", "Acrobatics", "Acupressure", "Aerial Ace", "Aeroblast", "After You", "Agility", "Air Cutter", "Air Slash", "All-Out Pummeling", "All-Out Pummeling", "Ally Switch", "Amnesia", "Anchor Shot", "Ancient Power", "Aqua Jet", "Aqua Ring", "Aqua Tail", "Arm Thrust", "Aromatherapy", "Aromatic Mist", "Assist", "Assurance", "Astonish", "Attack Order", "Attract", "Aura Sphere", "Aurora Beam", "Aurora Veil", "Autotomize", "Avalanche", "Baby-Doll Eyes", "Baneful Bunker", "Barrage", "Barrier", "Baton Pass", "Beak Blast", "Beat Up", "Belch", "Belly Drum", "Bestow", "Bide", "Bind", "Bite", "Black Hole Eclipse", "Black Hole Eclipse", "Blast Burn", "Blaze Kick", "Blizzard", "Block", "Bloom Doom", "Bloom Doom", "Blue Flare", "Body Slam", "Bolt Strike", "Bone Club", "Bone Rush", "Bonemerang", "Boomburst", "Bounce", "Brave Bird", "Breakneck Blitz", "Breakneck Blitz", "Brick Break", "Brine", "Brutal Swing", "Bubble", "Bubble Beam", "Bug Bite", "Bug Buzz", "Bulk Up", "Bulldoze", "Bullet Punch", "Bullet Seed", "Burn Up", "Calm Mind", "Camouflage", "Captivate", "Catastropika", "Celebrate", "Charge", "Charge Beam", "Charm", "Chatter", "Chip Away", "Circle Throw", "Clamp", "Clanging Scales", "Clear Smog", "Close Combat", "Coil", "Comet Punch", "Confide", "Confuse Ray", "Confusion", "Constrict", "Continental Crush", "Continental Crush", "Conversion", "Conversion 2", "Copycat", "Core Enforcer", "Corkscrew Crash", "Corkscrew Crash", "Cosmic Power", "Cotton Guard", "Cotton Spore", "Counter", "Covet", "Crabhammer", "Crafty Shield", "Cross Chop", "Cross Poison", "Crunch", "Crush Claw", "Crush Grip", "Curse", "Cut", "Dark Pulse", "Dark Void", "Darkest Lariat", "Dazzling Gleam", "Defend Order", "Defense Curl", "Defog", "Destiny Bond", "Detect", "Devastating Drake", "Devastating Drake", "Diamond Storm", "Dig", "Disable", "Disarming Voice", "Discharge", "Dive", "Dizzy Punch", "Doom Desire", "Double Hit", "Double Kick", "Double Slap", "Double Team", "Double-Edge", "Draco Meteor", "Dragon Ascent", "Dragon Breath", "Dragon Claw", "Dragon Dance", "Dragon Hammer", "Dragon Pulse", "Dragon Rage", "Dragon Rush", "Dragon Tail", "Drain Punch", "Draining Kiss", "Dream Eater", "Drill Peck", "Drill Run", "Dual Chop", "Dynamic Punch", "Earth Power", "Earthquake", "Echoed Voice", "Eerie Impulse", "Egg Bomb", "Electric Terrain", "Electrify", "Electro Ball", "Electroweb", "Embargo", "Ember", "Encore", "Endeavor", "Endure", "Energy Ball", "Entrainment", "Eruption", "Explosion", "Extrasensory", "Extreme Evoboost", "Extreme Speed", "Facade", "Fairy Lock", "Fairy Wind", "Fake Out", "Fake Tears", "False Swipe", "Feather Dance", "Feint", "Feint Attack", "Fell Stinger", "Fiery Dance", "Final Gambit", "Fire Blast", "Fire Fang", "Fire Lash", "Fire Pledge", "Fire Punch", "Fire Spin", "First Impression", "Fissure", "Flail", "Flame Burst", "Flame Charge", "Flame Wheel", "Flamethrower", "Flare Blitz", "Flash", "Flash Cannon", "Flatter", "Fleur Cannon", "Fling", "Floral Healing", "Flower Shield", "Fly", "Flying Press", "Focus Blast", "Focus Energy", "Focus Punch", "Follow Me", "Force Palm", "Foresight", "Forest’s Curse", "Foul Play", "Freeze Shock", "Freeze-Dry", "Frenzy Plant", "Frost Breath", "Frustration", "Fury Attack", "Fury Cutter", "Fury Swipes", "Fusion Bolt", "Fusion Flare", "Future Sight", "Gastro Acid", "Gear Grind", "Gear Up", "Genesis Supernova", "Geomancy", "Giga Drain", "Giga Impact", "Gigavolt Havoc", "Gigavolt Havoc", "Glaciate", "Glare", "Grass Knot", "Grass Pledge", "Grass Whistle", "Grassy Terrain", "Gravity", "Growl", "Growth", "Grudge", "Guard Split", "Guard Swap", "Guardian of Alola", "Guillotine", "Gunk Shot", "Gust", "Gyro Ball", "Hail", "Hammer Arm", "Happy Hour", "Harden", "Haze", "Head Charge", "Head Smash", "Headbutt", "Heal Bell", "Heal Block", "Heal Order", "Heal Pulse", "Healing Wish", "Heart Stamp", "Heart Swap", "Heat Crash", "Heat Wave", "Heavy Slam", "Helping Hand", "Hex", "Hidden Power", "High Horsepower", "High Jump Kick", "Hold Back", "Hold Hands", "Hone Claws", "Horn Attack", "Horn Drill", "Horn Leech", "Howl", "Hurricane", "Hydro Cannon", "Hydro Pump", "Hydro Vortex", "Hydro Vortex", "Hyper Beam", "Hyper Fang", "Hyper Voice", "Hyperspace Fury", "Hyperspace Hole", "Hypnosis", "Ice Ball", "Ice Beam", "Ice Burn", "Ice Fang", "Ice Hammer", "Ice Punch", "Ice Shard", "Icicle Crash", "Icicle Spear", "Icy Wind", "Imprison", "Incinerate", "Inferno", "Inferno Overdrive", "Inferno Overdrive", "Infestation", "Ingrain", "Instruct", "Ion Deluge", "Iron Defense", "Iron Head", "Iron Tail", "Judgment", "Jump Kick", "Karate Chop", "Kinesis", "King’s Shield", "Knock Off", "Land’s Wrath", "Laser Focus", "Last Resort", "Lava Plume", "Leaf Blade", "Leaf Storm", "Leaf Tornado", "Leafage", "Leech Life", "Leech Seed", "Leer", "Lick", "Light of Ruin", "Light Screen", "Liquidation", "Lock-On", "Lovely Kiss", "Low Kick", "Low Sweep", "Lucky Chant", "Lunar Dance", "Lunge", "Luster Purge", "Mach Punch", "Magic Coat", "Magic Room", "Magical Leaf", "Magma Storm", "Magnet Bomb", "Magnet Rise", "Magnetic Flux", "Magnitude", "Malicious Moonsault", "Mat Block", "Me First", "Mean Look", "Meditate", "Mega Drain", "Mega Kick", "Mega Punch", "Megahorn", "Memento", "Metal Burst", "Metal Claw", "Metal Sound", "Meteor Mash", "Metronome", "Milk Drink", "Mimic", "Mind Reader", "Minimize", "Miracle Eye", "Mirror Coat", "Mirror Move", "Mirror Shot", "Mist", "Mist Ball", "Misty Terrain", "Moonblast", "Moongeist Beam", "Moonlight", "Morning Sun", "Mud Bomb", "Mud Shot", "Mud Sport", "Mud-Slap", "Muddy Water", "Multi-Attack", "Mystical Fire", "Nasty Plot", "Natural Gift", "Nature Power", "Nature’s Madness", "Needle Arm", "Never-Ending Nightmare", "Never-Ending Nightmare", "Night Daze", "Night Shade", "Night Slash", "Nightmare", "Noble Roar", "Nuzzle", "Oblivion Wing", "Oceanic Operetta", "Octazooka", "Odor Sleuth", "Ominous Wind", "Origin Pulse", "Outrage", "Overheat", "Pain Split", "Parabolic Charge", "Parting Shot", "Pay Day", "Payback", "Peck", "Perish Song", "Petal Blizzard", "Petal Dance", "Phantom Force", "Pin Missile", "Play Nice", "Play Rough", "Pluck", "Poison Fang", "Poison Gas", "Poison Jab", "Poison Powder", "Poison Sting", "Poison Tail", "Pollen Puff", "Pound", "Powder", "Powder Snow", "Power Gem", "Power Split", "Power Swap", "Power Trick", "Power Trip", "Power Whip", "Power-Up Punch", "Precipice Blades", "Present", "Prismatic Laser", "Protect", "Psybeam", "Psych Up", "Psychic", "Psychic Fangs", "Psychic Terrain", "Psycho Boost", "Psycho Cut", "Psycho Shift", "Psyshock", "Psystrike", "Psywave", "Pulverizing Pancake", "Punishment", "Purify", "Pursuit", "Quash", "Quick Attack", "Quick Guard", "Quiver Dance", "Rage", "Rage Powder", "Rain Dance", "Rapid Spin", "Razor Leaf", "Razor Shell", "Razor Wind", "Recover", "Recycle", "Reflect", "Reflect Type", "Refresh", "Relic Song", "Rest", "Retaliate", "Return", "Revelation Dance", "Revenge", "Reversal", "Roar", "Roar of Time", "Rock Blast", "Rock Climb", "Rock Polish", "Rock Slide", "Rock Smash", "Rock Throw", "Rock Tomb", "Rock Wrecker", "Role Play", "Rolling Kick", "Rollout", "Roost", "Rototiller", "Round", "Sacred Fire", "Sacred Sword", "Safeguard", "Sand Attack", "Sand Tomb", "Sandstorm", "Savage Spin-Out", "Savage Spin-Out", "Scald", "Scary Face", "Scratch", "Screech", "Searing Shot", "Secret Power", "Secret Sword", "Seed Bomb", "Seed Flare", "Seismic Toss", "Self-Destruct", "Shadow Ball", "Shadow Bone", "Shadow Claw", "Shadow Force", "Shadow Punch", "Shadow Sneak", "Sharpen", "Shattered Psyche", "Shattered Psyche", "Sheer Cold", "Shell Smash", "Shell Trap", "Shift Gear", "Shock Wave", "Shore Up", "Signal Beam", "Silver Wind", "Simple Beam", "Sing", "Sinister Arrow Raid", "Sketch", "Skill Swap", "Skull Bash", "Sky Attack", "Sky Drop", "Sky Uppercut", "Slack Off", "Slam", "Slash", "Sleep Powder", "Sleep Talk", "Sludge", "Sludge Bomb", "Sludge Wave", "Smack Down", "Smart Strike", "Smelling Salts", "Smog", "Smokescreen", "Snarl", "Snatch", "Snore", "Soak", "Soft-Boiled", "Solar Beam", "Solar Blade", "Sonic Boom", "Soul-Stealing 7-Star Strike", "Spacial Rend", "Spark", "Sparkling Aria", "Spectral Thief", "Speed Swap", "Spider Web", "Spike Cannon", "Spikes", "Spiky Shield", "Spirit Shackle", "Spit Up", "Spite", "Splash", "Spore", "Spotlight", "Stealth Rock", "Steam Eruption", "Steamroller", "Steel Wing", "Sticky Web", "Stockpile", "Stoked Sparksurfer", "Stomp", "Stomping Tantrum", "Stone Edge", "Stored Power", "Storm Throw", "Strength", "Strength Sap", "String Shot", "Struggle", "Struggle Bug", "Stun Spore", "Submission", "Substitute", "Subzero Slammer", "Subzero Slammer", "Sucker Punch", "Sunny Day", "Sunsteel Strike", "Super Fang", "Superpower", "Supersonic", "Supersonic Skystrike", "Supersonic Skystrike", "Surf", "Swagger", "Swallow", "Sweet Kiss", "Sweet Scent", "Swift", "Switcheroo", "Swords Dance", "Synchronoise", "Synthesis", "Tackle", "Tail Glow", "Tail Slap", "Tail Whip", "Tailwind", "Take Down", "Taunt", "Tearful Look", "Techno Blast", "Tectonic Rage", "Tectonic Rage", "Teeter Dance", "Telekinesis", "Teleport", "Thief", "Thousand Arrows", "Thousand Waves", "Thrash", "Throat Chop", "Thunder", "Thunder Fang", "Thunder Punch", "Thunder Shock", "Thunder Wave", "Thunderbolt", "Tickle", "Topsy-Turvy", "Torment", "Toxic", "Toxic Spikes", "Toxic Thread", "Transform", "Tri Attack", "Trick", "Trick Room", "Trick-or-Treat", "Triple Kick", "Trop Kick", "Trump Card", "Twineedle", "Twinkle Tackle", "Twinkle Tackle", "Twister", "U-turn", "Uproar", "V-create", "Vacuum Wave", "Venom Drench", "Venoshock", "Vice Grip", "Vine Whip", "Vital Throw", "Volt Switch", "Volt Tackle", "Wake-Up Slap", "Water Gun", "Water Pledge", "Water Pulse", "Water Shuriken", "Water Sport", "Water Spout", "Waterfall", "Weather Ball", "Whirlpool", "Whirlwind", "Wide Guard", "Wild Charge", "Will-O-Wisp", "Wing Attack", "Wish", "Withdraw", "Wonder Room", "Wood Hammer", "Work Up", "Worry Seed", "Wrap", "Wring Out", "X-Scissor", "Yawn", "Zap Cannon", "Zen Headbutt", "Zing Zap", "10,000,000 Volt Thunderbolt"};
static char *natures[] = {"Hardy", "Lonely", "Brave", "Adamant", "Naughty", "Bold", "Docile", "Relaxed", "Impish", "Lax", "Timid", "Hasty", "Serious", "Jolly", "Naive", "Modest", "Mild", "Quiet", "Bashful", "Rash", "Calm", "Gentle", "Sassy", "Careful", "Quirky"};
static char *items[] = {"None", "Master Ball", "Ultra Ball", "Great Ball", "Poke Ball", "Safari Ball", "Net Ball", "Dive Ball", "Nest Ball", "Repeat Ball", "Timer Ball", "Luxury Ball", "Premier Ball", "Dusk Ball", "Heal Ball", "Quick Ball", "Cherish Ball", "Potion", "Antidote", "Burn Heal", "Ice Heal", "Awakening", "Paralyze Heal", "Full Restore", "Max Potion", "Hyper Potion", "Super Potion", "Full Heal", "Revive", "Max Revive", "Fresh Water", "Soda Pop", "Lemonade", "Moomoo Milk", "Energy Powder", "Energy Root", "Heal Powder", "Revival Herb", "Ether", "Max Ether", "Elixir", "Max Elixir", "Lava Cookie", "Berry Juice", "Sacred Ash", "HP Up", "Protein", "Iron", "Carbos", "Calcium", "Rare Candy", "PP Up", "Zinc", "PP Max", "Old Gateau", "Guard Spec.", "Dire Hit", "X Attack", "X Defense", "X Speed", "X Accuracy", "X Sp. Atk", "X Sp. Def", "Poké Doll", "Fluffy Tail", "Blue Flute", "Yellow Flute", "Red Flute", "Black Flute", "White Flute", "Shoal Salt", "Shoal Shell", "Red Shard", "Blue Shard", "Yellow Shard", "Green Shard", "Super Repel", "Max Repel", "Escape Rope", "Repel", "Sun Stone", "Moon Stone", "Fire Stone", "Thunder Stone", "Water Stone", "Leaf Stone", "Tiny Mushroom", "Big Mushroom", "Pearl", "Big Pearl", "Stardust", "Star Piece", "Nugget", "Heart Scale", "Honey", "Growth Mulch", "Damp Mulch", "Stable Mulch", "Gooey Mulch", "Root Fossil", "Claw Fossil", "Helix Fossil", "Dome Fossil", "Old Amber", "Armor Fossil", "Skull Fossil", "Rare Bone", "Shiny Stone", "Dusk Stone", "Dawn Stone", "Oval Stone", "Odd Keystone", "Griseous Orb", "???", "???", "???", "Douse Drive", "Shock Drive", "Burn Drive", "Chill Drive", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "Sweet Heart", "Adamant Orb", "Lustrous Orb", "Greet Mail", "Favored Mail", "RSVP Mail", "Thanks Mail", "Inquiry Mail", "Like Mail", "Reply Mail", "Bridge Mail S", "Bridge Mail D", "Bridge Mail T", "Bridge Mail V", "Bridge Mail M", "Cheri Berry", "Chesto Berry", "Pecha Berry", "Rawst Berry", "Aspear Berry", "Leppa Berry", "Oran Berry", "Persim Berry", "Lum Berry", "Sitrus Berry", "Figy Berry", "Wiki Berry", "Mago Berry", "Aguav Berry", "Iapapa Berry", "Razz Berry", "Bluk Berry", "Nanab Berry", "Wepear Berry", "Pinap Berry", "Pomeg Berry", "Kelpsy Berry", "Qualot Berry", "Hondew Berry", "Grepa Berry", "Tamato Berry", "Cornn Berry", "Magost Berry", "Rabuta Berry", "Nomel Berry", "Spelon Berry", "Pamtre Berry", "Watmel Berry", "Durin Berry", "Belue Berry", "Occa Berry", "Passho Berry", "Wacan Berry", "Rindo Berry", "Yache Berry", "Chople Berry", "Kebia Berry", "Shuca Berry", "Coba Berry", "Payapa Berry", "Tanga Berry", "Charti Berry", "Kasib Berry", "Haban Berry", "Colbur Berry", "Babiri Berry", "Chilan Berry", "Liechi Berry", "Ganlon Berry", "Salac Berry", "Petaya Berry", "Apicot Berry", "Lansat Berry", "Starf Berry", "Enigma Berry", "Micle Berry", "Custap Berry", "Jaboca Berry", "Rowap Berry", "Bright Powder", "White Herb", "Macho Brace", "Exp. Share", "Quick Claw", "Soothe Bell", "Mental Herb", "Choice Band", "King’s Rock", "Silver Powder", "Amulet Coin", "Cleanse Tag", "Soul Dew", "Deep Sea Tooth", "Deep Sea Scale", "Smoke Ball", "Everstone", "Focus Band", "Lucky Egg", "Scope Lens", "Metal Coat", "Leftovers", "Dragon Scale", "Light Ball", "Soft Sand", "Hard Stone", "Miracle Seed", "Black Glasses", "Black Belt", "Magnet", "Mystic Water", "Sharp Beak", "Poison Barb", "Never-Melt Ice", "Spell Tag", "Twisted Spoon", "Charcoal", "Dragon Fang", "Silk Scarf", "Up-Grade", "Shell Bell", "Sea Incense", "Lax Incense", "Lucky Punch", "Metal Powder", "Thick Club", "Stick", "Red Scarf", "Blue Scarf", "Pink Scarf", "Green Scarf", "Yellow Scarf", "Wide Lens", "Muscle Band", "Wise Glasses", "Expert Belt", "Light Clay", "Life Orb", "Power Herb", "Toxic Orb", "Flame Orb", "Quick Powder", "Focus Sash", "Zoom Lens", "Metronome", "Iron Ball", "Lagging Tail", "Destiny Knot", "Black Sludge", "Icy Rock", "Smooth Rock", "Heat Rock", "Damp Rock", "Grip Claw", "Choice Scarf", "Sticky Barb", "Power Bracer", "Power Belt", "Power Lens", "Power Band", "Power Anklet", "Power Weight", "Shed Shell", "Big Root", "Choice Specs", "Flame Plate", "Splash Plate", "Zap Plate", "Meadow Plate", "Icicle Plate", "Fist Plate", "Toxic Plate", "Earth Plate", "Sky Plate", "Mind Plate", "Insect Plate", "Stone Plate", "Spooky Plate", "Draco Plate", "Dread Plate", "Iron Plate", "Odd Incense", "Rock Incense", "Full Incense", "Wave Incense", "Rose Incense", "Luck Incense", "Pure Incense", "Protector", "Electirizer", "Magmarizer", "Dubious Disc", "Reaper Cloth", "Razor Claw", "Razor Fang", "TM01", "TM02", "TM03", "TM04", "TM05", "TM06", "TM07", "TM08", "TM09", "TM10", "TM11", "TM12", "TM13", "TM14", "TM15", "TM16", "TM17", "TM18", "TM19", "TM20", "TM21", "TM22", "TM23", "TM24", "TM25", "TM26", "TM27", "TM28", "TM29", "TM30", "TM31", "TM32", "TM33", "TM34", "TM35", "TM36", "TM37", "TM38", "TM39", "TM40", "TM41", "TM42", "TM43", "TM44", "TM45", "TM46", "TM47", "TM48", "TM49", "TM50", "TM51", "TM52", "TM53", "TM54", "TM55", "TM56", "TM57", "TM58", "TM59", "TM60", "TM61", "TM62", "TM63", "TM64", "TM65", "TM66", "TM67", "TM68", "TM69", "TM70", "TM71", "TM72", "TM73", "TM74", "TM75", "TM76", "TM77", "TM78", "TM79", "TM80", "TM81", "TM82", "TM83", "TM84", "TM85", "TM86", "TM87", "TM88", "TM89", "TM90", "TM91", "TM92", "HM01", "HM02", "HM03", "HM04", "HM05", "HM06", "???", "???", "Explorer Kit", "Loot Sack", "Rule Book", "Poké Radar", "Point Card", "Journal", "Seal Case", "Fashion Case", "Seal Bag", "Pal Pad", "Works Key", "Old Charm", "Galactic Key", "Red Chain", "Town Map", "Vs. Seeker", "Coin Case", "Old Rod", "Good Rod", "Super Rod", "Sprayduck", "Poffin Case", "Bike", "Suite Key", "Oak’s Letter", "Lunar Wing", "Member Card", "Azure Flute", "S.S. Ticket", "Contest Pass", "Magma Stone", "Parcel", "Coupon 1", "Coupon 2", "Coupon 3", "Storage Key", "Secret Potion", "Vs. Recorder", "Gracidea", "Secret Key", "Apricorn Box", "Unown Report", "Berry Pots", "Dowsing Machine", "Blue Card", "Slowpoke Tail", "Clear Bell", "Card Key", "Basement Key", "Squirt Bottle", "Red Scale", "Lost Item", "Pass", "Machine Part", "Silver Wing", "Rainbow Wing", "Mystery Egg", "Red Apricorn", "Blue Apricorn", "Yellow Apricorn", "Green Apricorn", "Pink Apricorn", "White Apricorn", "Black Apricorn", "Fast Ball", "Level Ball", "Lure Ball", "Heavy Ball", "Love Ball", "Friend Ball", "Moon Ball", "Sport Ball", "Park Ball", "Photo Album", "GB Sounds", "Tidal Bell", "Rage Candy Bar", "Data Card 01", "Data Card 02", "Data Card 03", "Data Card 04", "Data Card 05", "Data Card 06", "Data Card 07", "Data Card 08", "Data Card 09", "Data Card 10", "Data Card 11", "Data Card 12", "Data Card 13", "Data Card 14", "Data Card 15", "Data Card 16", "Data Card 17", "Data Card 18", "Data Card 19", "Data Card 20", "Data Card 21", "Data Card 22", "Data Card 23", "Data Card 24", "Data Card 25", "Data Card 26", "Data Card 27", "Jade Orb", "Lock Capsule", "Red Orb", "Blue Orb", "Enigma Stone", "Prism Scale", "Eviolite", "Float Stone", "Rocky Helmet", "Air Balloon", "Red Card", "Ring Target", "Binding Band", "Absorb Bulb", "Cell Battery", "Eject Button", "Fire Gem", "Water Gem", "Electric Gem", "Grass Gem", "Ice Gem", "Fighting Gem", "Poison Gem", "Ground Gem", "Flying Gem", "Psychic Gem", "Bug Gem", "Rock Gem", "Ghost Gem", "Dragon Gem", "Dark Gem", "Steel Gem", "Normal Gem", "Health Wing", "Muscle Wing", "Resist Wing", "Genius Wing", "Clever Wing", "Swift Wing", "Pretty Wing", "Cover Fossil", "Plume Fossil", "Liberty Pass", "Pass Orb", "Dream Ball", "Poké Toy", "Prop Case", "Dragon Skull", "Balm Mushroom", "Big Nugget", "Pearl String", "Comet Shard", "Relic Copper", "Relic Silver", "Relic Gold", "Relic Vase", "Relic Band", "Relic Statue", "Relic Crown", "Casteliacone", "Dire Hit 2", "X Speed 2", "X Sp. Atk 2", "X Sp. Def 2", "X Defense 2", "X Attack 2", "X Accuracy 2", "X Speed 3", "X Sp. Atk 3", "X Sp. Def 3", "X Defense 3", "X Attack 3", "X Accuracy 3", "X Speed 6", "X Sp. Atk 6", "X Sp. Def 6", "X Defense 6", "X Attack 6", "X Accuracy 6", "Ability Urge", "Item Drop", "Item Urge", "Reset Urge", "Dire Hit 3", "Light Stone", "Dark Stone", "TM93", "TM94", "TM95", "Xtransceiver", "???", "Gram 1", "Gram 2", "Gram 3", "Xtransceiver", "Medal Box", "DNA Splicers", "DNA Splicers", "Permit", "Oval Charm", "Shiny Charm", "Plasma Card", "Grubby Hanky", "Colress Machine", "Dropped Item", "Dropped Item", "Reveal Glass", "Weakness Policy", "Assault Vest", "Holo Caster", "Prof’s Letter", "Roller Skates", "Pixie Plate", "Ability Capsule", "Whipped Dream", "Sachet", "Luminous Moss", "Snowball", "Safety Goggles", "Poké Flute", "Rich Mulch", "Surprise Mulch", "Boost Mulch", "Amaze Mulch", "Gengarite", "Gardevoirite", "Ampharosite", "Venusaurite", "Charizardite X", "Blastoisinite", "Mewtwonite X", "Mewtwonite Y", "Blazikenite", "Medichamite", "Houndoominite", "Aggronite", "Banettite", "Tyranitarite", "Scizorite", "Pinsirite", "Aerodactylite", "Lucarionite", "Abomasite", "Kangaskhanite", "Gyaradosite", "Absolite", "Charizardite Y", "Alakazite", "Heracronite", "Mawilite", "Manectite", "Garchompite", "Latiasite", "Latiosite", "Roseli Berry", "Kee Berry", "Maranga Berry", "Sprinklotad", "TM96", "TM97", "TM98", "TM99", "TM100", "Power Plant Pass", "Mega Ring", "Intriguing Stone", "Common Stone", "Discount Coupon", "Elevator Key", "TMV Pass", "Honor of Kalos", "Adventure Rules", "Strange Souvenir", "Lens Case", "Makeup Bag", "Travel Trunk", "Lumiose Galette", "Shalour Sable", "Jaw Fossil", "Sail Fossil", "Looker Ticket", "Bike", "Holo Caster", "Fairy Gem", "Mega Charm", "Mega Glove", "Mach Bike", "Acro Bike", "Wailmer Pail", "Devon Parts", "Soot Sack", "Basement Key", "Pokéblock Kit", "Letter", "Eon Ticket", "Scanner", "Go-Goggles", "Meteorite", "Key to Room 1", "Key to Room 2", "Key to Room 4", "Key to Room 6", "Storage Key", "Devon Scope", "S.S. Ticket", "HM07", "Devon Scuba Gear", "Contest Costume", "Contest Costume", "Magma Suit", "Aqua Suit", "Pair of Tickets", "Mega Bracelet", "Mega Pendant", "Mega Glasses", "Mega Anchor", "Mega Stickpin", "Mega Tiara", "Mega Anklet", "Meteorite", "Swampertite", "Sceptilite", "Sablenite", "Altarianite", "Galladite", "Audinite", "Metagrossite", "Sharpedonite", "Slowbronite", "Steelixite", "Pidgeotite", "Glalitite", "Diancite", "Prison Bottle", "Mega Cuff", "Cameruptite", "Lopunnite", "Salamencite", "Beedrillite", "Meteorite", "Meteorite", "Key Stone", "Meteorite Shard", "Eon Flute", "Normalium Z", "Firium Z", "Waterium Z", "Electrium Z", "Grassium Z", "Icium Z", "Fightinium Z", "Poisonium Z", "Groundium Z", "Flyinium Z", "Psychium Z", "Buginium Z", "Rockium Z", "Ghostium Z", "Dragonium Z", "Darkinium Z", "Steelium Z", "Fairium Z", "Pikanium Z", "Bottle Cap", "Gold Bottle Cap", "Z-Ring", "Decidium Z", "Incinium Z", "Primarium Z", "Tapunium Z", "Marshadium Z", "Aloraichium Z", "Snorlium Z", "Eevium Z", "Mewnium Z", "Normalium Z", "Firium Z", "Waterium Z", "Electrium Z", "Grassium Z", "Icium Z", "Fightinium Z", "Poisonium Z", "Groundium Z", "Flyinium Z", "Psychium Z", "Buginium Z", "Rockium Z", "Ghostium Z", "Dragonium Z", "Darkinium Z", "Steelium Z", "Fairium Z", "Pikanium Z", "Decidium Z", "Incinium Z", "Primarium Z", "Tapunium Z", "Marshadium Z", "Aloraichium Z", "Snorlium Z", "Eevium Z", "Mewnium Z", "Pikashunium Z", "Pikashunium Z", "???", "???", "???", "???", "Forage Bag", "Fishing Rod", "Professor’s Mask", "Festival Ticket", "Sparkling Stone", "Adrenaline Orb", "Zygarde Cube", "???", "Ice Stone", "Ride Pager", "Beast Ball", "Big Malasada", "Red Nectar", "Yellow Nectar", "Pink Nectar", "Purple Nectar", "Sun Flute", "Moon Flute", "???", "Enigmatic Card", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "Terrain Extender", "Protective Pads", "Electric Seed", "Psychic Seed", "Misty Seed", "Grassy Seed", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "Fighting Memory", "Flying Memory", "Poison Memory", "Ground Memory", "Rock Memory", "Bug Memory", "Ghost Memory", "Steel Memory", "Fire Memory", "Water Memory", "Grass Memory", "Electric Memory", "Psychic Memory", "Ice Memory", "Dragon Memory", "Dark Memory", "Fairy Memory"};
static char *itemsSorted[] = {"None", "Ability Capsule", "Ability Urge", "Abomasite", "Absolite", "Absorb Bulb", "Acro Bike", "Adamant Orb", "Adrenaline Orb", "Adventure Rules", "Aerodactylite", "Aggronite", "Aguav Berry", "Air Balloon", "Alakazite", "Aloraichium Z", "Aloraichium Z", "Altarianite", "Amaze Mulch", "Ampharosite", "Amulet Coin", "Antidote", "Apicot Berry", "Apricorn Box", "Aqua Suit", "Armor Fossil", "Aspear Berry", "Assault Vest", "Audinite", "Awakening", "Azure Flute", "Babiri Berry", "Balm Mushroom", "Banettite", "Basement Key", "Basement Key", "Beast Ball", "Beedrillite", "Belue Berry", "Berry Juice", "Berry Pots", "Big Malasada", "Big Mushroom", "Big Nugget", "Big Pearl", "Big Root", "Bike", "Bike", "Binding Band", "Black Apricorn", "Black Belt", "Black Flute", "Black Glasses", "Black Sludge", "Blastoisinite", "Blazikenite", "Blue Apricorn", "Blue Card", "Blue Flute", "Blue Orb", "Blue Scarf", "Blue Shard", "Bluk Berry", "Boost Mulch", "Bottle Cap", "Bridge Mail D", "Bridge Mail M", "Bridge Mail S", "Bridge Mail T", "Bridge Mail V", "Bright Powder", "Bug Gem", "Bug Memory", "Buginium Z", "Buginium Z", "Burn Drive", "Burn Heal", "Calcium", "Cameruptite", "Carbos", "Card Key", "Casteliacone", "Cell Battery", "Charcoal", "Charizardite X", "Charizardite Y", "Charti Berry", "Cheri Berry", "Cherish Ball", "Chesto Berry", "Chilan Berry", "Chill Drive", "Choice Band", "Choice Scarf", "Choice Specs", "Chople Berry", "Claw Fossil", "Cleanse Tag", "Clear Bell", "Clever Wing", "Coba Berry", "Coin Case", "Colbur Berry", "Colress Machine", "Comet Shard", "Common Stone", "Contest Costume", "Contest Costume", "Contest Pass", "Cornn Berry", "Coupon 1", "Coupon 2", "Coupon 3", "Cover Fossil", "Custap Berry", "Damp Mulch", "Damp Rock", "Dark Gem", "Dark Memory", "Dark Stone", "Darkinium Z", "Darkinium Z", "Data Card 01", "Data Card 02", "Data Card 03", "Data Card 04", "Data Card 05", "Data Card 06", "Data Card 07", "Data Card 08", "Data Card 09", "Data Card 10", "Data Card 11", "Data Card 12", "Data Card 13", "Data Card 14", "Data Card 15", "Data Card 16", "Data Card 17", "Data Card 18", "Data Card 19", "Data Card 20", "Data Card 21", "Data Card 22", "Data Card 23", "Data Card 24", "Data Card 25", "Data Card 26", "Data Card 27", "Dawn Stone", "Decidium Z", "Decidium Z", "Deep Sea Scale", "Deep Sea Tooth", "Destiny Knot", "Devon Parts", "Devon Scope", "Devon Scuba Gear", "Diancite", "Dire Hit", "Dire Hit 2", "Dire Hit 3", "Discount Coupon", "Dive Ball", "DNA Splicers", "DNA Splicers", "Dome Fossil", "Douse Drive", "Dowsing Machine", "Draco Plate", "Dragon Fang", "Dragon Gem", "Dragon Memory", "Dragon Scale", "Dragon Skull", "Dragonium Z", "Dragonium Z", "Dread Plate", "Dream Ball", "Dropped Item", "Dropped Item", "Dubious Disc", "Durin Berry", "Dusk Ball", "Dusk Stone", "Earth Plate", "Eevium Z", "Eevium Z", "Eject Button", "Electirizer", "Electric Gem", "Electric Memory", "Electric Seed", "Electrium Z", "Electrium Z", "Elevator Key", "Elixir", "Energy Powder", "Energy Root", "Enigma Berry", "Enigma Stone", "Enigmatic Card", "Eon Flute", "Eon Ticket", "Escape Rope", "Ether", "Everstone", "Eviolite", "Exp. Share", "Expert Belt", "Explorer Kit", "Fairium Z", "Fairium Z", "Fairy Gem", "Fairy Memory", "Fashion Case", "Fast Ball", "Favored Mail", "Festival Ticket", "Fighting Gem", "Fighting Memory", "Fightinium Z", "Fightinium Z", "Figy Berry", "Fire Gem", "Fire Memory", "Fire Stone", "Firium Z", "Firium Z", "Fishing Rod", "Fist Plate", "Flame Orb", "Flame Plate", "Float Stone", "Fluffy Tail", "Flying Gem", "Flying Memory", "Flyinium Z", "Flyinium Z", "Focus Band", "Focus Sash", "Forage Bag", "Fresh Water", "Friend Ball", "Full Heal", "Full Incense", "Full Restore", "Galactic Key", "Galladite", "Ganlon Berry", "Garchompite", "Gardevoirite", "GB Sounds", "Gengarite", "Genius Wing", "Ghost Gem", "Ghost Memory", "Ghostium Z", "Ghostium Z", "Glalitite", "Go-Goggles", "Gold Bottle Cap", "Good Rod", "Gooey Mulch", "Gracidea", "Gram 1", "Gram 2", "Gram 3", "Grass Gem", "Grass Memory", "Grassium Z", "Grassium Z", "Grassy Seed", "Great Ball", "Green Apricorn", "Green Scarf", "Green Shard", "Greet Mail", "Grepa Berry", "Grip Claw", "Griseous Orb", "Ground Gem", "Ground Memory", "Groundium Z", "Groundium Z", "Growth Mulch", "Grubby Hanky", "Guard Spec.", "Gyaradosite", "Haban Berry", "Hard Stone", "Heal Ball", "Heal Powder", "Health Wing", "Heart Scale", "Heat Rock", "Heavy Ball", "Helix Fossil", "Heracronite", "HM01", "HM02", "HM03", "HM04", "HM05", "HM06", "HM07", "Holo Caster", "Holo Caster", "Hondew Berry", "Honey", "Honor of Kalos", "Houndoominite", "HP Up", "Hyper Potion", "Iapapa Berry", "Ice Gem", "Ice Heal", "Ice Memory", "Ice Stone", "Icicle Plate", "Icium Z", "Icium Z", "Icy Rock", "Incinium Z", "Incinium Z", "Inquiry Mail", "Insect Plate", "Intriguing Stone", "Iron", "Iron Ball", "Iron Plate", "Item Drop", "Item Urge", "Jaboca Berry", "Jade Orb", "Jaw Fossil", "Journal", "Kangaskhanite", "Kasib Berry", "Kebia Berry", "Kee Berry", "Kelpsy Berry", "Key Stone", "Key to Room 1", "Key to Room 2", "Key to Room 4", "Key to Room 6", "King’s Rock", "Lagging Tail", "Lansat Berry", "Latiasite", "Latiosite", "Lava Cookie", "Lax Incense", "Leaf Stone", "Leftovers", "Lemonade", "Lens Case", "Leppa Berry", "Letter", "Level Ball", "Liberty Pass", "Liechi Berry", "Life Orb", "Light Ball", "Light Clay", "Light Stone", "Like Mail", "Lock Capsule", "Looker Ticket", "Loot Sack", "Lopunnite", "Lost Item", "Love Ball", "Lucarionite", "Luck Incense", "Lucky Egg", "Lucky Punch", "Lum Berry", "Luminous Moss", "Lumiose Galette", "Lunar Wing", "Lure Ball", "Lustrous Orb", "Luxury Ball", "Mach Bike", "Machine Part", "Macho Brace", "Magma Stone", "Magma Suit", "Magmarizer", "Magnet", "Mago Berry", "Magost Berry", "Makeup Bag", "Manectite", "Maranga Berry", "Marshadium Z", "Marshadium Z", "Master Ball", "Mawilite", "Max Elixir", "Max Ether", "Max Potion", "Max Repel", "Max Revive", "Meadow Plate", "Medal Box", "Medichamite", "Mega Anchor", "Mega Anklet", "Mega Bracelet", "Mega Charm", "Mega Cuff", "Mega Glasses", "Mega Glove", "Mega Pendant", "Mega Ring", "Mega Stickpin", "Mega Tiara", "Member Card", "Mental Herb", "Metagrossite", "Metal Coat", "Metal Powder", "Meteorite", "Meteorite", "Meteorite", "Meteorite", "Meteorite Shard", "Metronome", "Mewnium Z", "Mewnium Z", "Mewtwonite X", "Mewtwonite Y", "Micle Berry", "Mind Plate", "Miracle Seed", "Misty Seed", "Moomoo Milk", "Moon Ball", "Moon Flute", "Moon Stone", "Muscle Band", "Muscle Wing", "Mystery Egg", "Mystic Water", "Nanab Berry", "Nest Ball", "Net Ball", "Never-Melt Ice", "Nomel Berry", "Normal Gem", "Normalium Z", "Normalium Z", "Nugget", "Oak’s Letter", "Occa Berry", "Odd Incense", "Odd Keystone", "Old Amber", "Old Charm", "Old Gateau", "Old Rod", "Oran Berry", "Oval Charm", "Oval Stone", "Pair of Tickets", "Pal Pad", "Pamtre Berry", "Paralyze Heal", "Parcel", "Park Ball", "Pass", "Pass Orb", "Passho Berry", "Payapa Berry", "Pearl", "Pearl String", "Pecha Berry", "Permit", "Persim Berry", "Petaya Berry", "Photo Album", "Pidgeotite", "Pikanium Z", "Pikanium Z", "Pikashunium Z", "Pikashunium Z", "Pinap Berry", "Pink Apricorn", "Pink Nectar", "Pink Scarf", "Pinsirite", "Pixie Plate", "Plasma Card", "Plume Fossil", "Poffin Case", "Point Card", "Poison Barb", "Poison Gem", "Poison Memory", "Poisonium Z", "Poisonium Z", "Poké Ball", "Poké Doll", "Poké Flute", "Poké Radar", "Poké Toy", "Pokéblock Kit", "Pomeg Berry", "Potion", "Power Anklet", "Power Band", "Power Belt", "Power Bracer", "Power Herb", "Power Lens", "Power Plant Pass", "Power Weight", "PP Max", "PP Up", "Premier Ball", "Pretty Wing", "Primarium Z", "Primarium Z", "Prism Scale", "Prison Bottle", "Prof’s Letter", "Professor’s Mask", "Prop Case", "Protective Pads", "Protector", "Protein", "Psychic Gem", "Psychic Memory", "Psychic Seed", "Psychium Z", "Psychium Z", "Pure Incense", "Purple Nectar", "Qualot Berry", "Quick Ball", "Quick Claw", "Quick Powder", "Rabuta Berry", "Rage Candy Bar", "Rainbow Wing", "Rare Bone", "Rare Candy", "Rawst Berry", "Razor Claw", "Razor Fang", "Razz Berry", "Reaper Cloth", "Red Apricorn", "Red Card", "Red Chain", "Red Flute", "Red Nectar", "Red Orb", "Red Scale", "Red Scarf", "Red Shard", "Relic Band", "Relic Copper", "Relic Crown", "Relic Gold", "Relic Silver", "Relic Statue", "Relic Vase", "Repeat Ball", "Repel", "Reply Mail", "Reset Urge", "Resist Wing", "Reveal Glass", "Revival Herb", "Revive", "Rich Mulch", "Ride Pager", "Rindo Berry", "Ring Target", "Rock Gem", "Rock Incense", "Rock Memory", "Rockium Z", "Rockium Z", "Rocky Helmet", "Roller Skates", "Root Fossil", "Rose Incense", "Roseli Berry", "Rowap Berry", "RSVP Mail", "Rule Book", "S.S. Ticket", "S.S. Ticket", "Sablenite", "Sachet", "Sacred Ash", "Safari Ball", "Safety Goggles", "Sail Fossil", "Salac Berry", "Salamencite", "Scanner", "Sceptilite", "Scizorite", "Scope Lens", "Sea Incense", "Seal Bag", "Seal Case", "Secret Key", "Secret Potion", "Shalour Sable", "Sharp Beak", "Sharpedonite", "Shed Shell", "Shell Bell", "Shiny Charm", "Shiny Stone", "Shoal Salt", "Shoal Shell", "Shock Drive", "Shuca Berry", "Silk Scarf", "Silver Powder", "Silver Wing", "Sitrus Berry", "Skull Fossil", "Sky Plate", "Slowbronite", "Slowpoke Tail", "Smoke Ball", "Smooth Rock", "Snorlium Z", "Snorlium Z", "Snowball", "Soda Pop", "Soft Sand", "Soot Sack", "Soothe Bell", "Soul Dew", "Sparkling Stone", "Spell Tag", "Spelon Berry", "Splash Plate", "Spooky Plate", "Sport Ball", "Sprayduck", "Sprinklotad", "Squirt Bottle", "Stable Mulch", "Star Piece", "Stardust", "Starf Berry", "Steel Gem", "Steel Memory", "Steelium Z", "Steelium Z", "Steelixite", "Stick", "Sticky Barb", "Stone Plate", "Storage Key", "Storage Key", "Strange Souvenir", "Suite Key", "Sun Flute", "Sun Stone", "Super Potion", "Super Repel", "Super Rod", "Surprise Mulch", "Swampertite", "Sweet Heart", "Swift Wing", "Tamato Berry", "Tanga Berry", "Tapunium Z", "Tapunium Z", "Terrain Extender", "Thanks Mail", "Thick Club", "Thunder Stone", "Tidal Bell", "Timer Ball", "Tiny Mushroom", "TM01", "TM02", "TM03", "TM04", "TM05", "TM06", "TM07", "TM08", "TM09", "TM10", "TM100", "TM11", "TM12", "TM13", "TM14", "TM15", "TM16", "TM17", "TM18", "TM19", "TM20", "TM21", "TM22", "TM23", "TM24", "TM25", "TM26", "TM27", "TM28", "TM29", "TM30", "TM31", "TM32", "TM33", "TM34", "TM35", "TM36", "TM37", "TM38", "TM39", "TM40", "TM41", "TM42", "TM43", "TM44", "TM45", "TM46", "TM47", "TM48", "TM49", "TM50", "TM51", "TM52", "TM53", "TM54", "TM55", "TM56", "TM57", "TM58", "TM59", "TM60", "TM61", "TM62", "TM63", "TM64", "TM65", "TM66", "TM67", "TM68", "TM69", "TM70", "TM71", "TM72", "TM73", "TM74", "TM75", "TM76", "TM77", "TM78", "TM79", "TM80", "TM81", "TM82", "TM83", "TM84", "TM85", "TM86", "TM87", "TM88", "TM89", "TM90", "TM91", "TM92", "TM93", "TM94", "TM95", "TM96", "TM97", "TM98", "TM99", "TMV Pass", "Town Map", "Toxic Orb", "Toxic Plate", "Travel Trunk", "Twisted Spoon", "Tyranitarite", "Ultra Ball", "Unown Report", "Up-Grade", "Venusaurite", "Vs. Recorder", "Vs. Seeker", "Wacan Berry", "Wailmer Pail", "Water Gem", "Water Memory", "Water Stone", "Waterium Z", "Waterium Z", "Watmel Berry", "Wave Incense", "Weakness Policy", "Wepear Berry", "Whipped Dream", "White Apricorn", "White Flute", "White Herb", "Wide Lens", "Wiki Berry", "Wise Glasses", "Works Key", "X Accuracy", "X Accuracy 2", "X Accuracy 3", "X Accuracy 6", "X Attack", "X Attack 2", "X Attack 3", "X Attack 6", "X Defense", "X Defense 2", "X Defense 3", "X Defense 6", "X Sp. Atk", "X Sp. Atk 2", "X Sp. Atk 3", "X Sp. Atk 6", "X Sp. Def", "X Sp. Def 2", "X Sp. Def 3", "X Sp. Def 6", "X Speed", "X Speed 2", "X Speed 3", "X Speed 6", "Xtransceiver", "Xtransceiver", "Yache Berry", "Yellow Apricorn", "Yellow Flute", "Yellow Nectar", "Yellow Scarf", "Yellow Shard", "Z-Ring", "Zap Plate", "Zinc", "Zoom Lens", "Zygarde Cube", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???", "???"};
char *hpList[] = {"Fighting", "Flying", "Poison", "Ground", "Rock", "Bug", "Ghost", "Steel", "Fire", "Water", "Grass", "Electric", "Psychic", "Ice", "Dragon", "Dark", " ", " "};

void GUIElementsInit() {
	fontBold18 = sftd_load_font_file("romfs:/res/Bold.ttf");
	fontBold15 = sftd_load_font_file("romfs:/res/Bold.ttf");
	fontBold14 = sftd_load_font_file("romfs:/res/Bold.ttf");
	fontBold12 = sftd_load_font_file("romfs:/res/Bold.ttf");
	fontBold11 = sftd_load_font_file("romfs:/res/Bold.ttf");
	fontBold9 = sftd_load_font_file("romfs:/res/Bold.ttf");
	fontFixed = sftd_load_font_file("romfs:/res/VeraMono.ttf");
	warningTop = sfil_load_PNG_file("romfs:/res/Warning Top.png", SF2D_PLACE_RAM);
	warningBottom = sfil_load_PNG_file("romfs:/res/Warning Bottom.png", SF2D_PLACE_RAM);
	freezeMsg("Loading graphics...");
	
	init_font_cache();
	cleanTop = sfil_load_PNG_file("romfs:/res/Clean Top.png", SF2D_PLACE_RAM);
	mainMenuBottom = sfil_load_PNG_file("romfs:/res/Main Menu Bottom.png", SF2D_PLACE_RAM);
}

void GUIElementsSpecify(int game) {
	freezeMsg("Loading graphics...");
	alternativeSpritesSmall = sfil_load_PNG_file("/3ds/data/PKSM/additionalassets/alternative_icons_spritesheet.png", SF2D_PLACE_RAM);
	spritesSmall = sfil_load_PNG_file("/3ds/data/PKSM/additionalassets/pokemon_icons_spritesheet.png", SF2D_PLACE_RAM);
	balls = sfil_load_PNG_file("/3ds/data/PKSM/additionalassets/balls_spritesheet.png", SF2D_PLACE_RAM);
	settings = sfil_load_PNG_file("romfs:/res/Settings.png", SF2D_PLACE_RAM);
	
	if (game < 6) {
		editorBar = sfil_load_PNG_file("/3ds/data/PKSM/additionalassets/editor_bar.png", SF2D_PLACE_RAM);
		typesSheet = sfil_load_PNG_file("/3ds/data/PKSM/additionalassets/types_sheet.png", SF2D_PLACE_RAM);
		boxView = sfil_load_PNG_file("/3ds/data/PKSM/additionalassets/editor_bottom.png", SF2D_PLACE_RAM);
		back = sfil_load_PNG_file("/3ds/data/PKSM/additionalassets/back_button.png", SF2D_PLACE_RAM);
		
		dexBox = sfil_load_PNG_file("romfs:/res/Dex Box.png", SF2D_PLACE_RAM);
		dexN = sfil_load_PNG_file("romfs:/res/Dex N.png", SF2D_PLACE_RAM);
		hiddenPowerBG = sfil_load_PNG_file("romfs:/res/Hidden Power BG.png", SF2D_PLACE_RAM);
		hiddenPowerButton = sfil_load_PNG_file("romfs:/res/Hidden Power Button.png", SF2D_PLACE_RAM);
		selectBoxButton = sfil_load_PNG_file("romfs:/res/Select Box Button.png", SF2D_PLACE_RAM);
		selectBoxButton = sfil_load_PNG_file("romfs:/res/Select Box Button.png", SF2D_PLACE_RAM);
		ballsBG = sfil_load_PNG_file("romfs:/res/BallsBG.png", SF2D_PLACE_RAM);
		ballButton = sfil_load_PNG_file("romfs:/res/Ball Button.png", SF2D_PLACE_RAM);
		male = sfil_load_PNG_file("romfs:/res/Male.png", SF2D_PLACE_RAM);
		female = sfil_load_PNG_file("romfs:/res/Female.png", SF2D_PLACE_RAM);
		naturesButton = sfil_load_PNG_file("romfs:/res/Natures Button.png", SF2D_PLACE_RAM);
		naturestx = sfil_load_PNG_file("romfs:/res/Natures.png", SF2D_PLACE_RAM);
		movesBottom = sfil_load_PNG_file("romfs:/res/Moves Bottom.png", SF2D_PLACE_RAM);
		topMovesBG = sfil_load_PNG_file("romfs:/res/Top Moves.png", SF2D_PLACE_RAM);
		editorStatsBG = sfil_load_PNG_file("romfs:/res/Editor Stats.png", SF2D_PLACE_RAM);
		subArrow = sfil_load_PNG_file("romfs:/res/Sub Arrow.png", SF2D_PLACE_RAM);
		backgroundTop = sfil_load_PNG_file("romfs:/res/Background.png", SF2D_PLACE_RAM);
		miniBox = sfil_load_PNG_file("romfs:/res/Mini Box.png", SF2D_PLACE_RAM);
		minusButton = sfil_load_PNG_file("romfs:/res/Minus Button.png", SF2D_PLACE_RAM);
		plusButton = sfil_load_PNG_file("romfs:/res/Plus Button.png", SF2D_PLACE_RAM);
		bottomMask = sfil_load_PNG_file("romfs:/res/Mask Bottom.png", SF2D_PLACE_RAM);
		transferButton = sfil_load_PNG_file("romfs:/res/Transfer Button.png", SF2D_PLACE_RAM);
		bankTop = sfil_load_PNG_file("romfs:/res/Bank Top.png", SF2D_PLACE_RAM);
		shinyStar = sfil_load_PNG_file("romfs:/res/Shiny.png", SF2D_PLACE_RAM);
		normalBar = sfil_load_PNG_file("romfs:/res/Normal Bar.png", SF2D_PLACE_RAM);
		RButton = sfil_load_PNG_file("romfs:/res/R Button.png", SF2D_PLACE_RAM);
		LButton = sfil_load_PNG_file("romfs:/res/L Button.png", SF2D_PLACE_RAM);
		creditsTop = sfil_load_PNG_file("romfs:/res/Credits Top.png", SF2D_PLACE_RAM);
		pokeball = sfil_load_PNG_file("romfs:/res/Pokeball.png", SF2D_PLACE_RAM);
		menuBar = sfil_load_PNG_file("romfs:/res/Main Menu Dark Bar.png", SF2D_PLACE_RAM);
		menuSelectedBar = sfil_load_PNG_file("romfs:/res/Main Menu Red Bar.png", SF2D_PLACE_RAM);
		darkButton = sfil_load_PNG_file("romfs:/res/Dark Button.png", SF2D_PLACE_RAM);
		eventTop = sfil_load_PNG_file("romfs:/res/Event Top.png", SF2D_PLACE_RAM);
		left = sfil_load_PNG_file("romfs:/res/Left.png", SF2D_PLACE_RAM);
		lightButton = sfil_load_PNG_file("romfs:/res/Light Button.png", SF2D_PLACE_RAM);
		redButton = sfil_load_PNG_file("romfs:/res/Red Button.png", SF2D_PLACE_RAM);
		right = sfil_load_PNG_file("romfs:/res/Right.png", SF2D_PLACE_RAM);
		eventMenuBottomBar = sfil_load_PNG_file("romfs:/res/Event Menu Bottom Bar.png", SF2D_PLACE_RAM);
		eventMenuTopBarSelected = sfil_load_PNG_file("romfs:/res/Event Menu Top Bar Selected.png", SF2D_PLACE_RAM);
		eventMenuTopBar = sfil_load_PNG_file("romfs:/res/Event Menu Top Bar.png", SF2D_PLACE_RAM);
		infoView = sfil_load_PNG_file("romfs:/res/Info View.png", SF2D_PLACE_RAM);
		LRBar = sfil_load_PNG_file("romfs:/res/LR Bar.png", SF2D_PLACE_RAM);
		selector = sfil_load_PNG_file("romfs:/res/Selector.png", SF2D_PLACE_RAM);
		selectorCloning = sfil_load_PNG_file("romfs:/res/Selector (cloning).png", SF2D_PLACE_RAM);
		editorBG = sfil_load_PNG_file("romfs:/res/Editor Bottom BG.png", SF2D_PLACE_RAM);
		plus = sfil_load_PNG_file("romfs:/res/Plus.png", SF2D_PLACE_RAM);
		minus = sfil_load_PNG_file("romfs:/res/Minus.png", SF2D_PLACE_RAM);
		button = sfil_load_PNG_file("romfs:/res/Button.png", SF2D_PLACE_RAM);
		setting = sfil_load_PNG_file("romfs:/res/Setting.png", SF2D_PLACE_RAM);
		bottomPopUp = sfil_load_PNG_file("romfs:/res/Bottom Pop-Up.png", SF2D_PLACE_RAM);
		pokemonBufferBox = sfil_load_PNG_file("romfs:/res/Pokemon Box.png", SF2D_PLACE_RAM);
		topSelectedMove = sfil_load_PNG_file("romfs:/res/Top Selected Move.png", SF2D_PLACE_RAM);
		item = sfil_load_PNG_file("romfs:/res/item.png", SF2D_PLACE_RAM);
	} else {
		DSBottomBG = sfil_load_PNG_file("romfs:/res/Bottom BG.png", SF2D_PLACE_RAM);
		DSTopBG = sfil_load_PNG_file("romfs:/res/Top BG.png", SF2D_PLACE_RAM);
		DSBarSelected = sfil_load_PNG_file("romfs:/res/Bar Selected.png", SF2D_PLACE_RAM);
		DSBar = sfil_load_PNG_file("romfs:/res/Bar.png", SF2D_PLACE_RAM);
		DSEventBottom = sfil_load_PNG_file("romfs:/res/DS Menu Bottom BG.png", SF2D_PLACE_RAM);
		DSLangSelected = sfil_load_PNG_file("romfs:/res/Language Button Selected.png", SF2D_PLACE_RAM);
		DSLang = sfil_load_PNG_file("romfs:/res/Language Button.png", SF2D_PLACE_RAM);
		DSEventTop = sfil_load_PNG_file("romfs:/res/Event Database BG.png", SF2D_PLACE_RAM);
		DSNormalBarL = sfil_load_PNG_file("romfs:/res/Normal L.png", SF2D_PLACE_RAM);
		DSNormalBarR = sfil_load_PNG_file("romfs:/res/Normal R.png", SF2D_PLACE_RAM);
		DSSelectedBarL = sfil_load_PNG_file("romfs:/res/Selected L.png", SF2D_PLACE_RAM);
		DSSelectedBarR = sfil_load_PNG_file("romfs:/res/Selected R.png", SF2D_PLACE_RAM);
	}
}

void GUIGameElementsInit() {
	upperTextGS = sfil_load_PNG_file("romfs:/res/3DS.png", SF2D_PLACE_RAM);
	lowerTextGS = sfil_load_PNG_file("romfs:/res/DS.png", SF2D_PLACE_RAM);
	topBorder = sfil_load_PNG_file("romfs:/res/3dsborder.png", SF2D_PLACE_RAM);
	bottomBorder = sfil_load_PNG_file("romfs:/res/dsborder.png", SF2D_PLACE_RAM);
	gameSelectorBottom = sfil_load_PNG_file("/3ds/data/PKSM/additionalassets/dsicons.png", SF2D_PLACE_RAM);
	gameSelectorTop = sfil_load_PNG_file("/3ds/data/PKSM/additionalassets/3dsicons.png", SF2D_PLACE_RAM);
}

void GUIGameElementsExit() {
	sf2d_free_texture(upperTextGS);
	sf2d_free_texture(lowerTextGS);
	sf2d_free_texture(gameSelectorBottom);
	sf2d_free_texture(gameSelectorTop);
	sf2d_free_texture(topBorder);
	sf2d_free_texture(bottomBorder);
}

void GUIElementsExit() {
	sf2d_free_texture(dexBox);
	sf2d_free_texture(dexN);
	sf2d_free_texture(hiddenPowerBG);
	sf2d_free_texture(hiddenPowerButton);
	sf2d_free_texture(selectBoxButton);
	sf2d_free_texture(ballsBG);
	sf2d_free_texture(ballButton);
	sf2d_free_texture(male);
	sf2d_free_texture(female);
	sf2d_free_texture(naturesButton);
	sf2d_free_texture(naturestx);
	sf2d_free_texture(movesBottom);
	sf2d_free_texture(topMovesBG);
	sf2d_free_texture(editorBar);
	sf2d_free_texture(editorStatsBG);
	sf2d_free_texture(subArrow);
	sf2d_free_texture(backgroundTop);
	sf2d_free_texture(miniBox);
	sf2d_free_texture(minusButton);
	sf2d_free_texture(plusButton);
	sf2d_free_texture(balls);
	sf2d_free_texture(typesSheet);
	sf2d_free_texture(bottomMask);
	sf2d_free_texture(transferButton);
	sf2d_free_texture(bankTop);
	sf2d_free_texture(shinyStar);
	sf2d_free_texture(normalBar);
	sf2d_free_texture(RButton);
	sf2d_free_texture(LButton);
	sf2d_free_texture(creditsTop);
	sf2d_free_texture(pokeball);
	sf2d_free_texture(alternativeSpritesSmall);
	sf2d_free_texture(item);
	sf2d_free_texture(settings);
	sf2d_free_texture(topSelectedMove);
	sf2d_free_texture(DSEventBottom);
	sf2d_free_texture(DSLangSelected);
	sf2d_free_texture(DSLang);
	sf2d_free_texture(DSEventTop);
	sf2d_free_texture(DSNormalBarL);
	sf2d_free_texture(DSNormalBarR);
	sf2d_free_texture(DSSelectedBarL);
	sf2d_free_texture(DSSelectedBarR);
	sf2d_free_texture(DSBottomBG);
	sf2d_free_texture(DSTopBG);
	sf2d_free_texture(DSBarSelected);
	sf2d_free_texture(DSBar);
	sf2d_free_texture(cleanTop);
	sf2d_free_texture(bottomPopUp);
	sf2d_free_texture(pokemonBufferBox);
	sf2d_free_texture(selectorCloning);
	sf2d_free_texture(back);
	sf2d_free_texture(setting);
	sf2d_free_texture(editorBG);
	sf2d_free_texture(plus);
	sf2d_free_texture(minus);
	sf2d_free_texture(button);
	sf2d_free_texture(boxView);
	sf2d_free_texture(infoView);
	sf2d_free_texture(LRBar);
	sf2d_free_texture(selector);
	sf2d_free_texture(warningTop);
	sf2d_free_texture(warningBottom);
	sf2d_free_texture(eventMenuBottomBar);
	sf2d_free_texture(eventMenuTopBarSelected);
	sf2d_free_texture(eventMenuTopBar);
	sf2d_free_texture(spritesSmall);
	sf2d_free_texture(darkButton);
	sf2d_free_texture(eventTop);
	sf2d_free_texture(left);
	sf2d_free_texture(lightButton);
	sf2d_free_texture(redButton);
	sf2d_free_texture(right);
	sf2d_free_texture(mainMenuBottom);
	sf2d_free_texture(menuBar);
	sf2d_free_texture(menuSelectedBar);
	sftd_free_font(fontBold9);
	sftd_free_font(fontBold11);
	sftd_free_font(fontBold12);
	sftd_free_font(fontBold14);
	sftd_free_font(fontBold15);
	sftd_free_font(fontBold18);
	sftd_free_font(fontFixed);
}

void init_font_cache() {
	sftd_draw_text(fontBold18, 0, 0, RGBA8(0, 0, 0, 0), 18, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890:-.'!?()\"end");
	sftd_draw_text(fontBold14, 0, 0, RGBA8(0, 0, 0, 0), 14, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890:-.'!?()\"end");
	sftd_draw_text(fontBold15, 0, 0, RGBA8(0, 0, 0, 0), 15, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890:-.'!?()\"end");
	sftd_draw_text(fontBold12, 0, 0, RGBA8(0, 0, 0, 0), 12, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890:-.'!?()\"end");
	sftd_draw_text(fontBold11, 0, 0, RGBA8(0, 0, 0, 0), 11, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890:-.'!?()\"end");
	sftd_draw_text(fontBold9, 0, 0, RGBA8(0, 0, 0, 0), 9, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890:-.'!?()\"end");
	sftd_draw_text(fontFixed, 0, 0, RGBA8(0, 0, 0, 0), 10, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890:-.'!?()\"end");
}

void infoDisp(char* message) {
	while (aptMainLoop()) {
		hidScanInput();

		if (hidKeysDown() & KEY_A) break;
		
		sf2d_start_frame(GFX_TOP, GFX_LEFT);
			sf2d_draw_texture(warningTop, 0, 0);
			sftd_draw_text(fontBold15, (400 - sftd_get_text_width(fontBold15, 15, message)) / 2, 95, RGBA8(255, 255, 255, giveTransparence()), 15, message);
			sftd_draw_text(fontBold12, (400 - sftd_get_text_width(fontBold12, 12, "Press A to continue.")) / 2, 130, WHITE, 12, "Press A to continue.");
		sf2d_end_frame();
		
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
			sf2d_draw_texture(warningBottom, 0, 0);
		sf2d_end_frame();
		sf2d_swapbuffers();
	}
}

int confirmDisp(char* message) {
	while (aptMainLoop()) {
		hidScanInput();

		if (hidKeysDown() & KEY_A) return 1;
		if (hidKeysDown() & KEY_B) return 0;
		
		sf2d_start_frame(GFX_TOP, GFX_LEFT);
			sf2d_draw_texture(warningTop, 0, 0);
			sftd_draw_text(fontBold15, (400 - sftd_get_text_width(fontBold15, 15, message)) / 2, 95, RGBA8(255, 255, 255, giveTransparence()), 15, message);
			sftd_draw_text(fontBold12, (400 - sftd_get_text_width(fontBold12, 12, "Press A to continue, B to cancel.")) / 2, 130, WHITE, 12, "Press A to continue, B to cancel.");
		sf2d_end_frame();
		
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
			sf2d_draw_texture(warningBottom, 0, 0);
		sf2d_end_frame();
		sf2d_swapbuffers();
	}
	return 0;
}

void freezeMsg(char* message) {
	sf2d_start_frame(GFX_TOP, GFX_LEFT);
		sf2d_draw_texture(warningTop, 0, 0);
		sftd_draw_text(fontBold15, (400 - sftd_get_text_width(fontBold15, 15, message)) / 2, 95, WHITE, 15, message);
		sftd_draw_text(fontBold12, (400 - sftd_get_text_width(fontBold12, 12, "Please wait.")) / 2, 130, WHITE, 12, "Please wait.");
	sf2d_end_frame();
	
	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		sf2d_draw_texture(warningBottom, 0, 0);
	sf2d_end_frame();
	sf2d_swapbuffers();
}

void progressBar(char* message, u32 current, u32 sz) {
	char* progress = (char*)malloc(40 * sizeof(char));
	snprintf(progress, 40, "Progress: %lu/%lu bytes", current, sz);
	
	sf2d_start_frame(GFX_TOP, GFX_LEFT);
		sf2d_draw_texture(warningTop, 0, 0);
		sftd_draw_text(fontBold15, (400 - sftd_get_text_width(fontBold15, 15, message)) / 2, 95, WHITE, 15, message);
		sftd_draw_text(fontBold12, (400 - sftd_get_text_width(fontBold12, 12, progress)) / 2, 130, WHITE, 12, progress);
	sf2d_end_frame();
	
	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		sf2d_draw_texture(warningBottom, 0, 0);
	sf2d_end_frame();
	sf2d_swapbuffers();
	
	free(progress);
}

void drawMenuTop(int game) {
	char* version = (char*)malloc(12 * sizeof(char));
	snprintf(version, 12, "v%d.%d.%d", V1, V2, V3);
	
	if (game < 6) {
		sf2d_draw_texture(cleanTop, 0, 0);
		printTitle("PKSM");
		sf2d_draw_texture(pokeball, (400 - pokeball->width) / 2 + 5, (240 - pokeball->height) / 2 + 10);
	}
	else 
		sf2d_draw_texture(DSTopBG, 0, 0);
	
	sftd_draw_text(fontBold9, (398 - sftd_get_text_width(fontBold9, 9, version)), 229, LIGHTBLUE, 9, version);
	
	free(version);
}

void printAnimatedBG(bool isUp) {
	sf2d_draw_texture(backgroundTop, 0, 0);
	animateBG(isUp);
}

void printTitle(const char* title) {
	sftd_draw_text(fontBold14, (400 - sftd_get_text_width(fontBold14, 14, title)) / 2, 4, BLUE, 14, title);
}

void printBottomIndications(const char* message) {
	sftd_draw_text(fontBold9, (320 - sftd_get_text_width(fontBold9, 9, message)) / 2, 225, LIGHTBLUE, 9, message);
}

void gameSelectorMenu(int n) {
	sf2d_start_frame(GFX_TOP, GFX_LEFT);
		sf2d_draw_texture(cleanTop, 0, 0);
		sf2d_draw_texture(upperTextGS, 172, 65);
		
		sftd_draw_text(fontBold9, (400 - sftd_get_text_width(fontBold9, 9, "Cart has priority over digital copy.")) / 2, 6, BLUE, 9, "Cart has priority over digital copy.");
		if (n < 6) 
			sf2d_draw_texture(topBorder, 21 + 60 * n, 112);
		sf2d_draw_texture(gameSelectorTop, 26, 117);
		
		for (int i = 0; i < 6; i++) 
			sftd_draw_text(fontBold18, 17 + 60 * i + (68 - sftd_get_text_width(fontBold18, 18, gamesList[i])) / 2, 170, LIGHTBLUE, 18, gamesList[i]);
		
	sf2d_end_frame();
	
	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		sf2d_draw_texture(mainMenuBottom, 0, 0);
		sf2d_draw_texture(lowerTextGS, 142, 25);
		switch (n) {
			case GAME_DIAMOND  : { sf2d_draw_texture(bottomBorder, 56, 64); break; }
			case GAME_PEARL    : { sf2d_draw_texture(bottomBorder, 98, 64); break; }
			case GAME_PLATINUM : { sf2d_draw_texture(bottomBorder, 140, 64); break; }
			case GAME_HG       : { sf2d_draw_texture(bottomBorder, 182, 64); break; }
			case GAME_SS	   : { sf2d_draw_texture(bottomBorder, 224, 64); break; }
			case GAME_B1	   : { sf2d_draw_texture(bottomBorder, 77, 123); break; }
			case GAME_W1	   : { sf2d_draw_texture(bottomBorder, 119, 123); break; }
			case GAME_B2	   : { sf2d_draw_texture(bottomBorder, 161, 123); break; }
			case GAME_W2	   : { sf2d_draw_texture(bottomBorder, 203, 123); break; }
		}
		sf2d_draw_texture(gameSelectorBottom, 60, 68);
		
		for (int i = 6; i < 11; i++)
			sftd_draw_text(fontBold15, 61 + 42 * (i - 6) + (32 - sftd_get_text_width(fontBold15, 15, gamesList[i])) / 2, 104, LIGHTBLUE, 15, gamesList[i]);
		
		for (int i = 11; i < 15; i++)
			sftd_draw_text(fontBold15, 82 + 42 * (i - 11) + (32 - sftd_get_text_width(fontBold15, 15, gamesList[i])) / 2, 163, LIGHTBLUE, 15, gamesList[i]);
		
		sf2d_draw_rectangle(60, 68, 32, 32, RGBA8(0, 0, 0, 210));
		sf2d_draw_rectangle(102, 68, 32, 32, RGBA8(0, 0, 0, 210));
		sf2d_draw_rectangle(144, 68, 32, 32, RGBA8(0, 0, 0, 210));
		printBottomIndications("Press A continue, B to exit.");
	sf2d_end_frame();
	sf2d_swapbuffers();
}

void mainMenu(int currentEntry) {
	char* menu[3] = {"EVENTS", "MANAGEMENT", "OTHER"};
	sf2d_start_frame(GFX_TOP, GFX_LEFT);
		drawMenuTop(0);
	sf2d_end_frame();
	
	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		sf2d_draw_texture(mainMenuBottom, 0, 0);
		sf2d_draw_texture(settings, 292, 194);
		for (int i = 0; i < 3; i++) {
			if (i == currentEntry)
				sf2d_draw_texture(menuSelectedBar, (320 - menuSelectedBar->width) / 2, 60 + i * (menuSelectedBar->height));
			else
				sf2d_draw_texture(menuBar, (320 - menuBar->width) / 2, 60 + i * (menuBar->height));
			sftd_draw_text(fontBold18, (320 - sftd_get_text_width(fontBold18, 18, menu[i])) / 2 - 4, 53 + (menuBar->height - 18) / 2 + i * (menuBar->height), (i == currentEntry) ? DARKBLUE : YELLOW, 18, menu[i]);
		}
		printBottomIndications("Press START to save, X to exit.");
	sf2d_end_frame();
	sf2d_swapbuffers();
}

void mainMenuDS(int currentEntry) {
	char* menu[2] = {"EVENTS", "OTHER"};
	sf2d_start_frame(GFX_TOP, GFX_LEFT);
		drawMenuTop(7);
	sf2d_end_frame();
	
	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		sf2d_draw_texture(DSBottomBG, 0, 0);
		for (int i = 0; i < 2; i++) {
			if (i == currentEntry)
				sf2d_draw_texture(DSBarSelected, (320 - DSBarSelected->width) / 2, 66 + i * (DSBarSelected->height + 16) + i + 1);
			else
				sf2d_draw_texture(DSBar, (320 - DSBar->width) / 2, 66 + i * (DSBar->height + 16));
			sftd_draw_text(fontBold18, (320 - sftd_get_text_width(fontBold18, 18, menu[i])) / 2, 67 + (DSBar->height - 18) / 2 + i * (DSBar->height + 16), WHITE, 18, menu[i]);
		}
		printBottomIndications("Press START to save, X to exit.");
	sf2d_end_frame();
	sf2d_swapbuffers();
}

void menu4(int currentEntry, char* menu[], int n) {
	sf2d_start_frame(GFX_TOP, GFX_LEFT);
		drawMenuTop(0);
	sf2d_end_frame();
	
	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		sf2d_draw_texture(mainMenuBottom, 0, 0);
		
		for (int i = 0; i < n; i++) {
			if (i == currentEntry)
				sf2d_draw_texture(menuSelectedBar, (320 - menuSelectedBar->width) / 2, 40 + i * (menuSelectedBar->height));
			else
				sf2d_draw_texture(menuBar, (320 - menuBar->width) / 2, 40 + i * (menuBar->height));
			sftd_draw_text(fontBold18, (320 - sftd_get_text_width(fontBold18, 18, menu[i])) / 2 - 4, 44 + i * (menuBar->height), (i == currentEntry) ? DARKBLUE : YELLOW, 18, menu[i]);
		}
		printBottomIndications("Press A to select an option.");
	sf2d_end_frame();
	sf2d_swapbuffers();
}

void menu3(int currentEntry, char* menu[], int n) {
	sf2d_start_frame(GFX_TOP, GFX_LEFT);
		drawMenuTop(0);
	sf2d_end_frame();
	
	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		sf2d_draw_texture(mainMenuBottom, 0, 0);
		
		for (int i = 0; i < 3; i++) {
			if (i == currentEntry)
				sf2d_draw_texture(menuSelectedBar, (320 - menuSelectedBar->width) / 2, 60 + i * (menuSelectedBar->height));
			else
				sf2d_draw_texture(menuBar, (320 - menuBar->width) / 2, 60 + i * (menuBar->height));
			sftd_draw_text(fontBold18, (320 - sftd_get_text_width(fontBold18, 18, menu[i])) / 2 - 4, 53 + (menuBar->height - 18) / 2 + i * (menuBar->height), (i == currentEntry) ? DARKBLUE : YELLOW, 18, menu[i]);
		}
		printBottomIndications("Press A to select an option.");
	sf2d_end_frame();
	sf2d_swapbuffers();
}

void printCredits() {
	u8* buf = (u8*)malloc(1500 * sizeof(u8));
	loadFile(buf, "romfs:/res/credits.txt");
	
	while (aptMainLoop() && !(hidKeysUp() & KEY_B)) {
		hidScanInput();
		
		sf2d_start_frame(GFX_TOP, GFX_LEFT);
			sf2d_draw_texture(cleanTop, 0, 0);
			printTitle("Credits");
			sf2d_draw_texture(creditsTop, 0, 45);
			sftd_draw_text(fontBold15, 18, 77, LIGHTBLUE, 15,  "Bernardo Giordano");
			sftd_draw_text(fontBold15, 64, 174, LIGHTBLUE, 15,  "dsoldier for the complete GUI design");
		sf2d_end_frame();

		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
			sf2d_draw_texture(mainMenuBottom, 0, 0);
			sftd_draw_text(fontBold9, 20, 35, LIGHTBLUE, 9, (char*)buf);
			printBottomIndications("Press B to return.");
		sf2d_end_frame();
		sf2d_swapbuffers();
	}
	
	free(buf);
}

void printDatabase6(char *database[], int currentEntry, int page, int spriteArray[]) {
	int pk, y = 41;
	char *pages = (char*)malloc(10 * sizeof(char));
	snprintf(pages, 10, "%d/%d", page + 1, 205);
	
	sf2d_start_frame(GFX_TOP, GFX_LEFT);
		sf2d_draw_texture(cleanTop, 0, 0);
		printTitle("Event Database");
		
		for (int i = 0; i < 5; i++) {
			pk = spriteArray[page * 10 + i];
			if (i == currentEntry)
				sf2d_draw_texture(eventMenuTopBarSelected, 18, y);
			else
				sf2d_draw_texture(eventMenuTopBar, 18, y);
			
			if (pk != -1)
				sf2d_draw_texture_part(spritesSmall, 20, y - ((i == currentEntry) ? movementOffsetSlow(2) : 0), 40 * (pk % 25) + 4, 30 * (pk / 25), 34, 30);
			if (sftd_get_text_width(fontBold9, 9, database[page * 10 + i]) <= 148)
				sftd_draw_text(fontBold9, 54, y + 14, (i == currentEntry) ? HIGHBLUE : YELLOW, 9, database[page * 10 + i]);
			else
				sftd_draw_text_wrap(fontBold9, 54, y + 3, (i == currentEntry) ? HIGHBLUE : YELLOW, 9, 148, database[page * 10 + i]);
			
			y += 37;
		}
		
		y = 41;
		for (int i = 5; i < 10; i++) {
			pk = spriteArray[page * 10 + i];
			if (i == currentEntry)
				sf2d_draw_texture(eventMenuTopBarSelected, 200, y);
			else
				sf2d_draw_texture(eventMenuTopBar, 200, y);
			
			if (pk != -1)
				sf2d_draw_texture_part(spritesSmall, 202, y - ((i == currentEntry) ? movementOffsetSlow(2) : 0), 40 * (pk % 25) + 4, 30 * (pk / 25), 34, 30);
			if (sftd_get_text_width(fontBold9, 9, database[page * 10 + i]) <= 148)
				sftd_draw_text(fontBold9, 235, y + 14, (i == currentEntry) ? HIGHBLUE : YELLOW, 9, database[page * 10 + i]);
			else
				sftd_draw_text_wrap(fontBold9, 235, y + 3, (i == currentEntry) ? HIGHBLUE : YELLOW, 9, 148, database[page * 10 + i]);
			
			y += 37;
		}
	sf2d_end_frame();

	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		sf2d_draw_texture(mainMenuBottom, 0, 0);
		sf2d_draw_texture(eventMenuBottomBar, (320 - eventMenuBottomBar->width) / 2, 45);
		sf2d_draw_texture(LButton, 83, 52);
		sf2d_draw_texture(RButton, 221, 52);
		sftd_draw_text(fontBold12, (320 - sftd_get_text_width(fontBold12, 12, pages)) / 2, 52, WHITE, 12, pages);
		printBottomIndications("Press A to continue, B to return.");
	sf2d_end_frame();
	sf2d_swapbuffers();
	
	free(pages);
}

void printDatabase5(char *database[], int currentEntry, int page, int spriteArray[], bool isSelected, int langSelected, bool langVett[]) {
	int pk, y = 41;
	
	sf2d_start_frame(GFX_TOP, GFX_LEFT);
		sf2d_draw_texture(DSEventTop, 0, 0);
		
		for (int i = 0; i < 5; i++) {
			pk = spriteArray[page * 10 + i];
			if (i == currentEntry)
				sf2d_draw_texture(DSSelectedBarL, 18, y);
			else
				sf2d_draw_texture(DSNormalBarL, 18, y);
			
			if (pk != -1)
				sf2d_draw_texture_part(spritesSmall, 22, y + 2, 40 * (pk % 25) + 4, 30 * (pk / 25), 34, 30);
			if (sftd_get_text_width(fontBold9, 9, database[page * 10 + i]) <= 148)
				sftd_draw_text(fontBold9, 55, y + 14, WHITE, 9, database[page * 10 + i]);
			else
				sftd_draw_text_wrap(fontBold9, 55, y + 3, WHITE, 9, 148, database[page * 10 + i]);
			
			y += DSSelectedBarL->height;
		}
		
		y = 41;
		for (int i = 5; i < 10; i++) {
			pk = spriteArray[page * 10 + i];
			if (i == currentEntry)
				sf2d_draw_texture(DSSelectedBarR, 200, y);
			else
				sf2d_draw_texture(DSNormalBarR, 200, y);
			
			if (pk != -1)
				sf2d_draw_texture_part(spritesSmall, 204, y + 2, 40 * (pk % 25) + 4, 30 * (pk / 25), 34, 30);
			if (sftd_get_text_width(fontBold9, 9, database[page * 10 + i]) <= 148)
				sftd_draw_text(fontBold9, 235, y + 14, WHITE, 9, database[page * 10 + i]);
			else
				sftd_draw_text_wrap(fontBold9, 235, y + 3, WHITE, 9, 148, database[page * 10 + i]);
			
			y += DSSelectedBarR->height;
		}
	sf2d_end_frame();

	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		sf2d_draw_texture(DSEventBottom, 0, 0);
		sftd_draw_text(fontBold9, (320 - sftd_get_text_width(fontBold9, 9, isSelected ? "Press START to inject, B to return" : "Press A to select, B to return.")) / 2, 222, RGBA8(255, 255, 255, 130), 9, isSelected ? "Press START to inject, B to return" : "Press A to select, B to return.");
		
		if (isSelected) {
			char *languages[7] = {"JPN", "ENG", "FRE", "ITA", "GER", "SPA", "KOR"};
			char *path = (char*)malloc(40 * sizeof(char));
			u8* buf = (u8*)malloc(1500 * sizeof(u8));
			memset(buf, 0, 1500);
			snprintf(path, 40, "romfs:/database/gen5/%d.txt", page * 10 + currentEntry);
			loadFile(buf, path);
			
			sftd_draw_text(fontFixed, 8, 2,  WHITE, 10, (char*)buf);
			
			for (int t = 0; t < 7; t++) {
				int x = 0, y = 178;
				switch (t) {
					case 0 : { x = 25; break; }
					case 1 : { x = 63; break; }
					case 2 : { x = 101; break; }
					case 3 : { x = 139; break; }
					case 4 : { x = 177; break; }
					case 5 : { x = 215; break; }
					case 6 : { x = 253; break; }
				}
				
				sf2d_draw_texture(DSLang, x, y);
				if (langVett[t]) {
					if (t == langSelected) sf2d_draw_texture(DSLangSelected, x, y);
					sftd_draw_text(fontBold12, x + (36 - sftd_get_text_width(fontBold12, 12, languages[t])) / 2, y + 4, WHITE, 12, languages[t]);
				}
				else {
					if (t == langSelected) sf2d_draw_texture(DSLangSelected, x, y);
					sftd_draw_text(fontBold12, x + (36 - sftd_get_text_width(fontBold12, 12, languages[t])) / 2, y + 4, RGBA8(255, 255, 255, 100), 12, languages[t]);
				}
			}

			free(path);
			free(buf);
		}
	
	sf2d_end_frame();
	sf2d_swapbuffers();
}

void printDatabase4(char *database[], int currentEntry, int page, int spriteArray[], bool isSelected, int langSelected, bool langVett[]) {
	int pk, y = 41;
	
	sf2d_start_frame(GFX_TOP, GFX_LEFT);
		sf2d_draw_texture(DSEventTop, 0, 0);
		
		for (int i = 0; i < 5; i++) {
			pk = spriteArray[page * 10 + i];
			if (i == currentEntry)
				sf2d_draw_texture(DSSelectedBarL, 18, y);
			else
				sf2d_draw_texture(DSNormalBarL, 18, y);
			
			if (pk != -1)
				sf2d_draw_texture_part(spritesSmall, 22, y + 2, 40 * (pk % 25) + 4, 30 * (pk / 25), 34, 30);
			if (sftd_get_text_width(fontBold9, 9, database[page * 10 + i]) <= 148)
				sftd_draw_text(fontBold9, 55, y + 14, WHITE, 9, database[page * 10 + i]);
			else
				sftd_draw_text_wrap(fontBold9, 55, y + 3, WHITE, 9, 148, database[page * 10 + i]);
			
			y += DSSelectedBarL->height;
		}
		
		y = 41;
		for (int i = 5; i < 10; i++) {
			pk = spriteArray[page * 10 + i];
			if (i == currentEntry)
				sf2d_draw_texture(DSSelectedBarR, 200, y);
			else
				sf2d_draw_texture(DSNormalBarR, 200, y);
			
			if (pk != -1)
				sf2d_draw_texture_part(spritesSmall, 204, y + 2, 40 * (pk % 25) + 4, 30 * (pk / 25), 34, 30);
			if (sftd_get_text_width(fontBold9, 9, database[page * 10 + i]) <= 148)
				sftd_draw_text(fontBold9, 235, y + 14, WHITE, 9, database[page * 10 + i]);
			else
				sftd_draw_text_wrap(fontBold9, 235, y + 3, WHITE, 9, 148, database[page * 10 + i]);
			
			y += DSSelectedBarR->height;
		}
	sf2d_end_frame();

	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		sf2d_draw_texture(DSEventBottom, 0, 0);
		sftd_draw_text(fontBold9, (320 - sftd_get_text_width(fontBold9, 9, isSelected ? "Press START to inject, B to return" : "Press A to select, B to return.")) / 2, 222, RGBA8(255, 255, 255, 130), 9, isSelected ? "Press START to inject, B to return" : "Press A to select, B to return.");
		
		if (isSelected) {
			char *languages[7] = {"JPN", "ENG", "FRE", "ITA", "GER", "SPA", "KOR"};
			char *path = (char*)malloc(40 * sizeof(char));
			u8* buf = (u8*)malloc(1500 * sizeof(u8));
			memset(buf, 0, 1500);
			snprintf(path, 40, "romfs:/database/gen4/%d.txt", page * 10 + currentEntry);
			loadFile(buf, path);
			
			sftd_draw_text(fontFixed, 8, 2,  WHITE, 10, (char*)buf);
			
			for (int t = 0; t < 7; t++) {
				int x = 0, y = 178;
				switch (t) {
					case 0 : { x = 25; break; }
					case 1 : { x = 63; break; }
					case 2 : { x = 101; break; }
					case 3 : { x = 139; break; }
					case 4 : { x = 177; break; }
					case 5 : { x = 215; break; }
					case 6 : { x = 253; break; }
				}
				
				sf2d_draw_texture(DSLang, x, y);
				if (langVett[t]) {
					if (t == langSelected) sf2d_draw_texture(DSLangSelected, x, y);
					sftd_draw_text(fontBold12, x + (36 - sftd_get_text_width(fontBold12, 12, languages[t])) / 2, y + 4, WHITE, 12, languages[t]);
				}
				else {
					if (t == langSelected) sf2d_draw_texture(DSLangSelected, x, y);
					sftd_draw_text(fontBold12, x + (36 - sftd_get_text_width(fontBold12, 12, languages[t])) / 2, y + 4, RGBA8(255, 255, 255, 100), 12, languages[t]);
				}
			}

			free(path);
			free(buf);
		}
	
	sf2d_end_frame();
	sf2d_swapbuffers();
}

void printDB7(int sprite, int i, bool langVett[], bool adapt, bool overwrite, int langSelected, int nInjected) {
	char *languages[] = {"JPN", "ENG", "FRE", "ITA", "GER", "SPA", "KOR", "CHS", "CHT"};
	char *path = (char*)malloc(30 * sizeof(char));
	char *cont = (char*)malloc(3 * sizeof(char));
	u8* buf = (u8*)malloc(1500 * sizeof(u8));
	memset(buf, 0, 1499);
	snprintf(path, 30, "romfs:/database/%d.txt", i);
	snprintf(cont, 3, "%d", nInjected + 1);
	loadFile(buf, path);
	
	sf2d_start_frame(GFX_TOP, GFX_LEFT);
		printAnimatedBG(true);
		sf2d_draw_texture(eventTop, 0, 24);
		if (sprite != -1)
			sf2d_draw_texture_part_scale(spritesSmall, 282, 46 - movementOffsetLong(6), 40 * (sprite % 25) + 4, 30 * (sprite / 25), 34, 30, 2, 2);
		sftd_draw_text(fontFixed, 5, 28,  WHITE, 10, (char*)buf);
	sf2d_end_frame();
	
	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		sf2d_draw_texture(mainMenuBottom, 0, 0);
		sftd_draw_text(fontBold14, 16, 50, LIGHTBLUE, 14, "Languages:");
		sftd_draw_text(fontBold14, 33, 112, LIGHTBLUE, 14, "Overwrite Wondercard:");
		sftd_draw_text(fontBold14, 33, 140, LIGHTBLUE, 14, "Adapt language to WC:");
		sftd_draw_text(fontBold14, 33, 170, LIGHTBLUE, 14, "Inject WC in slot:");
		
		for (int t = 0; t < 9; t++) {
			int x = 0, y = 0;
			switch (t) {
				case 0 : { x = 114; y = 50; break; }
				case 1 : { x = 153; y = 50; break; }
				case 2 : { x = 192; y = 50; break; }
				case 3 : { x = 231; y = 50; break; }
				case 4 : { x = 270; y = 50; break; }
				case 5 : { x = 133; y = 74; break; }
				case 6 : { x = 172; y = 74; break; }
				case 7 : { x = 211; y = 74; break; }
				case 8 : { x = 250; y = 74; break; }
			}
			if (langVett[t]) {
				if (t == langSelected) {
					sf2d_draw_texture(redButton, x, y);
					sftd_draw_text(fontBold14, x + (36 - sftd_get_text_width(fontBold14, 14, languages[t])) / 2, y + 2, DARKBLUE, 14, languages[t]);
				} else {
					sf2d_draw_texture(darkButton, x, y);
					sftd_draw_text(fontBold14, x + (36 - sftd_get_text_width(fontBold14, 14, languages[t])) / 2, y + 2, YELLOW, 14, languages[t]);
				}
			}
			else {
				sf2d_draw_texture(lightButton, x, y);
				sftd_draw_text(fontBold14, x + (36 - sftd_get_text_width(fontBold14, 14, languages[t])) / 2, y + 2, DARKBLUE, 14, languages[t]);
			}
		}
			
		if (overwrite) {
			sf2d_draw_texture(redButton, 210, 110);
			sf2d_draw_texture(darkButton, 249, 110);
		}
		else {
			sf2d_draw_texture(darkButton, 210, 110);
			sf2d_draw_texture(redButton, 249, 110);			
		}
		
		if (adapt) {
			sf2d_draw_texture(redButton, 210, 138);
			sf2d_draw_texture(darkButton, 249, 138);
		}
		else {
			sf2d_draw_texture(darkButton, 210, 138);
			sf2d_draw_texture(redButton, 249, 138);
		}
		
		sf2d_draw_texture(darkButton, 229, 168);	
		
		sftd_draw_text(fontBold14, 210 + (36 - sftd_get_text_width(fontBold14, 14, "Yes")) / 2, 112, (overwrite) ? DARKBLUE : YELLOW, 14, "Yes");
		sftd_draw_text(fontBold14, 249 + (36 - sftd_get_text_width(fontBold14, 14, "No")) / 2, 112, (!overwrite) ? DARKBLUE : YELLOW, 14, "No");
		sftd_draw_text(fontBold14, 210 + (36 - sftd_get_text_width(fontBold14, 14, "Yes")) / 2, 140, (adapt) ? DARKBLUE : YELLOW, 14, "Yes");
		sftd_draw_text(fontBold14, 249 + (36 - sftd_get_text_width(fontBold14, 14, "No")) / 2, 140, (!adapt) ? DARKBLUE : YELLOW, 14, "No");
		sftd_draw_text(fontBold14, 229 + (36 - sftd_get_text_width(fontBold14, 14, cont)) / 2, 170, YELLOW, 14, cont);
		
		printBottomIndications("Press START to inject, B to return.");
		sf2d_end_frame();
	sf2d_swapbuffers();
	
	free(buf);
	free(cont);
	free(path);
}

void printDB6(int sprite, int i, bool langVett[], bool adapt, bool overwrite, int langSelected, int nInjected) {
	char *languages[7] = {"JPN", "ENG", "FRE", "ITA", "GER", "SPA", "KOR"};
	char *path = (char*)malloc(30 * sizeof(char));
	char *cont = (char*)malloc(3 * sizeof(char));
	u8* buf = (u8*)malloc(1500 * sizeof(u8));
	memset(buf, 0, 1499);
	snprintf(path, 30, "romfs:/database/%d.txt", i);
	snprintf(cont, 3, "%d", nInjected + 1);
	loadFile(buf, path);
	
	sf2d_start_frame(GFX_TOP, GFX_LEFT);
		sf2d_draw_texture(eventTop, 0, 24);
		if (sprite != -1)
			sf2d_draw_texture_part_scale(spritesSmall, 282, 46 - movementOffsetLong(6), 40 * (sprite % 25) + 4, 30 * (sprite / 25), 34, 30, 2, 2);
		sftd_draw_text(fontFixed, 5, 28,  WHITE, 10, (char*)buf);
	sf2d_end_frame();
	
	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		sf2d_draw_texture(mainMenuBottom, 0, 0);
		sftd_draw_text(fontBold14, 33, 50, LIGHTBLUE, 14, "Languages:");
		sftd_draw_text(fontBold14, 33, 112, LIGHTBLUE, 14, "Overwrite Wondercard:");
		sftd_draw_text(fontBold14, 33, 140, LIGHTBLUE, 14, "Adapt language to WC:");
		sftd_draw_text(fontBold14, 33, 170, LIGHTBLUE, 14, "Inject WC in slot:");
		
		for (int t = 0; t < 7; t++) {
			int x = 0, y = 0;
			switch (t) {
				case 0 : { x = 132; y = 50; break; }
				case 1 : { x = 171; y = 50; break; }
				case 2 : { x = 210; y = 50; break; }
				case 3 : { x = 249; y = 50; break; }
				case 4 : { x = 151; y = 74; break; }
				case 5 : { x = 190; y = 74; break; }
				case 6 : { x = 229; y = 74; break; }
			}
			if (langVett[t]) {
				if (t == langSelected) {
					sf2d_draw_texture(redButton, x, y);
					sftd_draw_text(fontBold14, x + (36 - sftd_get_text_width(fontBold14, 14, languages[t])) / 2, y + 2, DARKBLUE, 14, languages[t]);
				} else {
					sf2d_draw_texture(darkButton, x, y);
					sftd_draw_text(fontBold14, x + (36 - sftd_get_text_width(fontBold14, 14, languages[t])) / 2, y + 2, YELLOW, 14, languages[t]);
				}
			}
			else {
				sf2d_draw_texture(lightButton, x, y);
				sftd_draw_text(fontBold14, x + (36 - sftd_get_text_width(fontBold14, 14, languages[t])) / 2, y + 2, DARKBLUE, 14, languages[t]);
			}
		}
			
		if (overwrite) {
			sf2d_draw_texture(redButton, 210, 110);
			sf2d_draw_texture(darkButton, 249, 110);
		}
		else {
			sf2d_draw_texture(darkButton, 210, 110);
			sf2d_draw_texture(redButton, 249, 110);			
		}
		
		if (adapt) {
			sf2d_draw_texture(redButton, 210, 138);
			sf2d_draw_texture(darkButton, 249, 138);
		}
		else {
			sf2d_draw_texture(darkButton, 210, 138);
			sf2d_draw_texture(redButton, 249, 138);
		}
		
		sf2d_draw_texture(darkButton, 229, 168);	
		
		sftd_draw_text(fontBold14, 210 + (36 - sftd_get_text_width(fontBold14, 14, "Yes")) / 2, 112, (overwrite) ? DARKBLUE : YELLOW, 14, "Yes");
		sftd_draw_text(fontBold14, 249 + (36 - sftd_get_text_width(fontBold14, 14, "No")) / 2, 112, (!overwrite) ? DARKBLUE : YELLOW, 14, "No");
		sftd_draw_text(fontBold14, 210 + (36 - sftd_get_text_width(fontBold14, 14, "Yes")) / 2, 140, (adapt) ? DARKBLUE : YELLOW, 14, "Yes");
		sftd_draw_text(fontBold14, 249 + (36 - sftd_get_text_width(fontBold14, 14, "No")) / 2, 140, (!adapt) ? DARKBLUE : YELLOW, 14, "No");
		sftd_draw_text(fontBold14, 229 + (36 - sftd_get_text_width(fontBold14, 14, cont)) / 2, 170, YELLOW, 14, cont);
		
		printBottomIndications("Press START to inject, B to return.");
	sf2d_end_frame();
	sf2d_swapbuffers();
	
	free(buf);
	free(cont);
	free(path);
}

void printEditor(u8* mainbuf, int game, int currentEntry, int langCont) {
	int y = 41;
	char *languages[] = {"JPN", "ENG", "FRE", "ITA", "GER", "SPA", "KOR", "CHS", "CHT"};

	sf2d_start_frame(GFX_TOP, GFX_LEFT);
		sf2d_draw_texture(cleanTop, 0, 0);
		printTitle("Save File Editor");
		
		for (int i = 0; i < 5; i++) {
			if (i == currentEntry)
				sf2d_draw_texture(eventMenuTopBarSelected, 18, y);
			else
				sf2d_draw_texture(eventMenuTopBar, 18, y);
			
			switch (i) {
				case 0 : {
					int x = 140;
					sftd_draw_text(fontBold12, 40, y + 10, DARKGREY, 12, "Language:");
					sf2d_draw_texture(miniBox, x, y + 7);
					sftd_draw_text(fontBold12, x + (36 - sftd_get_text_width(fontBold12, 12, languages[langCont])) / 2, y + 10, YELLOW, 12, languages[langCont]);
					break;
				}
				case 1 : { sftd_draw_text(fontBold12, 18 + (182 - sftd_get_text_width(fontBold12, 12, "Set Heals to max")) / 2, y + 10, (i == currentEntry) ? DARKBLUE : YELLOW, 12, "Set Heals to max"); break; }
				case 2 : { sftd_draw_text(fontBold12, 18 + (182 - sftd_get_text_width(fontBold12, 12, "Set Items to max")) / 2, y + 10, (i == currentEntry) ? DARKBLUE : YELLOW, 12, "Set Items to max"); break; }
				case 3 : { sftd_draw_text(fontBold12, 18 + (182 - sftd_get_text_width(fontBold12, 12, "Set Berries to max")) / 2, y + 10, (i == currentEntry) ? DARKBLUE : YELLOW, 12, "Set Berries to max"); break; }
				case 4 : { sftd_draw_text(fontBold12, 18 + (182 - sftd_get_text_width(fontBold12, 12, "Set all TMs")) / 2, y + 10, (i == currentEntry) ? DARKBLUE : YELLOW, 12, "Set all TMs"); break; }
			}
			y += 37;
		}
		
		y = 41;
		for (int i = 5; i < 10; i++) {
			if (i == currentEntry)
				sf2d_draw_texture(eventMenuTopBarSelected, 200, y);
			else
				sf2d_draw_texture(eventMenuTopBar, 200, y);
			
			switch (i) {
				case 5 : { sftd_draw_text(fontBold12, 200 + (182 - sftd_get_text_width(fontBold12, 12, "Clear Mystery Gift box")) / 2, y + 10, (i == currentEntry) ? DARKBLUE : YELLOW, 12, "Clear Mystery Gift box"); break; }
			}
			y += 37;
		}
	sf2d_end_frame();

	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		sf2d_draw_texture(mainMenuBottom, 0, 0);
		printBottomIndications("Press START to edit, A to toggle, B to exit.");
	sf2d_end_frame();
	sf2d_swapbuffers();
}

u16 getAlternativeSprite(u8* pkmn) {
    u16 tempspecies;
    if (getForm(pkmn)) {
		memcpy(&tempspecies, &personal.pkmData[getPokedexNumber(pkmn)][0x1C], 2);
		
		switch (tempspecies) {
			case 829 : return 2;
			case 922 : return 3;
			case 828 : return 4;
			case 827 : return 5;
			case 945 : return 6;
			case 917 : return 7;
			case 916 : return 8;
			case 921 : return 10;
			case 920 : return 11;
			case 826 : return 12;
			case 919 : return 13;
			case 907 : return 14;
			case 823 : return 15;
			case 918 : return 16;
			case 898 : return 17;
			case 944 : return 18;
			case 913 : return 19;
			case 912 : return 20;
			case 808 : return 21;
			case 911 : return 22;
			case 909 : return 23;
			case 923 : return 24;
			case 908 : return 25;
			case 924 : return 26;
			case 914 : return 27;
			case 915 : return 28;
			case 809 : return 30;
		}
	}
	
	return 0;
}

void printElement(u8* pkmn, u16 n, int x, int y) {
	u16 t = getAlternativeSprite(pkmn);
	if (t) {
		t -= 1;
		sf2d_draw_texture_part(alternativeSpritesSmall, x, y, 40 * (t % 6) + 4, 30 * (t / 6), 34, 30); 
	} else 
		sf2d_draw_texture_part(spritesSmall, x, y, 40 * (n % 25) + 4, 30 * (n / 25), 34, 30);
	if (isEgg(pkmn))
		sf2d_draw_texture_part(spritesSmall, x + 6, y + 6, 40 * (EGGSPRITEPOS % 25) + 4, 30 * (EGGSPRITEPOS / 25), 34, 30);
	if (getItem(pkmn))
		sf2d_draw_texture(item, x + 3, y + 21);
}
void printElementBlend(u8* pkmn, u16 n, int x, int y) {
	u16 t = getAlternativeSprite(pkmn);
	if (t) {
		t -= 1;
		sf2d_draw_texture_part_blend(alternativeSpritesSmall, x, y, 40 * (t % 6) + 4, 30 * (t / 6), 34, 30, RGBA8(0x0, 0x0, 0x0, 100)); 
	} else
		sf2d_draw_texture_part_blend(spritesSmall, x, y, 40 * (n % 25) + 4, 30 * (n / 25), 34, 30, RGBA8(0x0, 0x0, 0x0, 100));
	if (isEgg(pkmn))
		sf2d_draw_texture_part_blend(spritesSmall, x + 6, y + 6, 40 * (EGGSPRITEPOS % 25) + 4, 30 * (EGGSPRITEPOS / 25), 34, 30, RGBA8(0x0, 0x0, 0x0, 100));
	if (getItem(pkmn))
		sf2d_draw_texture_blend(item, x + 3, y + 21, RGBA8(0x0, 0x0, 0x0, 100));
}

void infoViewer(u8* pkmn) {
	int y_desc = 29;
	char* entries[] = {"Nickname:", "OT:", "Pokerus:", "Nature", "Ability:", "Item:", "TID:", "SID:", "Friendship:", "Hidden Power:"};
	char* values[] = {"HP", "Attack", "Defence", "Sp. Atk", "Sp. Def", "Speed"};
	
	printAnimatedBG(true);
	sf2d_draw_texture(infoView, 0, 0);

	sf2d_draw_texture(normalBar, 252, 155);
	sf2d_draw_texture(normalBar, 252, 176);
	sf2d_draw_texture(normalBar, 252, 197);
	sf2d_draw_texture(normalBar, 252, 218);
	
	sftd_draw_text(fontBold12, 251, 138, WHITE, 12, "Moves");		
	for (int i = 0; i < 10; i++) {
		sftd_draw_text(fontBold12, 2, y_desc, BLUE, 12, entries[i]);
		y_desc += 20;
		if (i == 2) y_desc += 5;
		if (i == 5) y_desc += 6;
	}
	
	y_desc = 8;
	for (int i = 0; i < 6; i++) {
		sftd_draw_text(fontBold12, 225, y_desc, LIGHTBLUE, 12, values[i]);
		y_desc += 20;
	}
	
	if (getPokedexNumber(pkmn)) {
		sf2d_draw_texture_part(balls, -2, -5, 32 * (getBall(pkmn) % 8), 32 * (getBall(pkmn) / 8), 32, 32);
		sftd_draw_text(fontBold12, 30, 6, WHITE, 12, (char*)personal.species[getPokedexNumber(pkmn)]);
		
		if (getGender(pkmn) == 0)
			sf2d_draw_texture(male, 146, 7);
		else if (getGender(pkmn) == 1)
			sf2d_draw_texture(female, 148, 7);
		
		char* level = (char*)malloc(8 * sizeof(char));
		snprintf(level, 8, "Lv.%u", getLevel(pkmn));
		sftd_draw_text(fontBold12, 160, 6, WHITE, 12, level);
		free(level);
		
		char *nick = (char*)malloc(26 * sizeof(char));
		getNickname(pkmn, nick);
		sftd_draw_text(fontBold12, 215 - (sftd_get_text_width(fontBold12, 12, nick)), 29, WHITE, 12, nick);
		free(nick);
		
		char *ot_name = (char*)malloc(0x17 * sizeof(char));
		getOT(pkmn, ot_name);
		sftd_draw_text(fontBold12, 215 - (sftd_get_text_width(fontBold12, 12, ot_name)), 49, WHITE, 12, ot_name);
		free(ot_name);	
		
		sftd_draw_text(fontBold12, 215 - sftd_get_text_width(fontBold12, 12, (isInfected(pkmn) ? "Yes" : "No")), 69, WHITE, 12, isInfected(pkmn) ? "Yes" : "No");
		sftd_draw_text(fontBold12, 215 - sftd_get_text_width(fontBold12, 12, natures[getNature(pkmn)]), 94, WHITE, 12, natures[getNature(pkmn)]);
		sftd_draw_text(fontBold12, 215 - sftd_get_text_width(fontBold12, 12, abilities[getAbility(pkmn)]), 114, WHITE, 12, abilities[getAbility(pkmn)]);
		sftd_draw_text(fontBold12, 215 - sftd_get_text_width(fontBold12, 12, items[getItem(pkmn)]), 134, WHITE, 12, items[getItem(pkmn)]);
		
		if (isShiny(pkmn))
			sf2d_draw_texture(shinyStar, 205, 9);
		
		char* friendship = (char*)malloc(4 * sizeof(char));
		snprintf(friendship, 4, "%u", getFriendship(pkmn));
		sftd_draw_text(fontBold12, 215 - sftd_get_text_width(fontBold12, 12, friendship), 200, WHITE, 12, friendship);
		free(friendship);
		
		char* otid = (char*)malloc(7 * sizeof(char));
		snprintf(otid, 7, "%u", getOTID(pkmn));
		sftd_draw_text(fontBold12, 215 - sftd_get_text_width(fontBold12, 12, otid), 160, WHITE, 12, otid);
		snprintf(otid, 7, "%u", getSOTID(pkmn));
		sftd_draw_text(fontBold12, 215 - sftd_get_text_width(fontBold12, 12, otid), 180, WHITE, 12, otid);
		free(otid);

		sftd_draw_text(fontBold12, 215 - sftd_get_text_width(fontBold12, 12, hpList[getHPType(pkmn)]), 220, WHITE, 12, hpList[getHPType(pkmn)]);
		
		int max = sftd_get_text_width(fontBold12, 12, "252");		
		int y_moves = 159;
		for (int i = 0; i < 4; i++) {
			if (getMove(pkmn, i))
				sftd_draw_text(fontBold12, 396 - sftd_get_text_width(fontBold12, 12, moves[getMove(pkmn, i)]), y_moves, WHITE, 12, moves[getMove(pkmn, i)]);
			y_moves += 21;
		}
		
		char* tmp = (char*)malloc(4);
		for (int i = 0; i < 6; i++) {
			snprintf(tmp, 4, "%d", getIV(pkmn, lookup[i]));
			sftd_draw_text(fontBold12, 289 + (max - sftd_get_text_width(fontBold12, 12, tmp)) / 2, 8 + i * 20, WHITE, 12, tmp);
			snprintf(tmp, 4, "%d", getEV(pkmn, lookup[i]));
			sftd_draw_text(fontBold12, 328 + (max - sftd_get_text_width(fontBold12, 12, tmp)) / 2, 8 + i * 20, WHITE, 12, tmp);
			snprintf(tmp, 4, "%d", getStat(pkmn, lookup[i]));
			sftd_draw_text(fontBold12, 369 + (max - sftd_get_text_width(fontBold12, 12, tmp)) / 2, 8 + i * 20, WHITE, 12, tmp);
		}
		free(tmp);
	}
}

void printPKViewer(u8* mainbuf, u8* tmp, bool isTeam, int game, int currentEntry, int menuEntry, int box, int mode, bool speedy, int additional1) {
	char* menuEntries[] = {"EDIT", "CLONE", "RELEASE", "GENERATE", "EXIT"};
	int x;
	char* page = (char*)malloc(7 * sizeof(char));
	snprintf(page, 7, "Box %d", box + 1);
	
	u8* pkmn = (u8*)malloc(PKMNLENGTH * sizeof(u8));
	getPkmn(mainbuf, (isTeam) ? 33 : box, currentEntry, pkmn, game);
	
	sf2d_start_frame(GFX_TOP, GFX_LEFT);
		infoViewer(pkmn);
	sf2d_end_frame();

	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		sf2d_draw_texture(boxView, 0, 0);
		sf2d_draw_texture(editorBar, 0, 210);
		sftd_draw_text(fontBold12, 12 + (LRBar->width - sftd_get_text_width(fontBold12, 12, page)) / 2, 19, WHITE, 12, page);
		sf2d_draw_texture(left, 7, 17);
		sf2d_draw_texture(right, 185, 17);
		sf2d_draw_texture(back, 280, 210);
		sftd_draw_text(fontBold12, 247, 7, WHITE, 12, "Team");
		
		int y = 45;
		int pointer[2] = {0, 0};
		for (int i = 0; i < 5; i++) {
			x = 4;
			for (int j = 0; j < 6; j++) {
				getPkmn(mainbuf, box, i * 6 + j, pkmn, game);
				u16 n = getPokedexNumber(pkmn);
				if (n)
					printElement(pkmn, n, x, y);

				if ((currentEntry == (i * 6 + j)) && !isTeam) {
					pointer[0] = x + 18;
					pointer[1] = y - 8;
				}
				x += 34;
			}
			y += 30;
		}
		
		for (int i = 0; i < 3; i++) {
			sf2d_draw_texture(pokemonBufferBox, 214, 40 + i * 45);
			sf2d_draw_texture(pokemonBufferBox, 266, 60 + i * 45);
		}
		
		y = 45;
		for (int i = 0; i < 3; i++) {
			x = 222;
			for (int j = 0; j < 2; j++) {
				getPkmn(mainbuf, 33, i * 2 + j, pkmn, game);
				u16 n = getPokedexNumber(pkmn);
				if (n)
					printElement(pkmn, n, x, (j == 1) ? y + 20 : y);

				if ((currentEntry == (i * 2 + j)) && isTeam) {
					pointer[0] = x + 18;
					pointer[1] = y - 8 + ((j == 1) ? 20 : 0);
				}
				x += 52;
			}
			y += 45;
		}
		
		if (mode != ED_CLONE)
			sf2d_draw_texture(selector, pointer[0], pointer[1] - ((mode == ED_STANDARD) ? movementOffsetSlow(3) : 0));
		else {
			sf2d_draw_texture(selectorCloning, pointer[0], pointer[1] - movementOffsetSlow(3));			
			sf2d_draw_texture(bottomPopUp, 1, 214);
			sftd_draw_text(fontBold11, 8, 220, WHITE, 11, "Press A to clone in selected slot, B to cancel");
		}
		
		if (mode == ED_MENU) {
			sf2d_draw_texture(bottomMask, 0, 0);
			sf2d_draw_texture(bottomPopUp, 1, 214);
			sftd_draw_textf(fontBold11, 8, 220, WHITE, 11, "%s has been selected.", (char*)personal.species[getPokedexNumber(tmp)]);
			for (int i = 0; i < 5; i++) {
				sf2d_draw_texture(button, 208, 42 + i * 27);
				if (i == menuEntry)
					sf2d_draw_texture(subArrow, 203 - movementOffsetSlow(3), 46 + i * 27);
				sftd_draw_text(fontBold12, 208 + (109 - sftd_get_text_width(fontBold12, 12, menuEntries[i])) / 2, 49 + i * 27, BLACK, 12, menuEntries[i]);
			}
		} else if (mode == ED_GENERATE) {
			sf2d_draw_texture(bottomMask, 0, 0);
			sf2d_draw_texture(bottomPopUp, 1, 214);
			sftd_draw_textf(fontBold11, 8, 220, WHITE, 11, "You can switch speed with L/R: %s", (speedy) ? "FAST" : "SLOW");
			sf2d_draw_texture(left, 48, 91);
			sf2d_draw_texture(right, 145, 91);
			sf2d_draw_texture(dexBox, 79, 82);
			//sf2d_draw_texture(dexN, 50, 147);
			sf2d_draw_texture_part(spritesSmall, 87, 89, 40 * (additional1 % 25) + 4, 30 * (additional1 / 25), 34, 30);
			//sftd_draw_text(fontBold12, 50 + (109 - sftd_get_text_width(fontBold12, 12, "DEX N.")) / 2, 156, BLACK, 12, "DEX N.");
		}
		
	sf2d_end_frame();
	sf2d_swapbuffers();
	
	free(pkmn);
	free(page);
}

void printPKEditor(u8* pkmn, int game, bool speedy, int additional1, int additional2, int additional3, int mode) {
	int max = sftd_get_text_width(fontBold12, 12, "252");
	char* entries[] = {"Level:", "Nature:", "Ability:", "Item:", "Shiny:", "Pokerus:", "OT:", "Nickname:", "Friendship:"};
	char* options[] = {"STATS", "MOVES", "SAVE"};
	
	char* values[6] = {"HP:", "Attack:", "Defense:", "Sp. Attack:", "Sp. Defence:", "Speed:"};
	u16 n = getPokedexNumber(pkmn);
	
	sf2d_start_frame(GFX_TOP, GFX_LEFT);
	if (mode == ED_BASE || mode == ED_STATS) {
		sftd_draw_text(fontBold15, (400 - sftd_get_text_width(fontBold15, 15, "Edit your Pokemon in the bottom screen")) / 2, 95, RGBA8(255, 255, 255, giveTransparence()), 15, "Edit your Pokemon in the bottom screen");
		sftd_draw_text(fontBold15, (400 - sftd_get_text_width(fontBold15, 15, "Tap SAVE when you're done")) / 2, 115, RGBA8(255, 255, 255, giveTransparence()), 15, "Tap SAVE when you're done");
	} else if (mode == ED_ITEMS) {
		int y = 0, x = 0;
		sf2d_draw_texture(topMovesBG, 0, 0);
		for (int i = 0; i < 20; i++) {
			if (i == additional1) 
				sf2d_draw_texture(topSelectedMove, x + 2, y);
			sftd_draw_textf(fontBold9, x + 2, y, WHITE, 9, "%d - %s", 40 * additional2 + i, itemsSorted[40 * additional2 + i]);
			y += (topSelectedMove->height + 1);
		}
		y = 0; x = 200;
		for (int i = 20; i < 40; i++) {
			if (i == additional1) sf2d_draw_texture(topSelectedMove, x, y);
			sftd_draw_textf(fontBold9, x + 2, y, WHITE, 9, "%d - %s", 40 * additional2 + i, itemsSorted[40 * additional2 + i]);
			y += (topSelectedMove->height + 1);
		}		
	} else if (mode == ED_MOVES) {
		int y = 0, x = 0;
		sf2d_draw_texture(topMovesBG, 0, 0);
		for (int i = 0; i < 20; i++) {
			if (i == additional1) sf2d_draw_texture(topSelectedMove, x + 2, y);
			sftd_draw_textf(fontBold9, x + 2, y, WHITE, 9, "%d - %s", 40 * additional2 + i, movesSorted[40 * additional2 + i]);
			y += (topSelectedMove->height + 1);
		}
		y = 0; x = 200;
		for (int i = 20; i < 40; i++) {
			if (i == additional1) sf2d_draw_texture(topSelectedMove, x, y);
			sftd_draw_textf(fontBold9, x + 2, y, WHITE, 9, "%d - %s", 40 * additional2 + i, movesSorted[40 * additional2 + i]);
			y += (topSelectedMove->height + 1);
		}	
	} else if (mode == ED_NATURES) {
		char* hor[] = {"Neutral", "-Attack", "-Defence", "-Speed", "-Sp. Atk.", "-Sp. Def."};
		char* ver[] = {"+Attack", "+Defence", "+Speed", "+Sp. Atk.", "+Sp. Def."};
		
		sf2d_draw_texture(naturestx, 0, 0);
		for (int i = 0; i < 6; i++)
			sftd_draw_text(fontBold12, 66 * i + (66 - sftd_get_text_width(fontBold12, 12, hor[i])) / 2, 13, (i == 0) ? YELLOW : BLUE, 12, hor[i]);
		for (int i = 0; i < 5; i++)
			sftd_draw_text(fontBold12, (66 - sftd_get_text_width(fontBold12, 12, ver[i])) / 2, 53 + i * 40, BLUE, 12, ver[i]);
		
		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 5; j++) {
				if (additional1 == i * 5 + j)
					sf2d_draw_texture(naturesButton, 66 + j * 66 + j, 40 + i * 40);
				sftd_draw_text(fontBold12, 66 + 66 * j + (66 - sftd_get_text_width(fontBold12, 12, natures[i * 5 + j])) / 2 + j, 53 + i * 40, (i == j) ? YELLOW : WHITE, 12, natures[i * 5 + j]);
			}
		}
	} else if (mode == ED_BALLS) {
		char* entries[] = {"Master Ball", "Ultra Ball", "Great Ball", "Poke Ball", "Safari Ball", "Net Ball", "Dive Ball", "Nest Ball", "Repeat Ball", "Timer Ball", "Luxury Ball", "Premier Ball", "Dusk Ball", "Heal Ball", "Quick Ball", "Cherish Ball", "Fast Ball", "Level Ball", "Lure Ball", "Heavy Ball", "Love Ball", "Friend Ball", "Moon Ball", "Sport Ball", "Dream Ball", "Beast Ball", " ", " ", " ", " "};
		sf2d_draw_texture(ballsBG, 0, 0);
		
		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 6; j++) {
				if (additional1 == i * 6 + j)
					sf2d_draw_texture(ballButton, j * 66 + j, i * 47 + i);
				sf2d_draw_texture_part(balls, 17 + 66 * j + j, 2 + i * 47, 32 * ((i * 6 + j + 1) % 8), 32 * ((i * 6 + j + 1) / 8), 32, 32);
				sftd_draw_text(fontBold9, 66 * j + (66 - sftd_get_text_width(fontBold9, 9, entries[i * 6 + j])) / 2 + j, 30 + i * 47 + i, WHITE, 9, entries[i * 6 + j]);
			}
		}
	} else if (mode == ED_HIDDENPOWER) {
		sf2d_draw_texture(hiddenPowerBG, 0, 0);
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				if (additional1 == i * 4 + j)
					sf2d_draw_texture(hiddenPowerButton, j * 99 + j, + i * 59+ i);
				sf2d_draw_texture_part(typesSheet, 24 + 99 * j + j, 20 + 59 * i + i, 50 * (i * 4 + j + 1), 0, 50, 18); 
			}
		}
	}
	sf2d_end_frame();
	
	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		printAnimatedBG(false);
		if (mode == ED_BASE || mode == ED_ITEMS || mode == ED_NATURES || mode == ED_BALLS) {
			sf2d_draw_texture(editorBG, 0, 1);
			sf2d_draw_texture(editorBar, 0, 210);
			
			sftd_draw_text(fontBold12, 27, 4, WHITE, 12, (char*)personal.species[n]);
			sf2d_draw_texture_part(balls, -2, -6, 32 * (getBall(pkmn) % 8), 32 * (getBall(pkmn) / 8), 32, 32);
			
			u16 t = getAlternativeSprite(pkmn);
			int ofs = movementOffsetSlow(3);
			if (t) {
				t -= 1;
				sf2d_draw_texture_part_scale_blend(alternativeSpritesSmall, 232, 32 - ofs, 40 * (t % 6) + 4, 30 * (t / 6), 34, 30, 2, 2, RGBA8(0x0, 0x0, 0x0, 100)); 
				sf2d_draw_texture_part_scale(alternativeSpritesSmall, 227, 27 - ofs, 40 * (t % 6) + 4, 30 * (t / 6), 34, 30, 2, 2);
			} else {
				sf2d_draw_texture_part_scale_blend(spritesSmall, 232, 32 - ofs, 40 * (n % 25) + 4, 30 * (n / 25), 34, 30, 2, 2, RGBA8(0x0, 0x0, 0x0, 100));
				sf2d_draw_texture_part_scale(spritesSmall, 227, 27 - ofs, 40 * (n % 25) + 4, 30 * (n / 25), 34, 30, 2, 2);
			}
			
			if (getGender(pkmn) == 0)
				sf2d_draw_texture(male, 159, 6);
			else if (getGender(pkmn) == 1)
				sf2d_draw_texture(female, 161, 6);
			
			for (int i = 0; i < 3; i++) {
				sf2d_draw_texture(button, 206, 116 + i * 27);
				sftd_draw_text(fontBold12, 206 + (109 - sftd_get_text_width(fontBold12, 12, options[i])) / 2, 123 + i * 27, BLACK, 12, options[i]);
			}
			
			for (int i = 0; i < 9; i++)
				sftd_draw_text(fontBold12, 2, 29 + i * 20, LIGHTBLUE, 12, entries[i]);

			for (int i = 0; i < 7; i++)
				sf2d_draw_texture(setting, 180, 51 + i * 20);

			sf2d_draw_texture(minus, 180 - max - 6 - minus->width, 29);
			sf2d_draw_texture(plus, 180, 29);
			sf2d_draw_texture(minus, 180 - max - 6 - minus->width, 189);
			sf2d_draw_texture(plus, 180, 189);

			char* level = (char*)malloc(4 * sizeof(char));
			snprintf(level, 4, "%u", getLevel(pkmn));
			sftd_draw_text(fontBold12, 180 - max - 3 + (max - sftd_get_text_width(fontBold12, 12, level)) / 2, 29, WHITE, 12, level);
			free(level);
				
			sftd_draw_text(fontBold12, 178 - sftd_get_text_width(fontBold12, 12, natures[getNature(pkmn)]), 49, WHITE, 12, natures[getNature(pkmn)]);
			sftd_draw_text(fontBold12, 178 - sftd_get_text_width(fontBold12, 12, abilities[getAbility(pkmn)]), 69, WHITE, 12, abilities[getAbility(pkmn)]);
			sftd_draw_text(fontBold12, 178 - sftd_get_text_width(fontBold12, 12, items[getItem(pkmn)]), 89, WHITE, 12, items[getItem(pkmn)]);
			sftd_draw_text(fontBold12, 178 - sftd_get_text_width(fontBold12, 12, isShiny(pkmn) ? "Yes" : "No"), 109, WHITE, 12, isShiny(pkmn) ? "Yes" : "No");
			sftd_draw_text(fontBold12, 178 - sftd_get_text_width(fontBold12, 12, isInfected(pkmn) ? "Yes" : "No"), 129, WHITE, 12, isInfected(pkmn) ? "Yes" : "No");
			
			char* friendship = (char*)malloc(4 * sizeof(char));
			snprintf(friendship, 4, "%u", getFriendship(pkmn));
			sftd_draw_text(fontBold12, 180 - max - 3 + (max - sftd_get_text_width(fontBold12, 12, friendship)) / 2, 189, WHITE, 12, friendship);
			free(friendship);
			
			char *nick = (char*)malloc(26 * sizeof(char));
			getNickname(pkmn, nick);
			sftd_draw_text(fontBold12, 178 - (sftd_get_text_width(fontBold12, 12, nick)), 169, WHITE, 12, nick);
			free(nick);
			
			char *ot_name = (char*)malloc(0x17 * sizeof(char));
			getOT(pkmn, ot_name);
			sftd_draw_text(fontBold12, 178 - (sftd_get_text_width(fontBold12, 12, ot_name)), 149, WHITE, 12, ot_name);
			free(ot_name);
		}
		if (mode == ED_STATS || mode == ED_HIDDENPOWER) {
			sf2d_draw_texture(editorStatsBG, 0, 1);
			sf2d_draw_texture(editorBar, 0, 210);
			sf2d_draw_texture(setting, 291, 175);
			sftd_draw_text(fontBold12, 2, 28, LIGHTBLUE, 12, "STATS");
			sftd_draw_text(fontBold12, 118, 28, DARKBLUE, 12, "IV");
			sftd_draw_text(fontBold12, 197, 28, DARKBLUE, 12, "EV");
			sftd_draw_text(fontBold12, 256, 28, DARKBLUE, 12, "TOTAL");
			sftd_draw_text(fontBold12, 2, 173, LIGHTBLUE, 12, "Hidden Power:");
			
			sftd_draw_text(fontBold12, 27, 4, WHITE, 12, (char*)personal.species[n]);
			sf2d_draw_texture_part(balls, -2, -6, 32 * (getBall(pkmn) % 8), 32 * (getBall(pkmn) / 8), 32, 32);
			
			if (getGender(pkmn) == 0)
				sf2d_draw_texture(male, 159, 6);
			else if (getGender(pkmn) == 1)
				sf2d_draw_texture(female, 161, 6);
			
			for (int i = 0; i < 6; i++)
				sftd_draw_text(fontBold12, 2, 49 + i * 20, LIGHTBLUE, 12, values[i]);

			char* tmp = (char*)malloc(4);
			for (int i = 0; i < 6; i++) {
				snprintf(tmp, 4, "%d", getIV(pkmn, lookup[i]));
				sftd_draw_text(fontBold12, 112 + (max - sftd_get_text_width(fontBold12, 12, tmp)) / 2, 49 + i * 20, WHITE, 12, tmp);
				snprintf(tmp, 4, "%d", getEV(pkmn, lookup[i]));
				sftd_draw_text(fontBold12, 192 + (max - sftd_get_text_width(fontBold12, 12, tmp)) / 2, 49 + i * 20, WHITE, 12, tmp);
				snprintf(tmp, 4, "%d", getStat(pkmn, lookup[i]));
				sftd_draw_text(fontBold12, 263 + (max - sftd_get_text_width(fontBold12, 12, tmp)) / 2, 49 + i * 20, WHITE, 12, tmp);
			}
			free(tmp);
			
			sftd_draw_text(fontBold12, 288 - sftd_get_text_width(fontBold12, 12, hpList[getHPType(pkmn)]), 173, WHITE, 12, hpList[getHPType(pkmn)]);	

			for (int i = 0; i < 6; i++) {
				sf2d_draw_texture(minus, 96, 49 + i * 20);
				sf2d_draw_texture(plus, 139, 49 + i * 20);
				sf2d_draw_texture(minus, 177, 49 + i * 20);
				sf2d_draw_texture(plus, 218, 49 + i * 20);
			}
		}
		if (mode == ED_MOVES) {
			sf2d_draw_texture(movesBottom, 0, 1);
			sf2d_draw_texture(movesBottom, 0, 2 + movesBottom->height);
			sf2d_draw_texture(editorBar, 0, 210);
			sftd_draw_text(fontBold12, 2, 5, LIGHTBLUE, 12, "Moves");
			sftd_draw_text(fontBold12, 2, 110, LIGHTBLUE, 12, "Relearn Moves");
			
			for (int i = 0; i < 4; i++) {
				sftd_draw_text(fontBold12, 2, 28 + i * 20, (i == additional3) ? YELLOW : WHITE, 12, moves[getMove(pkmn, i)]);
				sftd_draw_text(fontBold12, 2, 132 + i * 20, (i == additional3 - 4) ? YELLOW: WHITE, 12, moves[getEggMove(pkmn, i)]);
				if (i == additional3)
					sf2d_draw_texture_rotate(subArrow, 198 - movementOffsetSlow(3), 33 + i * 20, 3.1415f);
				else if (i == additional3 - 4)
					sf2d_draw_texture_rotate(subArrow, 198 - movementOffsetSlow(3), 137 + i * 20, 3.1415f);
			}
		}

		sf2d_draw_texture(back, 280, 210);
		sftd_draw_textf(fontBold9, 55, 220, WHITE, 9, "You can switch speed using (L/R): %s", speedy ? "FAST" : "SLOW");
		
		// apply masks
		if (mode == ED_ITEMS) {
			sf2d_draw_texture(bottomMask, 0, 0);
			sftd_draw_text(fontBold14, (320 - sftd_get_text_width(fontBold14, 14, "Select an item with A in the top screen.")) / 2, 105, WHITE, 14, "Select an item with A in the top screen.");
		} else if (mode == ED_NATURES) {
			sf2d_draw_texture(bottomMask, 0, 0);
			sftd_draw_text(fontBold14, (320 - sftd_get_text_width(fontBold14, 14, "Select a nature with A in the top screen.")) / 2, 105, WHITE, 14, "Select a nature with A in the top screen.");
		} else if (mode == ED_BALLS) {
			sf2d_draw_texture(bottomMask, 0, 0);
			sftd_draw_text(fontBold14, (320 - sftd_get_text_width(fontBold14, 14, "Select a ball with A in the top screen.")) / 2, 105, WHITE, 14, "Select a ball with A in the top screen.");
		} else if (mode == ED_HIDDENPOWER) {
			sf2d_draw_texture(bottomMask, 0, 0);
			sftd_draw_text(fontBold14, (320 - sftd_get_text_width(fontBold14, 14, "Select a HP type with A in the top screen.")) / 2, 105, WHITE, 14, "Select a HP type with A in the top screen.");
		}
		
	sf2d_end_frame();
	sf2d_swapbuffers();
}

void printPKBank(u8* bankbuf, u8* mainbuf, u8* pkmnbuf, int game, int currentEntry, int saveBox, int bankBox, bool isBufferized, bool isSeen) {
	int x, y;
	int pointer[2] = {0, 0};
	char* page = (char*)malloc(10 * sizeof(char));
	
	u8* pkmn = (u8*)malloc(PKMNLENGTH * sizeof(u8));
	if (currentEntry < 30)
		memcpy(pkmn, &bankbuf[bankBox * 30 * PKMNLENGTH + currentEntry * PKMNLENGTH], PKMNLENGTH);
	else
		getPkmn(mainbuf, saveBox, currentEntry - 30, pkmn, game);
	
	sf2d_start_frame(GFX_TOP, GFX_LEFT);
		if (isSeen) {
			infoViewer(pkmnbuf);
		} else {
			printAnimatedBG(true);
			sf2d_draw_texture(bankTop, 34, 5);
			snprintf(page, 10, "Bank %d", bankBox + 1);
			sftd_draw_text(fontBold12, 55 + (LRBar->width - sftd_get_text_width(fontBold12, 12, page)) / 2, 9, WHITE, 12, page);

			if (getPokedexNumber(pkmn)) {
				u16 tempspecies = getPokedexNumber(pkmn);
				if (getForm(pkmn))
					memcpy(&tempspecies, &personal.pkmData[getPokedexNumber(pkmn)][0x1C], 2);
				u8 type1 = personal.pkmData[tempspecies][0x6];
				u8 type2 = personal.pkmData[tempspecies][0x7];
				
				sf2d_draw_texture_part(typesSheet, 273, 120, 50 * type1, 0, 50, 18); 
				if (type1 != type2)
					sf2d_draw_texture_part(typesSheet, 325, 120, 50 * type2, 0, 50, 18); 
				
				char *nick = (char*)malloc(26 * sizeof(char));
				getNickname(pkmn, nick);
				sftd_draw_text(fontBold12, 273, 69, WHITE, 12, nick);
				free(nick);
				
				char* level = (char*)malloc(8 * sizeof(char));
				snprintf(level, 8, "Lv.%u", getLevel(pkmn));
				sftd_draw_text(fontBold12, 372 - sftd_get_text_width(fontBold12, 12, level), 86, WHITE, 12, level);
				
				if (getGender(pkmn) == 0)
					sf2d_draw_texture(male, 358 - sftd_get_text_width(fontBold12, 12, level), 86);
				else if (getGender(pkmn) == 1)
					sf2d_draw_texture(female, 360 - sftd_get_text_width(fontBold12, 12, level), 86);
				if (isShiny(pkmn))
					sf2d_draw_texture(shinyStar, 360 - sftd_get_text_width(fontBold12, 12, level) - 14, 88);
				
				free(level);
				
				char *ot_name = (char*)malloc(0x17 * sizeof(char));
				getOT(pkmn, ot_name);
				sftd_draw_text(fontBold12, 273, 146, WHITE, 12, ot_name);
				free(ot_name);

				char* otid = (char*)malloc(12 * sizeof(char));
				snprintf(otid, 12, "ID. %u", getOTID(pkmn));
				sftd_draw_text(fontBold12, 372 - sftd_get_text_width(fontBold12, 12, otid), 163, WHITE, 12, otid);
				free(otid);
				
				sftd_draw_text(fontBold12, 273, 104, WHITE, 12, (char*)personal.species[getPokedexNumber(pkmn)]);
			}
			y = 45;
			for (int i = 0; i < 5; i++) {
				x = 44;
				for (int j = 0; j < 6; j++) {
					memcpy(pkmn, &bankbuf[bankBox * 30 * PKMNLENGTH + (i * 6 + j) * PKMNLENGTH], PKMNLENGTH);
					u16 n = getPokedexNumber(pkmn);
					if (n)
						printElement(pkmn, n, x, y);

					if (currentEntry == (i * 6 + j)) {
						pointer[0] = x + 18;
						pointer[1] = y - 8;
					}
					x += 34;
				}
				y += 30;
			}
			
			if (currentEntry < 30) {
				u16 n = getPokedexNumber(pkmnbuf);
				if (n) printElementBlend(pkmnbuf, n, pointer[0] - 14, pointer[1] + 8);
				if (n) printElement(pkmnbuf, n, pointer[0] - 18, pointer[1] + 3);
				sf2d_draw_texture(selector, pointer[0], pointer[1] - 2 - ((!isBufferized) ? movementOffsetSlow(3) : 0));
			}
		}		
	sf2d_end_frame();

	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		sf2d_draw_texture(boxView, 0, 0);
		sf2d_draw_texture(editorBar, 0, 210);
		snprintf(page, 10, "Save %d", saveBox + 1);
		sftd_draw_text(fontBold12, 12 + (LRBar->width - sftd_get_text_width(fontBold12, 12, page)) / 2, 19, WHITE, 12, page);
		sf2d_draw_texture(left, 7, 17);
		sf2d_draw_texture(right, 185, 17);
		sf2d_draw_texture(transferButton, 242, 5);
		sf2d_draw_texture(back, 280, 210);
		sf2d_draw_texture(button, 208, 43);
		sf2d_draw_texture(button, 208, 70);
		sf2d_draw_texture(button, 208, 97);
		sftd_draw_text(fontBold12, 208 + (109 - sftd_get_text_width(fontBold12, 12, "Y: VIEW")) / 2, 50, BLACK, 12, "Y: VIEW");
		sftd_draw_text(fontBold12, 208 + (109 - sftd_get_text_width(fontBold12, 12, "CLEAR BOX")) / 2, 77, BLACK, 12, "CLEAR BOX");
		sftd_draw_text(fontBold12, 208 + (109 - sftd_get_text_width(fontBold12, 12, "RELEASE")) / 2, 104, BLACK, 12, "RELEASE");
				
		y = 45;
		for (int i = 0; i < 5; i++) {
			x = 4;
			for (int j = 0; j < 6; j++) {
				getPkmn(mainbuf, saveBox, i*6+j, pkmn, game);
				u16 n = getPokedexNumber(pkmn);
				if (n)
					printElement(pkmn, n, x, y);

				if ((currentEntry - 30) == (i * 6 + j)) {
					pointer[0] = x + 18;
					pointer[1] = y - 8;
				}
				x += 34;
			}
			y += 30;
		}
		
		if (currentEntry > 29) {
			if (!isSeen) {
				u16 n = getPokedexNumber(pkmnbuf);
				if (n) printElementBlend(pkmnbuf, n, pointer[0] - 14, pointer[1] + 8);
				if (n) printElement(pkmnbuf, n, pointer[0] - 18, pointer[1] + 3);
				sf2d_draw_texture(selector, pointer[0], pointer[1] - 2 - ((!isBufferized) ? movementOffsetSlow(3) : 0));
			} else
				sf2d_draw_texture(selector, pointer[0], pointer[1] - 2);
		}
		
		if (isSeen)
			sf2d_draw_texture(bottomMask, 0, -30);
	sf2d_end_frame();
	sf2d_swapbuffers();
	
	free(pkmn);
	free(page);
}

void printMassInjector(int currentEntry) {
	int y = 41;
	char* entries[] = {"XD Collection", "Colosseum Collection", "10th Anniversary Collection", "N's Collection", "Entree Forest Collection", "Dream Radar Collection", "Living Dex", "Oblivia Deoxys Collection", "Pokemon Ranch Collection", "KOR Events Collection"};
	int boxes[] = {3, 2, 1, 1, 1, 1, 27, 1, 1, 1};
	
	char* message = (char*)malloc(30 * sizeof(char));
	snprintf(message, 30, "%d boxes will be replaced.", boxes[currentEntry]);
	
	sf2d_start_frame(GFX_TOP, GFX_LEFT);
		sf2d_draw_texture(cleanTop, 0, 0);
		printTitle("Mass Injector");
		
		for (int i = 0; i < 5; i++) {
			if (i == currentEntry)
				sf2d_draw_texture(eventMenuTopBarSelected, 18, y);
			else
				sf2d_draw_texture(eventMenuTopBar, 18, y);
			
			sftd_draw_text(fontBold12, 18 + (182 - sftd_get_text_width(fontBold12, 12, entries[i])) / 2, y + 10, (i == currentEntry) ? DARKBLUE : YELLOW, 12, entries[i]);
			
			y += 37;
		}
		
		y = 41;
		for (int i = 5; i < 10; i++) {
			if (i == currentEntry)
				sf2d_draw_texture(eventMenuTopBarSelected, 200, y);
			else
				sf2d_draw_texture(eventMenuTopBar, 200, y);
			
			sftd_draw_text(fontBold12, 200 + (182 - sftd_get_text_width(fontBold12, 12, entries[i])) / 2, y + 10, (i == currentEntry) ? DARKBLUE : YELLOW, 12, entries[i]);
			
			y += 37;
		}
	sf2d_end_frame();

	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		sf2d_draw_texture(mainMenuBottom, 0, 0);
		sftd_draw_text(fontBold12, (320 - sftd_get_text_width(fontBold12, 12, message)) / 2, 12, LIGHTBLUE, 12, message);
		printBottomIndications("Press START to inject, B to exit.");
	sf2d_end_frame();
	sf2d_swapbuffers();
	
	free(message);
}

void printSettings(int box, bool speedy) {
	char *menu[] = {"Bank Size:                   ", "Backup Save", "Backup Bank"};
	sf2d_start_frame(GFX_TOP, GFX_LEFT);
		drawMenuTop(0);
	sf2d_end_frame();
	
	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		sf2d_draw_texture(mainMenuBottom, 0, 0);

		for (int i = 0; i < 3; i++) {
			sf2d_draw_texture(menuBar, (320 - menuBar->width) / 2, 60 + i * (menuBar->height));
			sftd_draw_text(fontBold15, (320 - sftd_get_text_width(fontBold15, 15, menu[i])) / 2 - 4, 55 + (menuBar->height - 18) / 2 + i * (menuBar->height), DARKBLUE, 15, menu[i]);
		}
		
		sf2d_draw_texture(miniBox, 189, 64);
		sf2d_draw_texture(minusButton, 169, 65);
		sf2d_draw_texture(plusButton, 228, 65);
		
		char* size = (char*)malloc(5);
		snprintf(size, 5, "%d", box);
		sftd_draw_text(fontBold11, 189 + (36 - (sftd_get_text_width(fontBold11, 11, size))) / 2, 68, WHITE, 11, size);
		free(size);
		
		sftd_draw_textf(fontBold9, 60, 225, LIGHTBLUE, 9, "You can switch speed using (L/R): %s", speedy ? "FAST" : "SLOW");
	sf2d_end_frame();
	sf2d_swapbuffers();
}