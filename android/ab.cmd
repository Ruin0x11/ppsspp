xcopy ..\flash0 assets\flash0 /s /y <d.txt
xcopy ..\lang assets\lang /s /y <d.txt
xcopy ..\assets\shaders assets\shaders /s /y <d.txt
copy ..\assets\langregion.ini assets\langregion.ini
copy ..\assets\compat.ini assets\compat.ini
copy ..\assets\*.png assets
SET NDK=C:\AndroidNDK
SET NDK_MODULE_PATH=..\ext;..\ext\native\ext
%NDK%/ndk-build -j9 %*
