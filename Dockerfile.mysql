FROM mysql:latest

RUN test -n $MYSQL_USER
RUN test -n $MYSQL_PASSWORD
RUN test -n $MYSQL_DATABASE

WORKDIR /app

ADD . /app

CMD ["mysqld"]