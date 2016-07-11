
cd D:/code/underworld_client/script

for %%i in (xml_data/TechTreeData/*.xml) do AESCTREncryptor.exe xml_data/TechTreeData/%%i ../Resources/configs/TechTreeData/%%i

for %%i in (xml_data/MapData/*.xml) do AESCTREncryptor.exe xml_data/MapData/%%i ../Resources/configs/MapData/%%i

for %%i in (xml_data/*.xml) do AESCTREncryptor.exe xml_data/%%i ../Resources/configs/%%i

python android.py