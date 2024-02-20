#include <iostream>
#include <string>

class ServiceManager
{
    std::string groupName;
    std::string serviceName;
    std::string serviceDescription;
    std::string serviceDisplayName;
    std::string serviceDllFullPath;
    bool verbose;

    bool is_mandatory_high_process();
    bool add_to_svchost_group(); 
    bool registry_register_service();

public:
    bool start_service(const std::string& _groupName = "", const std::string& _serviceName = "", bool _verbose = false);
    bool stop_service(const std::string& _groupName = "", const std::string& _serviceName = "", bool _verbose = false);
    bool create_service(
        const std::string& _groupName = "", 
        const std::string& _serviceName = "", 
        const std::string& _serviceDescription = "",
        const std::string& _serviceDisplayName = "",
        const std::string& _serviceDllFullPath = "",
        bool _verbose = false);
};