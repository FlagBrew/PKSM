package main

import (
	"fmt"
	"io/ioutil"
	"net/http"
	"os"
	"strings"
	"time"
)

var bw2 = []string{
	"https://raw.githubusercontent.com/kwsch/PKHeX/master/PKHeX.Core/Resources/text/%s/text_bw2_00000_%s.txt",
	"https://raw.githubusercontent.com/kwsch/PKHeX/master/PKHeX.Core/Resources/text/%s/text_bw2_30000_%s.txt",
	"https://raw.githubusercontent.com/kwsch/PKHeX/master/PKHeX.Core/Resources/text/%s/text_bw2_40000_%s.txt",
	"https://raw.githubusercontent.com/kwsch/PKHeX/master/PKHeX.Core/Resources/text/%s/text_bw2_60000_%s.txt",
}

var hgss = []string{
	"https://raw.githubusercontent.com/kwsch/PKHeX/master/PKHeX.Core/Resources/text/%s/text_hgss_00000_%s.txt",
	"https://raw.githubusercontent.com/kwsch/PKHeX/master/PKHeX.Core/Resources/text/%s/text_hgss_02000_%s.txt",
	"https://raw.githubusercontent.com/kwsch/PKHeX/master/PKHeX.Core/Resources/text/%s/text_hgss_03000_%s.txt",
}

var sm = []string{
	"https://raw.githubusercontent.com/kwsch/PKHeX/master/PKHeX.Core/Resources/text/%s/text_sm_00000_%s.txt",
	"https://raw.githubusercontent.com/kwsch/PKHeX/master/PKHeX.Core/Resources/text/%s/text_sm_30000_%s.txt",
	"https://raw.githubusercontent.com/kwsch/PKHeX/master/PKHeX.Core/Resources/text/%s/text_sm_40000_%s.txt",
	"https://raw.githubusercontent.com/kwsch/PKHeX/master/PKHeX.Core/Resources/text/%s/text_sm_60000_%s.txt",
}

var xy = []string{
	"https://raw.githubusercontent.com/kwsch/PKHeX/master/PKHeX.Core/Resources/text/%s/text_xy_00000_%s.txt",
	"https://raw.githubusercontent.com/kwsch/PKHeX/master/PKHeX.Core/Resources/text/%s/text_xy_30000_%s.txt",
	"https://raw.githubusercontent.com/kwsch/PKHeX/master/PKHeX.Core/Resources/text/%s/text_xy_40000_%s.txt",
	"https://raw.githubusercontent.com/kwsch/PKHeX/master/PKHeX.Core/Resources/text/%s/text_xy_60000_%s.txt",
}

var lgpe = []string{
	"https://raw.githubusercontent.com/kwsch/PKHeX/master/PKHeX.Core/Resources/text/%s/text_gg_00000_%s.txt",
	"https://raw.githubusercontent.com/kwsch/PKHeX/master/PKHeX.Core/Resources/text/%s/text_gg_40000_%s.txt",
}

var swsh = []string{
	"https://raw.githubusercontent.com/kwsch/PKHeX/master/PKHeX.Core/Resources/text/%s/text_swsh_00000_%s.txt",
	"https://raw.githubusercontent.com/kwsch/PKHeX/master/PKHeX.Core/Resources/text/%s/text_swsh_30000_%s.txt",
	"https://raw.githubusercontent.com/kwsch/PKHeX/master/PKHeX.Core/Resources/text/%s/text_swsh_40000_%s.txt",
	"https://raw.githubusercontent.com/kwsch/PKHeX/master/PKHeX.Core/Resources/text/%s/text_swsh_60000_%s.txt",
}

var games = [][]string{
	bw2, hgss, sm, xy, lgpe, swsh,
}

var langs = []string{
	"de",
	"en",
	"es",
	"fr",
	"it",
	"ja",
	"ko",
	"zh",
}

