2015/06/10
Zengrui Wang

Protobuf Library building
Version: protobuf-2.5.0 https://github.com/google/protobuf/tree/v2.5.0'
Platform: Windows Visual Studio 2010

Instruction::
1) Modify hash.h line 42: append at the end " && !defined(EMSCRIPTEN)"
2) Open vsprojects/protobuf.sln in Microsoft Visual Studio.

3) For project "libprotobuf" 
	a) Change the extension of all source files from .cc to .cpp 
	b) Replace and add the following files in patch directory to the folder: ..\src\google\protobuf\stubs
	   atomicops.h
	   atomicops_internals_emscripten.h
	   platform_macros.h
	   common.cpp
	c) Also delete the Additional Options in the project properties->Clang C/C++->Command Line
	d) Compile the project, DONE

4) For project "libprotoc"
	a) Change the extension of all source files from .cc to .cpp 
	b) Also in the header folder, there is also a .cc file java_doc_comment.cc change its extension to .cpp too
	c) Also delete the Additional Options in the project properties->Clang C/C++->Command Line
	d) Compile the project, DONE  

5) Modify the extract_includes.bat file: add "copy ..\src\google\protobuf\stubs\atomicops_internals_emscripten.h include\google\protobuf\stubs\atomicops_internals_emscripten.h"

Note: we haven't its functionality, we just want to resolve those unresolved symbols during the linking phase.
   
