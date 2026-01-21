my-web-server
---

A minimal yet practical Web Server implemented in CPP, built from scratch based on socket programming, I/O multiplexing,
and a thread pool.

This project focuses on understanding how a web server works internally rather than providing a full-featured production
server.

### Features and Design Goals

- Low-level POSIX socket programming
- I/O multiplexing, actually selector for high concurrency
- Thread pool to separate I/O and biz logic
- Basic HTTP/1.1 request and response support
- Designed for learning network programing and server architecture


### Architecture

```text
        +-------------+
        |   Clients   |
        +------+------+
               |
               | TCP
               v
        +------+------+
        |   Socket     |   (bind / listen / accept)
        +------+------+
               |
               v
        +------+------+
        |  Selector    |   (kqueue / epoll)
        +------+------+
               |
        +------+------+
        | Thread Pool  |   (worker threads)
        +------+------+
               |
               v
        +------+------+
        | HTTP Handler |   (parse & response)
        +-------------+
```

### Testing and Debugging Tools

#### ApacheBench(ab)

Used for basic performance and concurrency testing.

- Install

    ```sh
    sudo apt install apache2-utils
    ```

- Verify

    ```sh
    ab -V
    ```
 
- Test
    ```sh
    ab -n 1000 -c 10 http://127.0.0.1:9527/
    ```

#### Telnet

Used to manually test HTTP.

- Install

    ```sh
    sudo apt install telnet
    ```
- Test

    ```sh
    telnet 127.0.0.1 9527
    GET / HTTP/1.1
    ```

    ```sh
    telnet 127.0.0.1 9527
    GET /index.html HTTP/1.1
    ```
