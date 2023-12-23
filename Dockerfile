FROM alpine:3.19 as base
RUN apk update && apk add libevent

FROM base as build
RUN apk add bsd-compat-headers build-base libevent-dev make
COPY Makefile /
COPY ./src ./src
RUN make debug

FROM base as c-http-api
COPY --from=build main /
ENV PATH="${PATH}:/"
EXPOSE 5555
ENTRYPOINT ["main"]