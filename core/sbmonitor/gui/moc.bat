set MOC=..\..\..\lib\qt\bin\moc.exe  -DUNICODE -DWIN32 -DQT_LARGEFILE_SUPPORT -DQT_DLL -DQT_GUI_LIB -DQT_CORE_LIB -DQT_HAVE_MMX -DQT_HAVE_3DNOW -DQT_HAVE_SSE -DQT_HAVE_MMXEXT -DQT_HAVE_SSE2 -DQT_THREAD_SUPPORT -I"..\vh\qt\include\QtCore" -I"..\vh\qt\include\QtGui" -I"..\vh\qt\include" -I"." -I"..\vh\qt\include\ActiveQt" -I"..\vh\qt\mkspecs\win32-msvc2008" -D_MSC_VER=1500 -DWIN32

%MOC% ConnectDialog.h   -o moc_ConnectDialog.cpp
%MOC% glwidget.h        -o moc_glwidget.cpp
%MOC% SbmDebuggerForm.h -o moc_SbmDebuggerForm.cpp
%MOC% SettingsDialog.h -o moc_SettingsDialog.cpp
%MOC% ResourceDialog.h -o moc_ResourceDialog.cpp
%MOC% CommandDialog.h -o moc_CommandDialog.cpp
%MOC% DataViewerDialog.h -o moc_DataViewerDialog.cpp
%MOC% UtilsDialog.h -o moc_UtilsDialog.cpp
%MOC% FaceDialog.h -o moc_FaceDialog.cpp
%MOC% BMLCreatorDialog.h -o moc_BMLCreatorDialog.cpp
%MOC% EntityCreatorDialog.h -o moc_EntityCreatorDialog.cpp

set UIC=..\..\..\lib\qt\bin\uic.exe
%UIC% designer\BMLCreatorDialog.ui -o ui_BMLCreatorDialog.h
%UIC% designer\CommandDialog.ui -o ui_CommandDialog.h
%UIC% designer\ConnectDialog.ui -o ui_ConnectDialog.h
%UIC% designer\DataViewerDialog.ui -o ui_DataViewerDialog.h
%UIC% designer\EntityCreatorDialog.ui -o ui_EntityCreatorDialog.h
%UIC% designer\FaceDialog.ui -o ui_FaceDialog.h
%UIC% designer\ResourceDialog.ui -o ui_ResourceDialog.h
%UIC% designer\SbmDebuggerForm.ui -o ui_SbmDebuggerForm.h
%UIC% designer\SettingsDialog.ui -o ui_SettingsDialog.h
%UIC% designer\UtilsDialog.ui -o ui_UtilsDialog.h
