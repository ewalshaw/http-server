# C++ HTTP server

A C++ program that opens a TCP port and answers HTTP requests. The pages are /, /time, /hello-world, and /health.

- `http://localhost:8080/` replies `Hello from C++ HTTP server`
- `http://localhost:8080/time` replies with this computer's current time
- `http://localhost:8080/hello-world` replies with ASCII art that says hello world
- `http://localhost:8080/health` replies with status, port, and uptime in seconds
- Any other path replies `Not Found`

Text after `?` is not part of the path. 
Example: `http://localhost:8080/time?x=1` returns the current time. 

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
curl "http://localhost:8080/"
curl "http://localhost:8080/time"
curl "http://localhost:8080/time?x=1"
curl "http://localhost:8080/hello-world"
curl "http://localhost:8080/health"
curl "http://localhost:8080/anything-else"
curl "http://localhost:8080/missing?x=1"
```

**Expected Output**:

```
Hello from C++ HTTP server
(Current Date and Time)
(Current Date and Time)
(hello world ASCII art)
status: ok
port: 8080
uptime_seconds: (seconds since start)
Not Found
Not Found
```

A browser can open `http://localhost:8080`.

Hello World! ASCII art created with the Ascii Art Archive Text to ASCII tool.