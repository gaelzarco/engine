cmake -B build -G Ninja
ln -sf build/compile_commands.json .
cmake --build build
