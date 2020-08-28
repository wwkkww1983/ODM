<distribution version="17.0.0" name="OFDA" type="MSI">
	<prebuild>
		<workingdir>workspacedir</workingdir>
		<actions></actions></prebuild>
	<postbuild>
		<workingdir>workspacedir</workingdir>
		<actions></actions></postbuild>
	<msi GUID="{9F5AE6F8-9433-4915-94C1-786874B19097}">
		<general appName="OFDA" outputLocation="e:\CVI\OFDA\OFDA_V1.1\cvidistkit.OFDA" relOutputLocation="cvidistkit.OFDA" outputLocationWithVars="e:\CVI\OFDA\OFDA_V1.1\cvidistkit.%name" relOutputLocationWithVars="cvidistkit.%name" upgradeBehavior="1" autoIncrement="true" version="1.0.12">
			<arp company="NEWKEY" companyURL="" supportURL="" contact="" phone="" comments=""/>
			<summary title="" subject="" keyWords="" comments="" author=""/></general>
		<userinterface language="English" showPaths="true" showRuntimeOnly="true" readMe="" license="">
			<dlgstrings welcomeTitle="NEWKEY" welcomeText=""/></userinterface>
		<dirs appDirID="101">
			<installDir name="OFDA" dirID="100" parentID="7" isMSIDir="false" visible="true" unlock="false"/>
			<installDir name="OFDA" dirID="101" parentID="2" isMSIDir="false" visible="true" unlock="false"/>
			<installDir name="[Program Files]" dirID="2" parentID="-1" isMSIDir="true" visible="true" unlock="false"/>
			<installDir name="[Start&gt;&gt;Programs]" dirID="7" parentID="-1" isMSIDir="true" visible="true" unlock="false"/></dirs>
		<files>
			<simpleFile fileID="0" sourcePath="e:\CVI\OFDA\OFDA_V1.1\cvibuild.OFDA\Release\OFDA.exe" targetDir="101" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="1" sourcePath="e:\CVI\OFDA\OFDA_V1.1\OFDA.uir" relSourcePath="OFDA.uir" relSourceBase="0" targetDir="101" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/></files>
		<fileGroups>
			<projectOutput targetType="0" dirID="101" projectID="0">
				<fileID>0</fileID></projectOutput>
			<projectDependencies dirID="101" projectID="0"/></fileGroups>
		<shortcuts>
			<shortcut name="OFDA" targetFileID="0" destDirID="100" cmdLineArgs="" description="" runStyle="NORMAL"/></shortcuts>
		<mergemodules/>
		<products/>
		<nonAutoSelectProducts>
			<productID>{24D0BA6A-3C44-41E7-A357-FD1CAFB59F58}</productID>
			<productID>{28BFBCB6-5EEE-47F1-A894-FCD877057827}</productID></nonAutoSelectProducts>
		<runtimeEngine installToAppDir="false" activeXsup="false" analysis="true" cvirte="true" dotnetsup="true" instrsup="false" lowlevelsup="true" lvrt="false" netvarsup="true" rtutilsup="false">
			<hasSoftDeps/>
			<doNotAutoSelect>
			<component>activeXsup</component>
			<component>instrsup</component>
			<component>lvrt</component>
			<component>rtutilsup</component>
			</doNotAutoSelect></runtimeEngine><sxsRuntimeEngine>
			<selected>false</selected>
			<doNotAutoSelect>false</doNotAutoSelect></sxsRuntimeEngine>
		<advanced mediaSize="650">
			<launchConditions>
				<condition>MINOS_WIN7_SP1</condition>
			</launchConditions>
			<includeConfigProducts>true</includeConfigProducts>
			<maxImportVisible>silent</maxImportVisible>
			<maxImportMode>merge</maxImportMode>
			<custMsgFlag>false</custMsgFlag>
			<custMsgPath>msgrte.txt</custMsgPath>
			<signExe>false</signExe>
			<certificate></certificate>
			<signTimeURL></signTimeURL>
			<signDescURL></signDescURL></advanced>
		<baselineProducts/>
		<Projects NumProjects="1">
			<Project000 ProjectID="0" ProjectAbsolutePath="e:\CVI\OFDA\OFDA_V1.1\OFDA.prj" ProjectRelativePath="OFDA.prj"/></Projects>
		<buildData progressBarRate="12.811488710107611">
			<progressTimes>
				<Begin>0.000000000000000</Begin>
				<ProductsAdded>0.155427550537109</ProductsAdded>
				<DPConfigured>0.942523328125000</DPConfigured>
				<DPMergeModulesAdded>1.359848497314453</DPMergeModulesAdded>
				<DPClosed>2.708309778076172</DPClosed>
				<DistributionsCopied>2.826496083984375</DistributionsCopied>
				<End>7.805494135986328</End></progressTimes></buildData>
	</msi>
</distribution>
