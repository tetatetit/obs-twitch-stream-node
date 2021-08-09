# obs-twitch-stream-node

Simplest node native module example that streams to Twitch via `libobs` with just a single function call

# Build

## Windows

```
yarn install
mkdir build
cd build
cmake .. -G"Visual Studio 15 2017" -A x64
cmake --build .
cpack -G ZIP -C Debug
```