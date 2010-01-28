# auto increment file version and product version
# author: tristar lu
# usage:
# Pre-Link Event --> Command Line
# AutoIncVer.py $(ProjectDir)$(ProjectName).rc

import sys

def increment_version(raw_data, ver_tag, delimiter, mark, ver_type):
    while(True):
        ver_pos = raw_data.find(ver_tag)
        if ver_pos == -1:
            break;
    
        ver_end_pos = raw_data[ver_pos:].find('\n')
        if ver_end_pos == -1:
            break;

        ver_tag_len = len(ver_tag) + 1
        old_ver = raw_data[ver_pos : ver_pos + ver_end_pos]
        old_ver_num = raw_data[ver_pos + ver_tag_len : ver_pos + ver_end_pos]
        old_ver_num = old_ver_num.replace(mark, '')
        print ' <python> Old ' + ver_type + ' version is: ' + old_ver_num
        ver_list = old_ver_num.split(delimiter)
        ver_list[len(ver_list) - 1] = str(int(ver_list[len(ver_list) - 1]) + 1)
        new_ver = delimiter.join(ver_list)
        print ' <python> New ' + ver_type + ' version is: ' + new_ver
        new_ver = ver_tag + ' ' + mark + new_ver + mark
        raw_data = raw_data.replace(old_ver, new_ver)
        return raw_data
    
        break
    
if len(sys.argv) < 2:
    sys.exit()

rc_path = sys.argv[1]
print ' <python> Begin increment build version: ' + rc_path

try:
    file = open(rc_path, 'r+')
except:
    print ' <python> Open resource file failed'
    sys.exit()

rc_data = file.read(100 * 1024)
rc_data = increment_version(rc_data, 'FILEVERSION', ',', '', 'file')
rc_data = increment_version(rc_data, 'PRODUCTVERSION', ',', '', 'product')

rc_data = increment_version(rc_data, '''"FileVersion",''', '.', '''"''', 'file')
rc_data = increment_version(rc_data, '''"ProductVersion",''', '.', '''"''', 'file')

if len(rc_data) <= 0:
    sys.exit()
    
file.seek(0)
file.write(rc_data)
