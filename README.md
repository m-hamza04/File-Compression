# File Compression System (C++ / OpenCV)

A C++ based **File Compression System** project developed for academic purposes. This repository contains **source code only**. External libraries like OpenCV are required to be installed separately on your system.

# Features

* File compression and decompression logic
* Efficient handling of files using C++
* Modular and easy-to-understand code structure

# Requirements

* **C++ Compiler (MSVC / MinGW / GCC)
* **OpenCV 4.x
* Windows OS (tested on Windows 10/11)

# Installing OpenCV (Windows)

## Download OpenCV

Download OpenCV from the official website:

* [https://opencv.org/releases/](https://opencv.org/releases/)

Choose the Windows version and extract it, for example:

```
C:\opencv
```

# Configure OpenCV in Your Project

## Include Directories

Add the following Include Directory to your compiler/project settings:

```
C:\opencv\build\include
```

## Library Directories

Add the following Library Directory:

```
C:\opencv\build\x64\vc16\lib
```

---

## Link OpenCV Libraries

Link the required OpenCV libraries (example):

```
opencv_world4120d.lib   (Debug)
opencv_world4120.lib    (Release)
```

### Version number may change depending on your OpenCV version.


# DLL Setup 

## Add OpenCV DLL Path to Environment Variables

You must add this path to your system PATH:

```
C:\opencv\build\x64\vc16\bin
```

### Steps:

1. Open **Environment Variables**
2. Edit **Path** under System Variables
3. Click **New**
4. Paste:

   ```
   C:\opencv\build\x64\vc16\bin
   ```
5. Click OK and restart your PC


## How to Run the Project

1. Open the project in your IDE (Qt Creator)
2. Ensure OpenCV is correctly configured
3. Build the project
4. Run the executable

If OpenCV is set correctly, the project will run without DLL errors.

## Important Notes

* OpenCV **DLL, LIB, and PDB files are NOT included** in this repository
* These files are large and must be installed locally
* This follows **industry-standard GitHub practices**

## .gitignore Policy

This project ignores:

* OpenCV folders
* DLL / EXE / PDB files
* Build directories

This keeps the repository clean and lightweight.


## License

This project is for **educational purposes only**.

