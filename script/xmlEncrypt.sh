#! /bin/bash
dir=$(cd `dirname $0`;pwd)
cd $dir
project_root=`dirname $dir`
echo $project_root
cd $dir/xml_data
for _f in `ls *.xml`;do
	echo $_f
	$dir/CTREncryptor $_f $project_root/Resources/configs/$_f
done
cd $dir/xml_data/MapData
for _f in `ls *.xml`;do
	echo $_f
	$dir/CTREncryptor $_f $project_root/Resources/configs/MapData/$_f
done
cd $dir/xml_data/TechTreeData
for _f in `ls *.xml`;do
	echo $_f
	$dir/CTREncryptor $_f $project_root/Resources/configs/TechTreeData/$_f
done
