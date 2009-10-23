@echo off

if "%1" == "" goto NoPath

if not exist "%1" goto CheckPathValidable
goto ValidPath

:CheckPathValidable
if not exit "%cd%/%1" goto InvalidPath
goto ValidPath

:ValidPath
echo Dest CAB file path: %1

rem 将打包时需要的参数写入配置文件
echo .OPTION EXPLICIT 							>  MakeCabPack.tmp
echo .Set CabinetNameTemplate=Package.cab		>> MakeCabPack.tmp
echo .set DiskDirectoryTemplate=. 				>> MakeCabPack.tmp
echo .Set CompressionType=LZX 					>> MakeCabPack.tmp
echo .Set MaxCabinetSize=0 						>> MakeCabPack.tmp
echo .Set MaxDiskFileCount=0 					>> MakeCabPack.tmp
echo .Set MaxDiskSize=0 						>> MakeCabPack.tmp
echo .Set UniqueFiles="OFF" 					>> MakeCabPack.tmp
echo .Set Cabinet=on 							>> MakeCabPack.tmp
echo .Set DiskDirectory1=%1						>> MakeCabPack.tmp

rem 将指定文件夹下的文件文件写入文件列表
echo .Set DestinationDir="" 					>> MakeCabPack.tmp
for %%f in (%1\*.*) do echo %%f 				>> MakeCabPack.tmp

rem 将目录也添加到文件列表
cd %1
for /D %%d in (*) do (
echo .Set DestinationDir="%%d" 					>> ..\MakeCabPack.tmp
for %%f in (%%d\*.*) do echo %1\%%f 			>> ..\MakeCabPack.tmp
)
cd..

echo make info file succeeded, begin make cab file...

makecab /F MakeCabPack.tmp
del setup.inf
del setup.rpt
del MakeCabPack.tmp

goto Succeeded

:InvalidPath
echo Invalid path
goto Succeeded

:NoPath
echo No path specified
goto Succeeded

:Succeeded
pause