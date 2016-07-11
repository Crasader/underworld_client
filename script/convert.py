#!/usr/bin/env python
# -*- coding: utf-8 -*-
import xlrd
#import lxml
import sys
from xml.dom.minidom import Document

encoding = "utf-8"
#def sheet2elements(root, sheet):

if __name__ == "__main__":
    doc = Document()
    root = doc.createElement("root")
    doc.appendChild(root)
    # xlsfile = xlrd.open_workbook("/Users/MacUser/csdn/underworld_config/核心战斗表.xlsx")
    xlsfile = xlrd.open_workbook(sys.argv[1])
    for sheet in xlsfile.sheets():
        sheetname = sheet.name
        attrnames = sheet.row_values(0)
        for i in xrange(1, sheet.nrows):
            datas = sheet.row_values(i)
            element = doc.createElement(sheetname)
            for j in xrange(len(attrnames)):
                data = datas[j]
                if not isinstance(data, unicode):
                    if isinstance(data, float):
                        data = int(data)
                    data = str(data)
                else:
                    data = data.encode(encoding)
                if data == "":
                    continue
                attrname = attrnames[j]
                element.setAttribute(attrname, data)
            root.appendChild(element)

    # print doc.toprettyxml()

    # file = open("/Users/MacUser/csdn/underworld_config/AAALL.xml", "w")
    file = open(sys.argv[2], "w")
    doc.writexml(file, addindent='\t', newl='\n')
    file.close()

    exit(1)

#创建workbook和sheet对象
workbook = xlwt.Workbook() #注意Workbook的开头W要大写
sheet1 = workbook.add_sheet('1',cell_overwrite_ok=True)
sheet2 = workbook.add_sheet('2',cell_overwrite_ok=True)
#向sheet页中写入数据
sheet1.write(0,0,'this should overwrite1')
sheet1.write(0,1,'aaaaaaaaaaaa')
sheet2.write(0,0,'this should overwrite2')
sheet2.write(1,2,'bbbbbbbbbbbbb')
"""
#-----------使用样式-----------------------------------
#初始化样式
style = xlwt.XFStyle()
#为样式创建字体
font = xlwt.Font()
font.name = 'Times New Roman'
font.bold = True
#设置样式的字体
style.font = font
#使用样式
sheet.write(0,1,'some bold Times text',style)
"""
#保存该excel文件,有同名文件时直接覆盖
workbook.save('/Users/MacUser/csdn/underworld_config/test2.xls')
print '创建excel文件完成！'