func main() {
	bw2Locations := [][]string{}
	hgssLocations := [][]string{}
	smLocations := [][]string{}
	xyLocations := [][]string{}
	lgpeLocations := [][]string{}
	swshLocations := [][]string{}
	for _, lang := range langs {
		for i, game := range games {
			tmpArray := []string{}
			for _, url := range game {
				fmt.Println(fmt.Sprintf("Fetching %s!", fmt.Sprintf(url, lang, lang)))
				resp, err := http.Get(fmt.Sprintf(url, lang, lang))
				if err != nil {
					panic(err)
				}
				defer resp.Body.Close()
				contents, err := ioutil.ReadAll(resp.Body)
				if err != nil {
					panic(err)
				}
				tmpArray2 := strings.Split(string(contents), "\n")
				for ii, location := range tmpArray2 {
					num := ""
					if location == "" || location == " " || location == "\n" {
						continue
					}
					if strings.Contains(url, "_00000_") {
						num = fmt.Sprintf("%x", ii)
					} else if strings.Contains(url, "_30000_") {
						num = fmt.Sprintf("%x", 30000+ii+1)
					} else if strings.Contains(url, "_40000_") {
						num = fmt.Sprintf("%x", 40000+ii+1)
					} else if strings.Contains(url, "_60000_") {
						num = fmt.Sprintf("%x", 60000+ii+1)
					} else if strings.Contains(url, "_02000_") {
						num = fmt.Sprintf("%x", 2000+ii)
					} else if strings.Contains(url, "_03000_") {
						num = fmt.Sprintf("%x", 3000+ii)
					}
					switch len(num) {
					case 1:
						placeholder := "0x000"
						num = fmt.Sprintf("%s%s", placeholder, num)
					case 2:
						placeholder := "0x00"
						num = fmt.Sprintf("%s%s", placeholder, num)
					case 3:
						placeholder := "0x0"
						num = fmt.Sprintf("%s%s", placeholder, num)
					case 4:
						placeholder := "0x"
						num = fmt.Sprintf("%s%s", placeholder, num)
					}
					tmpArray = append(tmpArray, fmt.Sprintf("%s|%s\n", num, location))
				}
			}
			switch i {
			case 0:
				bw2Locations = append(bw2Locations, tmpArray)
			case 1:
				hgssLocations = append(hgssLocations, tmpArray)
			case 2:
				smLocations = append(smLocations, tmpArray)
			case 3:
				xyLocations = append(xyLocations, tmpArray)
			case 4:
				lgpeLocations = append(lgpeLocations, tmpArray)
			case 5:
				swshLocations = append(swshLocations, tmpArray)
			}
			time.Sleep(time.Millisecond * 500)
		}
	}
	for i, l := range bw2Locations {
		f, _ := os.Create(fmt.Sprintf("bw2Locations_%s.txt", langs[i]))
		defer f.Close()
		for _, loc := range l {
			f.Write([]byte(loc))
		}
	}
	for i, l := range hgssLocations {
		f, _ := os.Create(fmt.Sprintf("hgssLocations_%s.txt", langs[i]))
		defer f.Close()
		for _, loc := range l {
			f.Write([]byte(loc))
		}
	}
	for i, l := range smLocations {
		f, _ := os.Create(fmt.Sprintf("smLocations_%s.txt", langs[i]))
		defer f.Close()
		for _, loc := range l {
			f.Write([]byte(loc))
		}
	}
	for i, l := range xyLocations {
		f, _ := os.Create(fmt.Sprintf("xyLocations_%s.txt", langs[i]))
		defer f.Close()
		for _, loc := range l {
			f.Write([]byte(loc))
		}
	}
	for i, l := range lgpeLocations {
		f, _ := os.Create(fmt.Sprintf("lgpeLocations_%s.txt", langs[i]))
		defer f.Close()
		for _, loc := range l {
			f.Write([]byte(loc))
		}
	}
	for i, l := range swshLocations {
		f, _ := os.Create(fmt.Sprintf("swshLocations_%s.txt", langs[i]))
		defer f.Close()
		for _, loc := range l {
			f.Write([]byte(loc))
		}
	}
}
