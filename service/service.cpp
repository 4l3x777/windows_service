#include <iostream>
#include <service_manager.h>
#include <boost/program_options.hpp>

// service options struct
typedef struct 
{
    uint8_t command_type;
    bool verbose;
    std::string groupName;
    std::string serviceName;
    std::string serviceDescription;
    std::string serviceDisplayName;
    std::string serviceDllFullPath;
} ServiceSettings, *PServiceSettings;

class ServiceClass
{
    ServiceSettings options;
    ServiceManager service_manager; 

    bool ParsingCLArgs(int argc, char* argv[]) 
    {
        try {
            boost::program_options::options_description named_description {
                "Allowed options"
            };

            named_description.add_options()
            ("help,h", "give this help list")
            ("type",
                boost::program_options::value<int>()->value_name("type"),
                "command type, 0 - create service, 1 - run service")
            ("group-name",
                boost::program_options::value<std::string>()->value_name("group-name"),
                "SvcHost service group name")
            ("service-name",
                boost::program_options::value<std::string>()->value_name("service-name"),
                "Service name")
            ("description",
                boost::program_options::value<std::string>()->value_name("description")->default_value(std::string("ServiceDescription")),
                "Service description")
            ("display-name",
                boost::program_options::value<std::string>()->value_name("display-name")->default_value(std::string("DisplayName")),
                "Service display name")
            ("dll-path",
                boost::program_options::value<std::string>()->value_name("dll-path"),
                "Service DLL full path")
            ("verbose",
                boost::program_options::value<bool>()->value_name("verbose")->default_value(true),
                "verbose messages, 0 - without, 1 - with");

            boost::program_options::options_description cmdline_description { };
            cmdline_description.add(named_description);

            boost::program_options::variables_map variables;
            boost::program_options::store(
            boost::program_options::command_line_parser(argc, argv)
                .options(cmdline_description)
                .run(),
            variables);
            boost::program_options::notify(variables);

            if (variables.find("type") == variables.end()) 
            {
                std::cout << "Options error \"type\": command type, 0 - create service, 1 - run service" << std::endl;
                named_description.print(std::cout);
                return false;
            } 
            else options.command_type = variables["type"].as<int>();

            if (variables.find("group-name") == variables.end()) 
            {
                std::cout << "Options error \"group-name\": svchost service group name" << std::endl;
                named_description.print(std::cout);
                return false;
            } 
            else options.groupName = variables["group-name"].as<std::string>();
            
            if (variables.find("service-name") == variables.end()) 
            {
                std::cout << "Options error \"service-name\": svchost service name" << std::endl;
                named_description.print(std::cout);
                return false;
            } 
            else options.serviceName = variables["service-name"].as<std::string>();

            options.serviceDisplayName = variables["display-name"].as<std::string>();

            if (options.command_type == 0)
            {
                if (variables.find("dll-path") == variables.end()) 
                {
                    std::cout << "Options error \"dll-path\": svchost service dll full path" << std::endl;
                    named_description.print(std::cout);
                    return false;
                } 
                else options.serviceDllFullPath = variables["dll-path"].as<std::string>();
            }

            options.serviceDescription = variables["description"].as<std::string>();

            options.verbose = variables["verbose"].as<bool>();

            variables.clear();
            return true;
        }
        catch (boost::program_options::error const &e) 
        {
            std::cerr << "Options error: " << e.what() << std::endl;
            return false;
        }
    };

public:
    ServiceClass(int argc, char* argv[]) 
    {
        if (ParsingCLArgs(argc, argv))
        {
            switch(options.command_type)
            {
            case 0:
            {
                service_manager.create_service(
                    options.groupName,
                    options.serviceName,
                    options.serviceDescription,
                    options.serviceDisplayName,
                    options.serviceDllFullPath,
                    options.verbose);
                break;
            }
            case 1:
            {
                service_manager.start_service(
                    options.groupName,
                    options.serviceName,
                    options.verbose);
                break;
            }
            default:
            {
                std::cout << "Type " << std::to_string(options.command_type) << " is not allowed!" << std::endl;
                break;
            }
            }
        }
    }
};


int main(int argc, char* argv[])
{
    auto service = ServiceClass(argc, argv);
    return 0;
}