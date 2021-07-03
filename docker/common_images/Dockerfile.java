# FROM adoptopenjdk/openjdk16:alpine-jre
FROM adoptopenjdk/openjdk15:alpine-jre
# FROM alpine:latest
# RUN apk --update --no-cache add openjdk11-jre

RUN apk --no-cache add bash
RUN apk --no-cache add tzdata && \
        cp /usr/share/zoneinfo/Asia/Taipei /etc/localtime && \
        echo "Asia/Taipei" > /etc/timezone

