' Run with a command like:
' cscript "elins.vbs" scxom64-rhel52-03 22 "su root -c" "rpm -U /tmp/rhel5.rpm" "rhel5.rpm"

                    '  
                    '  Copyright (c) Microsoft Corporation.  All rights reserved.
                    '
                    ' Exit Codes:
                    '   0 = Success
                    '   1 = Failure
                    '   

                    Option Explicit

		    Function FixCR(origStr)
		      Dim regEx, intStr               ' Create variables.
		      Set regEx = New RegExp            ' Create regular expression.
		      regEx.Pattern = "\r([^\n])"            ' Set pattern.
                      regEx.Global = True
		      intStr = regEx.Replace(origStr, vbCrLf + "$1")   ' Make replacement.

                      regEx.Pattern = "[\x01\x02\x03\x04\x05\x06\x07\x08\x0B\x0C\x0E\x0F\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F]"
                      'regEx.Pattern = "#x0*(0|1|2|3|4|5|6|7|8|B|C|E|F|10|11|12|13|14|15|16|17|18|19|1A|1B|1C|1D|1E|1F)"
                      FixCR = regEx.Replace(intStr, "")
		    End Function

                    Dim oArgs, SecureInput
                    Set oArgs = WScript.Arguments
                    'SecureInput = Split(WScript.StdIn.ReadLine(), " ")

                    'If (oArgs.Count < 5) OR (UBound(SecureInput) < 2) Then
		    If (oArgs.Count < 5) Then
                       WScript.Echo "Usage: SSH.vbs Host Port SUCommand Command InstallPackage"
                       WScript.Echo "Secure Input: Username Password SUPassword"
                       Wscript.Quit 1
                    End If

                    Dim TmpCommand, Host, Username, Password, SUPassword, Port, SUCommand, Command, InstallPackage
                    Host = oArgs(0)
                    Port = oArgs(1)
                    SUCommand = oArgs(2)
                    Command = oArgs(3)
                    InstallPackage = oArgs(4)

                    Username = "scxuser"
                    Password = "scxuser"
                    SUPassword = "OpsMgr2007R2"

                    Dim scxssh
                    Dim Status, Result

                    ' Enable error handling
                    On Error Resume Next

                    'Create SSH Object
                    Set scxssh = CreateObject("sshcom.scxssh")

                    'Set Connection info
                    scxssh.ConnectWithPassword Host, Port, Username, Password

                    'Open a shell for execution
		    Status = 1
                    Status = scxssh.OpenShell()
                    Result = scxssh.GetResponse()
                    WScript.Echo FixCR(Result)

                    ' Handle Failed Connection
                    If CInt(Status) <> 0 then WScript.Quit CInt(Status)

                    Dim sStart, sEnd, sCompressionSuffix, sUnCompressCommand, sInstallCommand, sCheckFileCommand, sRemoveCommand

                    'Check if InstallPackage has compression suffix (only .Z and .gz are supported)
                    'If InstallPackage is compressed there will be a period "." appended
                    sStart = InStr(command, InstallPackage & ".")

                    If sStart > 0 then
                      If (Mid(command, sStart+Len(InstallPackage), 2)) = ".Z" then
                        sCompressionSuffix = ".Z"
                        sUnCompressCommand = Mid(command, 1, sStart+Len(InstallPackage)+1)
                        sInstallCommand = Mid(command, sStart+Len(InstallPackage)+3, Len(command))
                      ElseIf (Mid(command, sStart+Len(InstallPackage), 3)) = ".gz" then
                        sCompressionSuffix = ".gz"
                        sUnCompressCommand = Mid(command, 1, sStart+Len(InstallPackage)+2)
                        sInstallCommand = Mid(command, sStart+Len(InstallPackage)+4, Len(command))
                      Else
                        WScript.Echo "Unknown compression suffix, aborting!"
                        WScript.Quit 1
                      End If
                    Else
                      'InstallPackage is not compressed
                      sCompressionSuffix=""
                      sUnCompressCommand=""
                      sInstallCommand=command
                    End If

                     'Check if package with correct permissions exists and echo if found.
                    sCheckFileCommand = "/usr/bin/find /tmp/" & InstallPackage & sCompressionSuffix & " -user $USER -exec echo Found package with correct permissions: {} \;"

                    If sUnCompressCommand <> "" then
                      'Add security checks to UnCompressCommand
                      sUnCompressCommand = "/usr/bin/find /tmp/" & InstallPackage & sCompressionSuffix & " -user $USER -exec " & SUCommand & " '" & sUnCompressCommand & "' \;"
                    End If

                    'Add security checks to InstallCommand
                    sInstallCommand = "/usr/bin/find /tmp/" & InstallPackage & " -user $USER -exec " & SUCommand & " '" & sInstallCommand & "' \;"

                    'Debug printouts
                    'WScript.Echo "Original Command: " & command & " Len: " & Len(command)
                    'WScript.Echo "CompressionSuffix: " & sCompressionSuffix & " Len: " & Len(sCompressionSuffix)
                    'WScript.Echo "UnCompressCommand: " & sUnCompressCommand & " Len: " & Len(sUnCompressCommand)
                    'WScript.Echo "InstallCommand: " & sInstallCommand & " Len: " & Len(sInstallCommand)

                    'Check if InstallPackage with correct permission exists and echo if found
		    Status = 1
                    Status = scxssh.ShellCommand3(sCheckFileCommand)
                    Result = scxssh.GetResponse()
                    WScript.Echo FixCR(Result)

		    If CInt(Status) <> 0 then WScript.Quit CInt(Status)
                    If InStr(Result, "Found package with correct permissions: /tmp/" & InstallPackage & sCompressionSuffix) = 0 Then
                      WScript.Echo "Permission mismatch or InstallPackage not found, aborting!"
                      WScript.Quit 1
                    End If

                    'If file is compressed, decompress it
                    If sUnCompressCommand <> "" then
		      Status = 1
                      Status = scxssh.ShellCommand3(sUnCompressCommand)
		      Result = scxssh.GetResponse()
		      WScript.Echo FixCR(Result)

                      ' Handle Failed Command Execution
                      If CInt(Status) <> 0 then
                        WScript.Echo "Uncompresscommand Failed"
                        WScript.Quit 1
                      End If
                    End If

		    Status = 1
                    Status = scxssh.ShellCommand3(sInstallCommand)
		    Result = scxssh.GetResponse()
		    WScript.Echo FixCR(Result)

                    ' Handle Failed Command Execution
                    If CInt(Status) <> 0 then
                      WScript.Echo "InstallCommand Failed"
                      WScript.Quit 1
                    End If

                    'Send root password
		    Status = 1
                    Status = scxssh.ShellCommand3(SUPassword)
		    'Note that this may contain the password (if elevation is done from root), but we don't want to miss other relevant output
		    Result = scxssh.GetResponse()
		    WScript.Echo FixCR(Replace(Result,SUPassword,"<Password>"))

                    ' Handle Failed Command Execution
                    If CInt(Status) <> 0 then
                      WScript.Echo "SUPWCommand Failed"
                      WScript.Quit 1
                    End If

                    'Remove the package, otherwise next installation may fail if another user is used.
                    'No need to add CompressionSuffix as the file has been uncompressed if it was compressed.
                    'Scxadmin is run to check if Agent is running, returncode from scxadmin is returned from script.
                    sRemoveCommand = "rm /tmp/" & InstallPackage & "; /usr/sbin/scxadmin -status"
		    Status = 1
                    Status = scxssh.ShellCommand3(sRemoveCommand)
                    Result = scxssh.GetResponse()
                    WScript.Echo FixCR(Result)

                    'Close the Shell and get returncode from last command (scxadmin -status)
		    Status = 1
                    Status = scxssh.CloseShell()
                    Result = scxssh.GetResponse()
                    WScript.Echo FixCR(Result)

                    'Close the SSH session
                    scxssh.Shutdown()

                    'Return status, convert from HResult
                    WScript.Quit CInt(Status)
