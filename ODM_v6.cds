<distribution version="17.0.0" name="ODM_v6" type="MSI">
	<prebuild>
		<workingdir>workspacedir</workingdir>
		<actions></actions></prebuild>
	<postbuild>
		<workingdir>workspacedir</workingdir>
		<actions></actions></postbuild>
	<msi GUID="{B9C6CC51-3CDD-4ADB-9F9C-95401EEA152C}">
		<general appName="ODM_v6" outputLocation="d:\1-Newkey_Photonics\2-我参与的项目\1-ODM\11-软件\OFDA_V6\OFDA_V6_2 - 无限制\cvidistkit.ODM_v6" relOutputLocation="cvidistkit.ODM_v6" outputLocationWithVars="d:\1-Newkey_Photonics\2-我参与的项目\1-ODM\11-软件\OFDA_V6\OFDA_V6_2 - 无限制\cvidistkit.%name" relOutputLocationWithVars="cvidistkit.%name" upgradeBehavior="1" autoIncrement="true" version="6.2.7">
			<arp company="Newkey Photonics" companyURL="" supportURL="" contact="" phone="" comments=""/>
			<summary title="" subject="" keyWords="" comments="" author=""/></general>
		<userinterface language="Chinese (Simplified)" showPaths="true" showRuntimeOnly="true" readMe="" license="">
			<dlgstrings welcomeTitle="ODM_v6" welcomeText=""/></userinterface>
		<dirs appDirID="100">
			<installDir name="[Program Files]" dirID="2" parentID="-1" isMSIDir="true" visible="true" unlock="false"/>
			<installDir name="[Start&gt;&gt;Programs]" dirID="7" parentID="-1" isMSIDir="true" visible="true" unlock="false"/>
			<installDir name="[Program Data]" dirID="27" parentID="-1" isMSIDir="true" visible="true" unlock="false"/>
			<installDir name="ODM_v6" dirID="100" parentID="2" isMSIDir="false" visible="true" unlock="false"/>
			<installDir name="ODM" dirID="101" parentID="27" isMSIDir="false" visible="true" unlock="false"/>
			<installDir name="ODM_v6" dirID="102" parentID="7" isMSIDir="false" visible="true" unlock="false"/>
			<installDir name="APD" dirID="103" parentID="101" isMSIDir="false" visible="true" unlock="false"/></dirs>
		<files>
			<simpleFile fileID="0" sourcePath="d:\1-Newkey_Photonics\2-我参与的项目\1-ODM\11-软件\OFDA_V6\OFDA_V6_2 - 无限制\cvibuild.OFDA_V6\Release\OFDA_V6.exe" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="1" sourcePath="D:\1-Newkey_Photonics\2-我参与的项目\1-ODM\11-软件\OFDA_V6\OFDA_V6_2\OFDA_V1.2\OFDA.exe" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="2" sourcePath="d:\1-Newkey_Photonics\2-我参与的项目\1-ODM\11-软件\OFDA_V6\OFDA_V6_2 - 无限制\OFDA_V6.uir" relSourcePath="OFDA_V6.uir" relSourceBase="0" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="3" sourcePath="D:\1-Newkey_Photonics\2-我参与的项目\1-ODM\11-软件\OFDA_V6\OFDA_V6_2\ODM型高精度光延时测量仪用户操作手册.pdf" targetDir="103" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="4" sourcePath="D:\1-Newkey_Photonics\2-我参与的项目\1-ODM\11-软件\OFDA_V6\OFDA_V6_2\OFDA_V1.2\OFDA.uir" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/></files>
		<fileGroups>
			<projectOutput targetType="0" dirID="100" projectID="0">
				<fileID>0</fileID></projectOutput>
			<projectDependencies dirID="100" projectID="0"/></fileGroups>
		<shortcuts>
			<shortcut name="ODM_v6" targetFileID="0" destDirID="102" cmdLineArgs="" description="" runStyle="NORMAL"/></shortcuts>
		<mergemodules/>
		<products/>
		<nonAutoSelectProducts>
			<productID>{272BF3A9-7DBB-47EE-B2B8-CB893AA8166B}</productID>
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
			<Project000 ProjectID="0" ProjectAbsolutePath="d:\1-Newkey_Photonics\2-我参与的项目\1-ODM\11-软件\OFDA_V6\OFDA_V6_2 - 无限制\OFDA_V6.prj" ProjectRelativePath="OFDA_V6.prj"/></Projects>
		<buildData progressBarRate="16.915082375153009">
			<progressTimes>
				<Begin>0.000000000000000</Begin>
				<ProductsAdded>0.471755708496093</ProductsAdded>
				<DPConfigured>1.160803161743164</DPConfigured>
				<DPMergeModulesAdded>1.762228287475585</DPMergeModulesAdded>
				<DPClosed>3.194670769165039</DPClosed>
				<DistributionsCopied>3.344382568603515</DistributionsCopied>
				<End>5.911883713134766</End></progressTimes></buildData>
	</msi>
</distribution>
