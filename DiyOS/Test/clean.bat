
attrib +h .\Test\MDK-ARM\Test\*.hex
attrib +h .\Test\MDK-ARM\Test\*.map
attrib +h .\Test\MDK-ARM\Test\*.sct

del /Q /A-H .\Test\MDK-ARM\Test\*.*

attrib -h .\Test\MDK-ARM\Test\*.hex
attrib -h .\Test\MDK-ARM\Test\*.map
attrib -h .\Test\MDK-ARM\Test\*.sct