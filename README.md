# Windows Service

+ Explore Windows services
+ Запуск реверс shell'a через Windows services

## How to

+ запустить `service.exe` c параметрами

```PYTHON
Allowed options:
  -h [ --help ]                         give this help list
  --type type                           command type, 0 - create service, 1 -
                                        run service
  --group-name group-name               SvcHost service group name
  --service-name service-name           Service name
  --description description (=ServiceDescription)
                                        Service description
  --display-name display-name (=DisplayName)
                                        Service display name
  --dll-path dll-path                   Service DLL full path
  --verbose verbose (=1)                verbose messages, 0 - without, 1 - with
```

+ собрать и поместить `service_dll.dll` например в `C:\test`
+ запустить netcat, поймать reverse shell

## Запись результата

+ после перезагрузки, нажмите `ESC`, далее `C:\mbr.bin` будет содержать dump MBR

## Пример работы

![alt text](/img/service.gif)
