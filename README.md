# The CLTH-N stack

This repository is an example of a web full-stack based on:

- [C Programming Language](https://en.wikipedia.org/wiki/C_(programming_language)) as the backend language
- [Libevent](https://libevent.org/) as the HTTP server library
- [Tailwind CSS](https://tailwindcss.com/) as the CSS library
- [HTMX](https://htmx.org/) as the front-end library
- [NGINX](https://www.nginx.com/) as the static content server

> **NOTE**: I implement it to study how to use asynchronous libraries in C. DO NOT USE IT IN PRODUCTION.


## How to run

You will need `docker` and `docker-compose` installed on your computer/server.

### Debug version
```
docker compose up
```
This will expose an entry point in `localhost:5555` using the debug version. Access it on your browser.

### Release version

Rename the file `Dockerfile-release` to `Dockerfile`, then:
```
docker compose up
```
This will expose an entry point in `localhost:5555` using the release version. Access it on your browser.
