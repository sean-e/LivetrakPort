@echo off

rem update LivetrakPortExe to point to whereever you copied or built LivetrakPort.exe
set LivetrakPortExe=D:\Apps\LivetrakPort\LivetrakPort.exe

rem update L20Template to point to your own L-20 template or to the one provided with LivetrakPort.exe
set L20Template=D:\Apps\LivetrakPort\L20Template.ZDT

rem add channel rename arguments if you want to change the default channel names; otherwise, leave blank
rem set RenameArgs=-r1=Vox -r2=Bass -r3=KeysL -r4=KeysR -r5=Extra1 -r6=Extra2 -r7=GtrL -r8=GtrR -r17=Drums -r18=Drums
set RenameArgs=

rem set StereoTracks to either c (copy), r (rename), x (ignore/nothing) to control migration of L-12 stereo tracks to L-20 stereo channels
set StereoTracks=c
rem set StereoTracks=r
rem set StereoTracks=x


rem use "del /s L20PRJDATA.ZDT" if you want to rerun the port with different arguments 
rem if you copied or renamed the stereo tracks in the first run, then you can use the ignore (x) option for subsequents reruns


for /R %%i in (PRJDATA*.ZDT) do %LivetrakPortExe% -t=%L20Template% -m=%StereoTracks% %RenameArgs% -s=%%i



rem If you no longer need the L-12 data, then you can delete the project files and stereo tracks:
rem del /s PRJDATA.ZDT
rem del /s TRACK09_10.WAV
rem del /s TRACK11_12.WAV
