# Injector
C++ Injector which is used by the Rewind-Launcher to inject dlls into a specific process.

# Usage

Make sure you have https://aka.ms/vs/17/release/vc_redist.x64.exe installed and are using a Visual Studio higher the VS2015.
Compile the project in "Release", to use it; spawn the process aka: "Injector.exe <<ProcessId>> <<DllPath>>",
DllPath being the **FULL PATH WITH EXTENSION** of the dll, the processid being the PID of the target process.

# Credits

When using this please credit this repository OR https://github.com/PrivateUserSource

