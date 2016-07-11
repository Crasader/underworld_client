#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys
import os



if __name__ == '__main__':
    rootdir = "../Classes/"
    dirList = []
    fileList = []
    dirList.append("jni/../../Classes")
    # dirList.append(os.path.relpath(os.path.join(fpathe, d), rootdir))
    for fpathe,dirs,files in os.walk(rootdir):
        for d in dirs:
            dirList.append("jni/" + os.path.relpath(os.path.join(fpathe, d), "../proj.android/jni/").replace("\\", "/"))
        for f in files:
            if f.endswith(".c") or f.endswith(".cpp"):
                fileList.append(os.path.relpath(os.path.join(fpathe, f), "../proj.android/jni/").replace("\\", "/"))

    fileList.append("underworld/main.cpp")
    dirContent = " ".join(dirList)
    fileContent =  " ".join(fileList)

    file = open("../proj.android/jni/Android.mk", 'r+')
    contentList = []
    flag = False
    for line in reversed(file.readlines()):
        if line.startswith("LOCAL_C_INCLUDES := "):
            contentList.insert(0, "LOCAL_C_INCLUDES := " + dirContent + " \\")
        elif not flag and line.startswith("LOCAL_SRC_FILES := "):
            flag = True
            contentList.insert(0, "LOCAL_SRC_FILES := " + fileContent)
        else:
            contentList.insert(0, line.rstrip())
    file.seek(0)
    srcContent = file.read().strip()
    newContent = ("\n".join(contentList)).strip()
    if cmp(srcContent, newContent) == 0:
        print "no change"
        exit()
    file.seek(0)
    file.truncate()
    file.write(newContent)
    file.flush()
    file.close()