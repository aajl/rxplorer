::说明: 传入的路径后面必须带\

@echo off

::如果不加这句,for里的set strPath=%%d就会无效
setlocal EnableDelayedExpansion

::构造CAB文件的输出路径
set strPath=%2
if strPath=="" set strPath=%1Package.cab

echo .OPTION EXPLICIT 							>  MakeCab.tmp
echo .Set CabinetNameTemplate=%strPath%			>> MakeCab.tmp
echo .set DiskDirectoryTemplate=. 				>> MakeCab.tmp
echo .Set CompressionType=LZX 					>> MakeCab.tmp
echo .Set MaxCabinetSize=0 						>> MakeCab.tmp
echo .Set MaxDiskFileCount=0 					>> MakeCab.tmp
echo .Set MaxDiskSize=0 						>> MakeCab.tmp
echo .Set UniqueFiles="OFF" 					>> MakeCab.tmp
echo .Set Cabinet=on 							>> MakeCab.tmp
echo .Set DiskDirectory1=. 						>> MakeCab.tmp

echo .Set DestinationDir="" 					>> MakeCab.tmp
::构造所有需要打包的文件
for %%f in (%1*.*) do echo %%f 					>> MakeCab.tmp

::计算传入路径的长度
set pathLen = 0
set strPath=%1
:next1
if not "%strPath%"=="" (
set /a pathLen+=1
set strPath=%strPath:~1%
goto next1
)

::构造需要打包的子目录,并构造子目录下面的文件
for /D /R %1 %%d in (*) do (
set strPath=%%d
echo .Set DestinationDir=!strPath:~%pathLen%!	>> MakeCab.tmp
for %%f in (%%d\*.*) do echo %%f				>> MakeCab.tmp
)

makecab.exe /F MakeCab.tmp
del setup.inf
del setup.rpt
del makecab.tmp