#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys
import os
from xml.dom.minidom import Document

encoding = "utf-8"

if __name__ == "__main__":
    doc = Document()
    root = doc.createElement("root")
    doc.appendChild(root)
    startdir = sys.argv[1]
    rootdir = os.path.dirname(startdir)
    for fpathe,dirs,files in os.walk(startdir):
        for f in files:
            if f.lower().endswith(".plist"):
                element = doc.createElement("item")
                element.setAttribute("name", os.path.relpath(os.path.join(fpathe, f), rootdir))
                root.appendChild(element)

    file = open(sys.argv[2], "wb")
    doc.writexml(file, addindent='\t', newl='\n')
    file.close()