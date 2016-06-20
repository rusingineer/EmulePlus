; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[Setup]
AllowNoIcons=yes
AppName=eMule Plus Configuration Update
AppPublisher=eMule Plus Team
AppPublisherURL=http://emuleplus.info
AppSupportURL=http://emuleplus.info/forum/index.php?showforum=23
AppUpdatesURL=http://emuleplus.info
Compression=lzma/max
DefaultDirName={pf}\eMule
DefaultGroupName=eMule
InfoBeforeFile=..\files\update.txt
PrivilegesRequired=admin
SolidCompression=yes
VersionInfoDescription=eMule Plus, The eye candy eMule client
WizardImageFile=..\res\Graphics\InstallerScreen.bmp
; Values to be changed with each new release
AppVerName=eMule Plus 1.2c Configuration Update
VersionInfoVersion=1.2.3.0
OutputBaseFilename=eMulePlus-1.2c.Update

[Files]
Source: "..\files\config\addresses.dat"; DestDir: "{app}\Config"; Flags: confirmoverwrite uninsneveruninstall
Source: "..\files\config\ipfilter.dat"; DestDir: "{app}\Config"; Flags: confirmoverwrite uninsneveruninstall
Source: "..\files\config\staticservers.dat"; DestDir: "{app}\Config"; Flags: confirmoverwrite uninsneveruninstall
Source: "..\files\config\webservices.dat"; DestDir: "{app}\Config"; Flags: confirmoverwrite uninsneveruninstall

;[InstallDelete]
;Type: files; Name: "{app}\Config\server.met"
