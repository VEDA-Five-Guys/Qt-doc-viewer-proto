# Qt-doc-viewer-proto

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

Before execute the file, you need to install Boost C++ Libraries with the following steps.
- Download the 1_86_0 Boost library from the Boost official website.
- Run the terminal from the location where bootstrap.bat is located.
- Execute the command below.  
  <complie_version> : Select the same compiler version as used by Qt.  
  <install_dir>             : Specifies the directory where Boost will be installed.  
  ```
  b2 toolset=<compile_version> address-model=64 architecture=x86 --build-type=complete --prefix=<install_dir> install -j 4
  ```
- In the System Environment variable, set the system variable BOST_ROOT = C:/boost_1_86_0_msvc and add a path to the path.  
  The current path is C:/boost_1_86_0_msvc, but it can be set freely depending on the folder name and installation path.

Once the build is complete, a directory named 'PdfViewer_with_Qt.app' will be generated.
You can run the program by navigating to 'PdfViewer_with_Qt.app/Contents/{your_OS}' and executing the following command.

```bash
./Qt-doc-viewer-proto
```
![project_img](https://github.com/user-attachments/assets/b54d0842-1350-423d-bb9f-ea0eed076788)
