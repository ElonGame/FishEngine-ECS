./compile_shader.sh
cd build
cmake .. -G Xcode
cmake --build . --target demo1 --config Debug
