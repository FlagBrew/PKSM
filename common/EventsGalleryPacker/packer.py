#!/usr/bin/python3
import git
import os
import json
import struct
import bz2
import gen4string
import hashlib
from shutil import rmtree

validLangs = ["CHS", "CHT", "ENG", "FRE", "GER", "ITA", "JPN", "KOR", "SPA"]
validTypes = ["wc7", "wc6", "wc7full", "wc6full", "pgf", "wc4", "pgt", "pcd"]

def getWC4(data):
	return bytearray(data[0x8:0x8 + 136])

def sortById(thing):
	return thing['id']

def scanDir(root, sheet, origOffset):
	retdata = b''
	for path, subdirs, files in os.walk(root):
		for fullname in files:
			try:
				name = fullname[:fullname.rindex(".")]
				type = fullname[fullname.rindex(".")+1:]
				if type not in validTypes:
					continue
				size = os.stat(os.path.join(path, fullname)).st_size
				game = name[name.index(" ")+1:name[name.index(" ")+1:].index(" ") + name.index(" ")+1]

				lang = None
				for validLang in validLangs:
					if "({})".format(validLang) in name:
						lang = validLang
						break
				if lang is None:
					lang = "ENG"

				entry = {}
				if type == 'pgt':
					if path[path.rindex("/")+1:] == "Pokemon Ranger Manaphy Egg":
						game = "DPPtHGSS"
						entry['name'] = "Pokemon Ranger Manaphy Egg"
					else:
						entry['name'] = name.replace("Item ", "").replace(" " + game, "").replace(" (" + lang + ")","")
				entry['type'] = type
				entry['size'] = size
				entry['game'] = game
				entry['offset'] = origOffset + len(retdata)
				sheet['wondercards'].append(entry)
				
				with open(os.path.join(path, fullname), 'rb') as f:
					tempdata = f.read()
					if type == 'wc7' or type == 'wc6':
						entry['species'] = -1 if tempdata[0x51] != 0 else struct.unpack('<H', tempdata[0x82:0x84])[0]
						entry['form'] = -1 if tempdata[0x51] != 0 else tempdata[0x84]
						entry['gender'] = -1 if tempdata[0x51] != 0 else tempdata[0xA1]
						cardId = struct.unpack('<H', tempdata[:0x2])[0]
						# get event title
						name = tempdata[0x2:0x4C]
						for i in range(0, len(name), 2):
							if name[i] == 0x00:
								if name[i+1] == 0x00:
									name = name[:i]
									break
						if len(name) == 0:
							if "(" in fullname:
								name = fullname[:fullname.index("(")].replace("Pokemon Link ","")
							else:
								name = fullname[:fullname.rindex(".")].replace("Pokemon Link ","")
						else:
							name = name.decode('utf-16le')
						entry['name'] = "%04i - " % cardId + name
						inMatches = False
						for i in range(len(sheet['matches'])):
							if sheet['matches'][i]['id'] == cardId and sheet['matches'][i]['species'] == entry['species'] and sheet['matches'][i]['form'] == entry['form'] and sheet['matches'][i]['gender'] == entry['gender']:
								sheet['matches'][i]['indices'][lang] = len(sheet['wondercards']) - 1
								inMatches = True
						if not inMatches:
							match = {}
							match['id'] = cardId
							match['species'] = entry['species']
							match['form'] = entry['form']
							match['gender'] = entry['gender']
							match['indices'] = {}
							match['indices'][lang] = len(sheet['wondercards']) - 1
							sheet['matches'].append(match)
					elif type == 'wc7full' or type == 'wc6full':
						entry['species'] = -1 if tempdata[0x51 + 0x208] != 0 else struct.unpack('<H', tempdata[0x28A:0x28C])[0]
						entry['form'] = -1 if tempdata[0x51 + 0x208] != 0 else tempdata[0x28C]
						entry['gender'] = -1 if tempdata[0x51 + 0x208] != 0 else tempdata[0x2A9]
						cardId = struct.unpack('<H', tempdata[0x208:0x20A])[0]
						# get event title
						name = tempdata[0x20A:0x254]
						for i in range(0, len(name), 2):
							if name[i] == 0x00:
								if name[i+1] == 0x00:
									name = name[:i]
									break
						name = name.decode('utf-16le')
						entry['name'] = "%04i - " % cardId + name
						inMatches = False
						for i in range(len(sheet['matches'])):
							if sheet['matches'][i]['id'] == cardId and sheet['matches'][i]['species'] == entry['species'] and sheet['matches'][i]['form'] == entry['form'] and sheet['matches'][i]['gender'] == entry['gender']:
								sheet['matches'][i]['indices'][lang] = len(sheet['wondercards']) - 1
								inMatches = True
						if not inMatches:
							match = {}
							match['id'] = cardId
							match['species'] = entry['species']
							match['form'] = entry['form']
							match['gender'] = entry['gender']
							match['indices'] = {}
							match['indices'][lang] = len(sheet['wondercards']) - 1
							sheet['matches'].append(match)
					elif type == 'pgf':
						entry['species'] = -1 if tempdata[0xB3] != 1 else struct.unpack('<H', tempdata[0x1A:0x1C])[0]
						entry['form'] = -1 if tempdata[0xB3] != 1 else tempdata[0x1C]
						entry['gender'] = -1 if tempdata[0xB3] != 1 else tempdata[0x35]
						cardId = struct.unpack('<H', tempdata[0xB0:0xB2])[0]
						# get event title
						name = tempdata[0x60:0xAA]
						for i in range(0, len(name), 2):
							if name[i] == 0xFF:
								if name[i+1] == 0xFF:
									name = name[:i]
									break
						name = name.decode('utf-16le')
						entry['name'] = "%04i - " % cardId + name
						inMatches = False
						for i in range(len(sheet['matches'])):
							if sheet['matches'][i]['id'] == cardId and sheet['matches'][i]['species'] == entry['species'] and sheet['matches'][i]['form'] == entry['form'] and sheet['matches'][i]['gender'] == entry['gender']:
								sheet['matches'][i]['indices'][lang] = len(sheet['wondercards']) - 1
								inMatches = True
						if not inMatches:
							match = {}
							match['id'] = cardId
							match['species'] = entry['species']
							match['form'] = entry['form']
							match['gender'] = entry['gender']
							match['indices'] = {}
							match['indices'][lang] = len(sheet['wondercards']) - 1
							sheet['matches'].append(match)
					elif type == 'wc4' or type == 'pcd':
						if tempdata[0] == 1 or tempdata[0] == 2:
							pk4 = getWC4(tempdata)
							entry['species'] = struct.unpack('<H', pk4[0x8:0x0A])[0]
							entry['form'] = pk4[0x40] >> 3
							entry['gender'] = pk4[0x40] >> 1 & 0x3
						elif tempdata[0] == 7:
							entry['species'] = 470
							entry['form'] = -1 # special meaning for Manaphy: egg
							entry['gender'] = 2
						else:
							entry['species'] = -1
							entry['form'] = -1
							entry['gender'] = -1
						cardId = struct.unpack('<H', tempdata[0x150:0x152])[0]
						entry['name'] = "%03i - " % cardId + gen4string.translateG4String(tempdata[0x104:0x104+0x48]).replace("Mystery Gift ","")
						if entry['name'] == "%03i - " % cardId:
							entry['name'] = name.replace("Item ", "").replace(" " + game, "").replace(" (" + lang + ")","")
						inMatches = False
						for i in range(len(sheet['matches'])):
							if sheet['matches'][i]['id'] == cardId and sheet['matches'][i]['species'] == entry['species'] and sheet['matches'][i]['form'] == entry['form'] and sheet['matches'][i]['gender'] == entry['gender']:
								sheet['matches'][i]['indices'][lang] = len(sheet['wondercards']) - 1
								inMatches = True
						if not inMatches:
							match = {}
							match['id'] = cardId
							match['species'] = entry['species']
							match['form'] = entry['form']
							match['gender'] = entry['gender']
							match['indices'] = {}
							match['indices'][lang] = len(sheet['wondercards']) - 1
							sheet['matches'].append(match)
					elif type == 'pgt':
						if tempdata[0] == 1 or tempdata[0] == 2:
							pk4 = getWC4(tempdata)
							entry['species'] = struct.unpack('<H', pk4[0x8:0x0A])[0]
							entry['form'] = pk4[0x40] >> 3
							entry['gender'] = pk4[0x40] >> 1 & 0x3
						elif tempdata[0] == 7:
							entry['species'] = 490
							entry['form'] = -1 # special meaning for Manaphy: egg
							entry['gender'] = 2
						else:
							entry['species'] = -1
							entry['form'] = -1
							entry['gender'] = -1
						cardId = entry['name'][:3]
						if not any(elem in "1234567890" for elem in cardId):
							cardId = 999
						inMatches = False
						for i in range(len(sheet['matches'])):
							if sheet['matches'][i]['id'] == cardId and sheet['matches'][i]['species'] == entry['species'] and sheet['matches'][i]['form'] == entry['form'] and sheet['matches'][i]['gender'] == entry['gender']:
								sheet['matches'][i]['indices'][lang] = len(sheet['wondercards']) - 1
								inMatches = True
						if not inMatches:
							match = {}
							match['id'] = cardId
							match['species'] = entry['species']
							match['form'] = entry['form']
							match['gender'] = entry['gender']
							match['indices'] = {}
							match['indices'][lang] = len(sheet['wondercards']) - 1
							sheet['matches'].append(match)
					retdata += tempdata
			except Exception as e:
				pass
	return retdata

