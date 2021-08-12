rmdir /s /q build
cmake -B build -DBUILD_FOR_PICO=ON -G "Unix Makefiles"