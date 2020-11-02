<distribution version="17.0.0" name="ODM_v6" type="MSI">
	<prebuild>
		<workingdir>workspacedir</workingdir>
		<actions></actions></prebuild>
	<postbuild>
		<workingdir>workspacedir</workingdir>
		<actions></actions></postbuild>
	<msi GUID="{B1CDF8FA-1BE1-46D9-8ABC-086217DE7BA7}">
		<general appName="ODM_v6" outputLocation="d:\WorkProject\614Project\ODM\OFDA_V6_2 - 授权机制改动\distribution" relOutputLocation="distribution" outputLocationWithVars="d:\WorkProject\614Project\ODM\OFDA_V6_2 - 授权机制改动\distribution" relOutputLocationWithVars="distribution" upgradeBehavior="1" autoIncrement="true" version="6.3.1">
			<arp company="Newkey Photonics" companyURL="" supportURL="" contact="" phone="" comments=""/>
			<summary title="" subject="" keyWords="" comments="" author=""/></general>
		<userinterface language="Chinese (Simplified)" showPaths="true" showRuntimeOnly="true" readMe="" license="">
			<dlgstrings welcomeTitle="ODM_v6" welcomeText=""/></userinterface>
		<dirs appDirID="100">
			<installDir name="ODM_v6" dirID="100" parentID="2" isMSIDir="false" visible="true" unlock="false"/>
			<installDir name="ODM" dirID="101" parentID="27" isMSIDir="false" visible="true" unlock="false"/>
			<installDir name="ODM_v6" dirID="102" parentID="7" isMSIDir="false" visible="true" unlock="false"/>
			<installDir name="[Start&gt;&gt;Programs]" dirID="7" parentID="-1" isMSIDir="true" visible="true" unlock="false"/>
			<installDir name="[Program Data]" dirID="27" parentID="-1" isMSIDir="true" visible="true" unlock="false"/>
			<installDir name="[Program Files]" dirID="2" parentID="-1" isMSIDir="true" visible="true" unlock="false"/></dirs>
		<files>
			<simpleFile fileID="0" sourcePath="D:\WorkProject\614Project\ODM\OFDA_V6_2 - 授权机制改动\OFDA_V1.2\OFDA.exe" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="1" sourcePath="d:\WorkProject\614Project\ODM\OFDA_V6_2 - 授权机制改动\OFDA_V6.uir" relSourcePath="OFDA_V6.uir" relSourceBase="0" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="2" sourcePath="D:\WorkProject\614Project\ODM\OFDA_V6_2 - 授权机制改动\document\ODM型高精度光延时测量仪用户操作手册.pdf" targetDir="101" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="3" sourcePath="D:\WorkProject\614Project\ODM\OFDA_V6_2 - 授权机制改动\src\conf\conf.dll" targetDir="101" readonly="true" hidden="true" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="4" sourcePath="d:\WorkProject\614Project\ODM\OFDA_V6_2 - 授权机制改动\cvibuild.OFDA_V6\Release\OFDA_V6.exe" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="5" sourcePath="D:\WorkProject\614Project\ODM\OFDA_V6_2 - 授权机制改动\OFDA_V1.2\OFDA.uir" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/></files>
		<fileGroups>
			<projectOutput targetType="0" dirID="100" projectID="0">
				<fileID>4</fileID></projectOutput>
			<projectDependencies dirID="100" projectID="0"/></fileGroups>
		<shortcuts>
			<shortcut name="OFDA_v6" targetFileID="4" destDirID="102" cmdLineArgs="" description="" runStyle="NORMAL"/></shortcuts>
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
			<Project000 ProjectID="0" ProjectAbsolutePath="d:\WorkProject\614Project\ODM\OFDA_V6_2 - 授权机制改动\OFDA_V6.prj" ProjectRelativePath="OFDA_V6.prj"/></Projects>
		<buildData progressBarRate="40.484250064090013">
			<progressTimes>
				<Begin>0.000000000000000</Begin>
				<ProductsAdded>0.451731660156236</ProductsAdded>
				<DPConfigured>0.711439496581751</DPConfigured>
				<DPMergeModulesAdded>0.960188969237983</DPMergeModulesAdded>
				<DPClosed>1.450071153808396</DPClosed>
				<DistributionsCopied>1.551894328613222</DistributionsCopied>
				<End>2.470096391601462</End></progressTimes></buildData>
	</msi>
</distribution>
