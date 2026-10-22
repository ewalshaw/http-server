# C++ HTTP server

A C++ program that opens a TCP port and answers HTTP requests. The pages are /, /time, and /hello-world.

- `http://localhost:8080/` replies `Hello from C++ HTTP server`
- `http://localhost:8080/time` replies with this computer's current time
- `http://localhost:8080/hello-world` replies with ASCII art that says hello world
- Any other path replies `404 Not Found`

## Requirements

- Windows
- `g++` with `C++17` support. 
- `curl` (optional)

## Compile

From this folder:

```
g++ -std=c++17 -Wall -Wextra -Wpedantic -o http-server.exe src/main.cpp src/server.cpp src/request.cpp src/response.cpp src/router.cpp -lws2_32
```



## Run

```
./http-server.exe
```

When ready, it prints `listening on port 8080`.
Open a second terminal and run:

```
curl http://localhost:8080/
curl http://localhost:8080/time
curl http://localhost:8080/hello-world
curl http://localhost:8080/anything-else
```

**Expected Output**:

```
Hello from C++ HTTP server
(Current Date and Time)
(hello world ASCII art)
Not Found
```

A browser can open `http://localhost:8080`.

Hello World! ASCII art created with the Ascii Art Archive Text to ASCII tool.
