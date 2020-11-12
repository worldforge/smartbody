pushd vhmsg-c
"c:\Program Files\doxygen\bin\doxygen.exe"
popd

pushd vhmsg-tt
"c:\Program Files\doxygen\bin\doxygen.exe"
popd

pushd vhmsg-tcl
"c:\Program Files\doxygen\bin\doxygen.exe"
popd

pushd vhmsg-java
call %ANT_HOME%\bin\ant javadoc
popd

pushd vhmsg-net
"c:\Program Files\Sandcastle Help File Builder\SandcastleBuilderConsole.exe" vhmsg-net.shfb
popd