# create out directory
try:
    os.stat("./out")
except:
    os.mkdir("./out")

# import a/o update the EventsGallery
if os.path.exists("./EventsGallery"):
	print("Pulling from EventsGallery...")
	try:
		repo = git.Repo("./EventsGallery")
		repo.remotes.origin.pull()
	except git.InvalidGitRepositoryError:
		print("Repository corrupted! Cloning EventsGallery...")
		rmtree('./EventsGallery')
		try:
			git.Git(".").clone("https://github.com/projectpokemon/EventsGallery.git")
		except git.GitCommandError:
			print("Could not clone EventsGallery. Aborting...")
			exit(1)
	except git.GitCommandError:
		print("Error while pulling! Continuing with existing repo")
else:
	print("Cloning EventsGallery...")
	try:
		git.Git(".").clone("https://github.com/projectpokemon/EventsGallery.git")
	except git.GitCommandError:
		print("Could not clone EventsGallery. Aborting...")
		exit(1)

# loop generations
print("Creating data...")
for gen in range (4, 7+1):
	# set root path
	root = "./EventsGallery/Released/Gen {}/Wondercards".format(gen)

	# initialize sheet
	sheet = {}
	sheet['gen'] = str(gen)
	sheet['wondercards'] = []
	sheet['matches'] = []

	# initialize data
	data = b''

	# parse files
	data += scanDir(root, sheet, 0)
	if (gen == 7):
		data += scanDir("./EventsGallery/Unreleased/Gen 7/Movie 21", sheet, len(data))
	elif (gen == 4):
		data += scanDir("./EventsGallery/Released/Gen 4/Pokemon Ranger Manaphy Egg", sheet, len(data))
	
	# sort, then get rid of data not needed in final product
	sheet['matches'] = sorted(sheet['matches'], key=sortById)
	for i in range(len(sheet['matches'])):
		temp = sheet['matches'][i]['indices']
		sheet['matches'][i] = temp
		sheet['matches'][i]
		
	# export sheet
	compressed = bz2.compress(str.encode(json.dumps(sheet)))
	with open("./out/sheet{}.json.bz2".format(gen), 'wb') as f:
		f.write(compressed)

	# sheet sha256
	sha = hashlib.sha256(compressed).digest()
	with open("./out/sheet{}.json.bz2.sha".format(gen), 'wb') as f:
		f.write(sha)

	# export data
	compressed = bz2.compress(data)
	with open("./out/data{}.bin.bz2".format(gen), 'wb') as f:
		f.write(compressed)

	# data sha256
	sha = hashlib.sha256(compressed).digest()
	with open("./out/data{}.bin.bz2.sha".format(gen), 'wb') as f:
		f.write(sha)
