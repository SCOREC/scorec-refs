#!/usr/bin/env python

#http://tex.stackexchange.com/questions/23410/how-to-convert-characters-to-latex-code

import unicodedata
import sys

accents = {
    0x0300: '`', 0x0301: "'", 0x0302: '^', 0x0308: '"',
    0x030B: 'H', 0x0303: '~', 0x0327: 'c', 0x0328: 'k',
    0x0304: '=', 0x0331: 'b', 0x0307: '.', 0x0323: 'd',
    0x030A: 'r', 0x0306: 'u', 0x030C: 'v',
}

def uni2tex(text):
    out = ""
    txt = tuple(text)
    i = 0
    while i < len(txt):
        char = text[i]
        code = ord(char)

        # Elsevier bibtex dumps sometimes have a fancy dash
        if code == 8211:
            out += "-"
        # combining marks
        elif unicodedata.category(char) in ("Mn", "Mc") and code in accents:
            out += "\\%s{%s}" %(accents[code], txt[i+1])
            i += 1
        # precomposed characters
        elif unicodedata.decomposition(char):
            base, acc = unicodedata.decomposition(char).split()
            acc = int(acc, 16)
            base = int(base, 16)
            if acc in accents:
                out += "\\%s{%s}" %(accents[acc], unichr(base))
            else:
                out += char
        else:
            out += char

        i += 1

    return out

if __name__ == '__main__':
    fi = open(sys.argv[1], 'r')
    ti = unicode(fi.read(), "utf-8-sig")
    fi.close()
    to = uni2tex(ti)
    fo = open(sys.argv[2], 'w')
    fo.write(to)
    fo.close()
