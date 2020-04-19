#include <algorithm>
#include <array>
#include <assert.h>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <stdio.h>
#include <string>
#include <vector>

enum Lang : int
{
    CHS,
    CHT,
    ENG,
    FRE,
    GER,
    ITA,
    JPN,
    KOR,
    SPA,
    FINAL_VAL
};

struct LinePair
{
    int pkhexNo;
    int pksmNo;
};

std::string pkhexLangString(Lang l)
{
    switch (l)
    {
        case CHS:
        case CHT:
            return "zh";
        case ENG:
            return "en";
        case FRE:
            return "fr";
        case GER:
            return "de";
        case ITA:
            return "it";
        case JPN:
            return "ja";
        case KOR:
            return "ko";
        case SPA:
            return "es";
        default:
            return "";
    }
}

std::string pksmLangString(Lang l)
{
    switch (l)
    {
        case CHS:
            return "chs";
        case CHT:
            return "cht";
        case ENG:
            return "eng";
        case FRE:
            return "fre";
        case GER:
            return "ger";
        case ITA:
            return "ita";
        case JPN:
            return "jpn";
        case KOR:
            return "kor";
        case SPA:
            return "spa";
        default:
            return "";
    }
}

// string, PKHeX line number(s)
std::vector<std::pair<std::string, std::vector<int>>> readFile(Lang l)
{
    std::vector<std::pair<std::string, std::vector<int>>> ret;

    std::string pkhexName = "text_Forms_" + pkhexLangString(l) + ".txt";

    std::ifstream in(pkhexName);
    assert(!in.bad());

    std::string tmp;
    int line = 0;
    while (std::getline(in, tmp))
    {
        tmp = tmp.substr(0, tmp.find('\n'));
        tmp = tmp.substr(0, tmp.find('\r'));
        if (!tmp.empty())
        {
            auto it = std::find_if(ret.begin(), ret.end(), [&tmp](auto v) { return v.first == tmp; });
            if (it != ret.end())
            {
                it->second.emplace_back(line);
            }
            else
            {
                ret.emplace_back(tmp, std::vector<int>{{line}});
            }
        }
        line++;
    }

    in.close();
    return ret;
}

void writeFile(Lang l, const std::vector<std::pair<std::string, std::vector<int>>>& array)
{
    std::string formsFile = "forms-" + pksmLangString(l) + ".txt";

    std::ofstream outForms(formsFile);
    for (auto i = array.begin(); i != array.end(); i++)
    {
        outForms << i->first << '\n';
    }
    outForms.close();
}

int main()
{
    std::array<std::vector<std::pair<std::string, std::vector<int>>>, FINAL_VAL> files;
    std::array<std::vector<std::pair<std::string, std::vector<int>>>, FINAL_VAL> result;

    for (Lang l = CHS; l < FINAL_VAL; l = Lang(l + 1))
    {
        files[l] = readFile(l);
    }

    for (const auto& str : files[ENG])
    {
        if (str.second.size() > 1)
        {
            std::vector<int> dupedInAllLines = str.second;
            for (Lang l = CHS; l < FINAL_VAL; l = Lang(l + 1))
            {
                if (l == ENG)
                    continue;
                for (const auto& otherStr : files[l])
                {
                    if (otherStr.second[0] != str.second[0])
                        continue;

                    for (size_t i = 1; i < dupedInAllLines.size(); i++)
                    {
                        if (std::find(otherStr.second.begin() + 1, otherStr.second.end(), dupedInAllLines[i]) == otherStr.second.end())
                        {
                            dupedInAllLines.erase(dupedInAllLines.begin() + i);
                            i -= 1;
                        }
                    }
                    // Can only happen once
                    break;
                }
            }

            for (Lang l = CHS; l < FINAL_VAL; l = Lang(l + 1))
            {
                if (l == ENG)
                    continue;
                for (auto& otherStr : files[l])
                {
                    if (otherStr.second[0] != dupedInAllLines[0])
                        continue;

                    for (size_t i = 1; i < dupedInAllLines.size(); i++)
                    {
                        auto it = std::find(otherStr.second.begin() + 1, otherStr.second.end(), dupedInAllLines[i]);
                        if (it != otherStr.second.end())
                        {
                            otherStr.second.erase(it);
                        }
                    }
                }
            }
        }
    }

    // bool done = false;
    // for (const auto& dupe : dupes[ENG])
    // {
    //     bool isDupedInAll = true;
    //     for (Lang l = CHS; l < FINAL_VAL; l = Lang(l + 1))
    //     {
    //         if (l == ENG)
    //             continue;
    //         auto comp = [&files, &dupe, &l](const std::pair<std::list<std::pair<int, std::string>>::const_iterator,
    //                         std::list<std::pair<int, std::string>>::const_iterator>& pair) {
    //             return std::distance(files[ENG].cbegin(), dupe.first) == std::distance(files[l].cbegin(), pair.first) &&
    //                    std::distance(files[ENG].cbegin(), dupe.second) == std::distance(files[l].cbegin(), pair.second);
    //         };
    //         if (std::find_if(dupes[l].begin(), dupes[l].end(), comp) == dupes[l].end())
    //         {
    //             isDupedInAll = false;
    //             break;
    //         }
    //     }

    //     if (isDupedInAll)
    //     {
    //         for (Lang l = CHS; l < FINAL_VAL; l = Lang(l + 1))
    //         {
    //             auto eraseMe = files[l].begin();
    //             while (std::distance(files[l].begin(), eraseMe) != std::distance(files[ENG].cbegin(), dupe.second))
    //             {
    //                 eraseMe++;
    //             }
    //             files[l].erase(eraseMe);
    //         }
    //     }
    // }

    for (Lang l = CHS; l < FINAL_VAL; l = Lang(l + 1))
    {
        writeFile(l, files[l]);
    }

    std::ofstream out("formConversionKey.txt");
    size_t currentLine = 0;
    for (auto i = files[ENG].begin(); i != files[ENG].end(); i++)
    {
        for (size_t j = 0; j < i->second.size(); j++)
        {
            out << i->second[j] << "->" << currentLine << '\n';
        }
        currentLine++;
    }
    out.close();

    return 0;
}
