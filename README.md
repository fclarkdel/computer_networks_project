# cpp_project_template

## Overview
The goal of this project is to provide a template for future C++ projects to start from.

It aims to provide reasonable defaults, configured format linters and sanitizers, continuous integration using GitHub, file structure, and CMake boilerplate.

## Files
***.gitignore***

***README.md***

***LICENSE.md***

## File Structure
The file structure is heavily inspired by the ___The Pitchfork Layout___ as specified at https://github.com/vector-of-bool/pitchfork.

> ***data***
> 
> Files needed by the project that should be versioned but are not code. Some examples would be configuration files, graphics or localization related files.

> ***docs***
> 
> Project documentation related files.

> ***external***
> 
> Embedded project dependencies. Each dependency should have its own directory within this directory.

> ***include***
> 
> Public project header files. If this is a library then the library's public header will be here.

> ***source***
>
> Project source code and private headers.
> 
> Try to model project namespaces with the directory structure here, i.e. ```project_name::tools::tool``` corresponds to ```/source/project_name/tools/tool.hpp``` and ```/source/project_name/tools/tool.cpp```.
> 
> Unit tests should also be colocated and suffixed with ```*.test.cpp```. So for ```project_name::tools::tool```, there should be a unit test at ```/source/project_name/tools/tool.test.cpp```

> ***tests***
>
> Non-unit tests source files. 

> ***tools***
>
> Scripts related to developing the project, such as build scripts, linting scripts, and test scripts.

## CMake

## Linters & Sanitizers

## Continuous Integration

## Doxygen