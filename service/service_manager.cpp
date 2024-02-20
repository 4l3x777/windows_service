#include <service_manager.h>
#include <Windows.h>

bool ServiceManager::registry_register_service()
{
    LSTATUS status = FALSE;
    HKEY hKey, hParKey;
    auto regValue = std::string(MAX_PATH, 0x0);
    auto regPath = "SYSTEM\\CurrentControlSet\\Services\\" + serviceName;
    status = RegCreateKeyA(HKEY_LOCAL_MACHINE, regPath.c_str(), &hKey);
    if (status == ERROR_SUCCESS)
    {		
        RegSetValueExA(hKey, "Description", 0, REG_SZ, (BYTE*)serviceDescription.c_str(), serviceDescription.size());
            
        RegSetValueExA(hKey, "DisplayName", 0, REG_SZ, (BYTE*)serviceDisplayName.c_str(), serviceDisplayName.size()); 
            
        DWORD value = 0x1;
        RegSetValueExA(hKey, "ErrorControl", 0, REG_DWORD, (BYTE*)&value, sizeof(DWORD)); 
            
        auto imagePath = "\%SystemRoot\%\\system32\\svchost.exe -k " + groupName;
        RegSetValueExA(hKey, "ImagePath", 0, REG_EXPAND_SZ, (BYTE*)imagePath.c_str(), imagePath.size()); 
            
        auto objectName = std::string("LocalSystem");
        RegSetValueExA(hKey, "ObjectName", 0, REG_SZ, (BYTE*)objectName.c_str(), objectName.size()); 

        value = 0x2;
        RegSetValueExA(hKey, "Start", 0, REG_DWORD, (BYTE*)&value, sizeof(DWORD)); 
           
        value = 0x20;
        RegSetValueExA(hKey, "Type", 0, REG_DWORD, (BYTE*)&value, sizeof(DWORD)); 
        if (RegCreateKeyA(hKey, "Parameters", &hParKey) == ERROR_SUCCESS) 
        {
            status = RegSetValueExA(hParKey, "ServiceDll", 0, REG_EXPAND_SZ, (BYTE*)serviceDllFullPath.c_str(), serviceDllFullPath.size());
            RegCloseKey(hParKey);
        }
        RegCloseKey(hKey);
    }
    else 
    {
        if (verbose) std::cout << __FUNCTION__ << " [-] RegCreateKeyA code: " << std::to_string(GetLastError()) << std::endl; 
    }
    return status == ERROR_SUCCESS;
}

bool ServiceManager::add_to_svchost_group()
{
    LSTATUS status = ERROR_SUCCESS;
    HKEY hKey;
    if (RegOpenKeyA(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows NT\\CurrentVersion\\SvcHost", &hKey) == ERROR_SUCCESS)
    {
        DWORD size, type;

        status = RegQueryValueExA(hKey, groupName.c_str(), NULL, &type, NULL, &size);

        if (type == REG_MULTI_SZ && status == ERROR_SUCCESS)
        {
            auto groupBuff = std::string(size - 1, 0x0);
            status = RegQueryValueExA(hKey, groupName.c_str(), NULL, NULL, (LPBYTE)groupBuff.data(), &size);
            if (groupBuff.find(serviceName) == std::string::npos)
            {
                groupBuff += serviceName + '\00' + '\00';
                status = RegSetValueExA(hKey, groupName.c_str(), 0, REG_MULTI_SZ, (LPBYTE)groupBuff.c_str(), groupBuff.size());
            }
        }

    }
    RegCloseKey(hKey);
    return (status == ERROR_SUCCESS);
}

bool ServiceManager::is_mandatory_high_process()
{
    DWORD dwLengthNeeded = 0;
    bool result = false;
    GetTokenInformation(GetCurrentProcessToken(), TokenIntegrityLevel, NULL, 0, &dwLengthNeeded);
    PTOKEN_MANDATORY_LABEL pTIL = (PTOKEN_MANDATORY_LABEL)LocalAlloc(LPTR, dwLengthNeeded);
    if (GetTokenInformation(GetCurrentProcessToken(), TokenIntegrityLevel, pTIL, dwLengthNeeded, &dwLengthNeeded))
    {
        auto dwIntegrityLevel = *GetSidSubAuthority(pTIL->Label.Sid, (DWORD)(UCHAR)(*GetSidSubAuthorityCount(pTIL->Label.Sid)-1));
        if (dwIntegrityLevel >= SECURITY_MANDATORY_HIGH_RID)
        {
            result = true;
        }
    }
    LocalFree(pTIL);
    return result;
}

bool ServiceManager::stop_service(const std::string& _groupName, const std::string& _serviceName, bool _verbose)
{
    //TODO: make implementation
    return true;
}

bool ServiceManager::start_service(const std::string& _groupName, const std::string& _serviceName, bool _verbose)
{
    groupName = _groupName;
    serviceName = _serviceName;
    verbose = _verbose;
    SC_HANDLE hService, hSCM;
	hSCM = OpenSCManagerA(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if (hSCM)
	{
		if (verbose) std::cout << " [+] OpenSCManager!" << std::endl;
		hService = CreateServiceA(
			hSCM, 
			serviceName.c_str(),
			serviceName.c_str(),
			SERVICE_ALL_ACCESS, SERVICE_WIN32_SHARE_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_IGNORE,
			("\%SystemRoot\%\\system32\\svchost.exe -k " + groupName).c_str(), 0, 0, 0, 0, 0);
        if (!hService) 
        {
            hService = OpenServiceA(
                		hSCM, 
				        serviceName.c_str(),
                        SERVICE_ALL_ACCESS);
        }
		if (hService)
		{
			if (verbose) std::cout << " [+] Create/Open service!" << std::endl;
			StartServiceA(hService, 0, 0);
			CloseServiceHandle(hService);
			CloseServiceHandle(hSCM);
            return true;
		}
		else if (verbose) std::cout << " [-] Create/Open service!" << std::endl;
	}
	else if (verbose) std::cout << " [-] OpenSCManager!" << std::endl;  
    return false;
}

bool ServiceManager::create_service(
        const std::string& _groupName, 
        const std::string& _serviceName, 
        const std::string& _serviceDescription,
        const std::string& _serviceDisplayName,
        const std::string& _serviceDllFullPath,
        bool _verbose)
{
    groupName = _groupName;
    serviceName = _serviceName;
    serviceDescription = _serviceDescription;
    serviceDisplayName = _serviceDisplayName;
    serviceDllFullPath = _serviceDllFullPath;
    verbose = _verbose;
    if (is_mandatory_high_process())
    {
        if (verbose) std::cout << " [+] Process is elevated!" << std::endl; 
        if (registry_register_service())
        {
            if (verbose) std::cout << " [+] Service has registered in registry!" << std::endl; 
            if(add_to_svchost_group())
            {
                if (verbose) std::cout << " [+] Service added to svchost group!" << std::endl;
                return true;
            }
            else if (verbose) std::cout << " [-] Service hasn't added to svchost group!" << std::endl;
        }
        else if (verbose) std::cout << " [-] Service hasn't registered in registry!" << std::endl; 
    }
    else if (verbose) std::cout << " [-] Process is not elevated!" << std::endl; 
    return false;
}

