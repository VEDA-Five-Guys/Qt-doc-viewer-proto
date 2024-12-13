# Qt-doc-viewer-proto

## Prequisite
You need to install Boost Library with the following steps.
### Windows
- Download the 1_86_0 Boost library from the Boost official website.
- Run the terminal from the location where bootstrap.bat is located.
- Execute the command below.  
  <complie_version> : Select the same compiler version as used by Qt.  
  <install_dir>             : Specifies the directory where Boost will be installed.  
  ```
  b2 toolset=<compile_version> address-model=64 architecture=x86 --build-type=complete --prefix=<install_dir> install -j 4
  ```
- In the System Environment variable, set the system variable and add a path to the path.  
  The current path is BOOST_ROOT_MSVC = C:/boost_1_86_0_msvc, but it can be set freely depending on the folder name and installation path.
### Mac
Boost can be easily installed using Homebrew, a package manager for macOS. To install Boost, simply run the following command in your terminal:
```bash
brew install boost
```
This command will download and install the latest version of Boost. Once installed, Boost's header files will typically be located in /usr/local/include/boost (or /opt/homebrew/include/boost on Apple Silicon Macs), and the library files in /usr/local/lib (or /opt/homebrew/lib).

## Instruction
Try the following commands in order.
```bash
git clone https://github.com/VEDA-Five-Guys/Qt-doc-viewer-proto.git
cd Qt-doc-viewer-proto
mkdir build
cd build
cmake ..
make
```
Then you can see the executable file named 'Qt-doc-viewer-proto'.  

Once the build is complete, a directory named 'PdfViewer_with_Qt.app' will be generated.
You can run the program by navigating to 'PdfViewer_with_Qt.app/Contents/{your_OS}' and executing the following command.

```bash
./Qt-doc-viewer-proto
```
![project_img](https://github.com/user-attachments/assets/b54d0842-1350-423d-bb9f-ea0eed076788)
