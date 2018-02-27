; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CFileTransfersDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "FileTransfers.h"

ClassCount=3
Class1=CFileTransfersApp
Class2=CFileTransfersDlg
Class3=CAboutDlg

ResourceCount=3
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_FILETRANSFERS_DIALOG

[CLS:CFileTransfersApp]
Type=0
HeaderFile=FileTransfers.h
ImplementationFile=FileTransfers.cpp
Filter=N

[CLS:CFileTransfersDlg]
Type=0
HeaderFile=FileTransfersDlg.h
ImplementationFile=FileTransfersDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=IDC_RADIO_SERVER

[CLS:CAboutDlg]
Type=0
HeaderFile=FileTransfersDlg.h
ImplementationFile=FileTransfersDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=5
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Control5=IDC_STATIC,static,1342308352

[DLG:IDD_FILETRANSFERS_DIALOG]
Type=1
Class=CFileTransfersDlg
ControlCount=20
Control1=IDC_STATIC,button,1342177287
Control2=IDC_RADIO_SERVER,button,1342373897
Control3=IDC_RADIO_CLIENT,button,1342242825
Control4=IDC_STATIC,static,1342308352
Control5=IDC_IPADDRESS,SysIPAddress32,1342242816
Control6=IDC_STATIC,static,1342308352
Control7=IDC_PORT,edit,1350631552
Control8=IDC_BEGIN,button,1342242816
Control9=IDC_DISCONNECT,button,1476460544
Control10=IDC_STATIC,static,1350565895
Control11=IDC_TRANSFERS_TIP,static,1342308352
Control12=IDC_PROGRESS,msctls_progress32,1350565889
Control13=IDC_SELECT_FILE,button,1476460544
Control14=IDC_STOP_TRANSFERS,button,1476460544
Control15=IDCANCEL,button,1342242816
Control16=IDC_STATIC,static,1342308352
Control17=IDC_STATIC,static,1342308352
Control18=IDC_FILE_NAME,static,1342308876
Control19=IDC_RECEIVE_SIZE,static,1342308864
Control20=IDC_FILE_SIZE,static,1342308864

