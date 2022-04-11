## context menu extension for handling code files license  
  
requires 3rd party tool to check and repair the files.  
install the COM extension using this command:  
```
regsvr32 CodeLicenseHelper.dll
```
to uninstall:
```
regsvr32 /u CodeLicenseHelper.dll
```

then create a file in the user profile directory named `code-license.config.xml`  
the content of the file should be as follow:  
```
<?xml version="1.0" encoding="utf-8"?>
<Settings>
	<!-- required. types of licenses the files have 
		 if not selectable set Selectable directive to false, is true by default
		 example: 
		 <Licenses>
		 	<Type> license A </Type>
			<Type> license B </Type>
		 </Licenses> -->
	<Licenses>
		<Type>license 1</Type>
		<Type>license 2</Type>
		<Type>license 3</Type>
		<Type>ignore</Type>
		<Type Selectable="false">unknown</Type>
	</Licenses>
	<!-- required. this command will be executed when a file is inspected
		 if it returns "none" the file should not show license menu
		 otherwise return value should be one of the Types specified above
		 Timeout attribute if specified will terminate the command process 
		 after specified time in milliseconds.
		 put $1 in the command and it will be replaced with the file path
		 example: 
		 <Get-Command Timeout="5000"> get_license.exe $1 </Get-Command> -->
	<Get-Command Timeout="2000"> command </Get-Command>
	<!-- this command will be executed when a license from the menu is selected
		 put $1 in the command and it will be replaced with the file path
		 put $2 in the command and it will be replaced with the selected license
		 example: 
		 <Set-Command> set_license.exe $2 $1 </Set-Command> -->
	<Set-Command> command </Set-Command>
	<!-- this command will be executed when a the check license option is selected
		 put $1 in the command and it will be replaced with the file path
		 example: 
		 <Check-Command> check_license.exe $1 </Check-Command> -->
	<Check-Command> command </Check-Command>
	<!-- this command will be executed when a the repair license option is selected
		 put $1 in the command and it will be replaced with the file path
		 example: 
		 <Repair-Command> repair_license.exe $1 </Repair-Command> -->
	<Repair-Command> command </Repair-Command>
	<!-- types of files extension that have license
		 will check only those files, unless this list is empty
		 otherwise all files will be checked.
		 example:
		 <Files>
			<Extension> c </Extension>
			<Extension> h </Extension>
			<Extension> cpp </Extension>
			<Name> makefile </Name>
		 </Files> -->
	<Files>
		<Extension> c </Extension>
		<Extension> h </Extension>
		<Extension> cpp </Extension>
		<Name> makefile </Name>
	</Files>
</Settings>
```

#### screenshot

![code license context menu](https://raw.githubusercontent.com/pcx229/CodeLicenseHelper/master/screenshot.png)