//The exports

//DllCanUnloadNow
//DllGetClassObject
//DllRegisterServer
//DllUnregisterServer

#pragma comment(linker, "/export:DllCanUnloadNow=C:/Windows/System32/Dism/DismCore.DllCanUnloadNow")
#pragma comment(linker, "/export:DllGetClassObject=C:/Windows/System32/Dism/DismCore.DllGetClassObject")
#pragma comment(linker, "/export:DllRegisterServer=C:/Windows/System32/Dism/DismCore.DllRegisterServer")
#pragma comment(linker, "/export:DllUnregisterServer=C:/Windows/System32/Dism/DismCore.DllUnregisterServer")