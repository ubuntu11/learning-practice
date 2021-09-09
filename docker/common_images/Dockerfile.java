FROM adoptopenjdk/openjdk15:alpine-jre

RUN apk --no-cache add bash
RUN apk --no-cache add tzdata && \
        cp /usr/share/zoneinfo/Asia/Taipei /etc/localtime && \
        echo "Asia/Taipei" > /etc/timezone
