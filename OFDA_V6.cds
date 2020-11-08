<distribution version="17.0.0" name="ODM_v6" type="MSI">
	<prebuild>
		<workingdir>workspacedir</workingdir>
		<actions></actions></prebuild>
	<postbuild>
		<workingdir>workspacedir</workingdir>
		<actions></actions></postbuild>
	<msi GUID="{B1CDF8FA-1BE1-46D9-8ABC-086217DE7BA7}">
		<general appName="ODM_v6" outputLocation="d:\WorkProject\614Project\ODM\OFDA_V6\distribution" relOutputLocation="distribution" outputLocationWithVars="d:\WorkProject\614Project\ODM\OFDA_V6\distribution" relOutputLocationWithVars="distribution" upgradeBehavior="1" autoIncrement="true" version="6.3.4">
			<arp company="Newkey Photonics" companyURL="" supportURL="" contact="" phone="" comments=""/>
			<summary title="" subject="" keyWords="" comments="" author=""/></general>
		<userinterface language="Chinese (Simplified)" showPaths="true" showRuntimeOnly="true" readMe="" license="">
			<dlgstrings welcomeTitle="ODM_v6" welcomeText=""/></userinterface>
		<dirs appDirID="101">
			<installDir name="ODM_v6" dirID="100" parentID="7" isMSIDir="false" visible="true" unlock="false"/>
			<installDir name="ODM_v6" dirID="101" parentID="2" isMSIDir="false" visible="true" unlock="false"/>
			<installDir name="ODM" dirID="102" parentID="27" isMSIDir="false" visible="true" unlock="false"/>
			<installDir name="[Start&gt;&gt;Programs]" dirID="7" parentID="-1" isMSIDir="true" visible="true" unlock="false"/>
			<installDir name="[Program Data]" dirID="27" parentID="-1" isMSIDir="true" visible="true" unlock="false"/>
			<installDir name="[Program Files]" dirID="2" parentID="-1" isMSIDir="true" visible="true" unlock="false"/></dirs>
		<files>
			<simpleFile fileID="0" sourcePath="D:\WorkProject\614Project\ODM\OFDA_V6\src\conf\conf.dll" targetDir="102" readonly="true" hidden="true" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="1" sourcePath="D:\WorkProject\614Project\ODM\OFDA_V6\document\ODM型高精度光延时测量仪用户操作手册.pdf" targetDir="102" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="2" sourcePath="D:\WorkProject\614Project\ODM\OFDA_V6\OFDA_V1.2\OFDA.uir" targetDir="101" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="3" sourcePath="D:\WorkProject\614Project\ODM\OFDA_V6\OFDA_V1.2\OFDA.exe" targetDir="101" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="4" sourcePath="d:\WorkProject\614Project\ODM\OFDA_V6\OFDA_V6.uir" relSourcePath="OFDA_V6.uir" relSourceBase="0" targetDir="101" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="5" sourcePath="d:\WorkProject\614Project\ODM\OFDA_V6\cvibuild.OFDA_V6\Release\OFDA_V6.exe" targetDir="101" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/></files>
		<fileGroups>
			<projectOutput targetType="0" dirID="101" projectID="0">
				<fileID>5</fileID></projectOutput>
			<projectDependencies dirID="101" projectID="0"/></fileGroups>
		<shortcuts>
			<shortcut name="OFDA_v6" targetFileID="5" destDirID="100" cmdLineArgs="" description="" runStyle="NORMAL"/></shortcuts>
		<mergemodules/>
		<products/>
		<nonAutoSelectProducts>
			<productID>{28BFBCB6-5EEE-47F1-A894-FCD877057827}</productID>
			<productID>{F9B5B433-547E-4A74-AFE6-91C16787824E}</productID></nonAutoSelectProducts>
		<runtimeEngine installToAppDir="false" activeXsup="false" analysis="false" cvirte="false" dotnetsup="false" instrsup="false" lowlevelsup="false" lvrt="false" netvarsup="false" rtutilsup="false">
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
			<Project000 ProjectID="0" ProjectAbsolutePath="d:\WorkProject\614Project\ODM\OFDA_V6\OFDA_V6.prj" ProjectRelativePath="OFDA_V6.prj"/></Projects>
		<buildData progressBarRate="38.164675284186302">
			<progressTimes>
				<Begin>0.000000000000000</Begin>
				<ProductsAdded>0.460785582519529</ProductsAdded>
				<DPConfigured>0.743249812072724</DPConfigured>
				<DPMergeModulesAdded>0.993094621154750</DPMergeModulesAdded>
				<DPClosed>1.491828894226054</DPClosed>
				<DistributionsCopied>1.592380666076654</DistributionsCopied>
				<End>2.620224048950193</End></progressTimes></buildData>
	</msi>
</distribution>
