#!/bin/bash

if [ -z "${MYSQL_HOST}" ] || [ -z "${MYSQL_DATABASE}" ] || [ -z "${MYSQL_USER}" ] || [ -z "${MYSQL_PASSWORD}" ]; then 
  echo "One or more MySQL environment variables not found"
  echo "   - MYSQL_HOST"
  echo "   - MYSQL_DATABASE"
  echo "   - MYSQL_USER"
  echo "   - MYSQL_PASSWORD"
  echo "Cannot connect to the database to seed"
  echo "Exiting..."
  exit 1
fi

mysql \
  --host=$MYSQL_HOST \
  --user=$MYSQL_USER \
  --password=$MYSQL_PASSWORD \
  -e "
-- select the database
DROP DATABASE my_database;
CREATE DATABASE my_database;
USE my_database;


-- create the tables
CREATE TABLE IF NOT EXISTS student (
  student_id  INT PRIMARY KEY,
  first_name  VARCHAR(64) NOT NULL,
  last_name   VARCHAR(64) NOT NULL
);
CREATE TABLE IF NOT EXISTS professor (
  professor_id  INT PRIMARY KEY,
  first_name    VARCHAR(64) NOT NULL,
  last_name     VARCHAR(64) NOT NULL
);
CREATE TABLE IF NOT EXISTS course (
  course_id     INT PRIMARY KEY,
  name          VARCHAR(64) NOT NULL,
  professor_id  INT NOT NULL REFERENCES professor(professor_id)
);
CREATE TABLE IF NOT EXISTS student_course (
  course_id     INT,
  student_id    INT,
  PRIMARY KEY (course_id, student_id)
);

-- seed the tables with some default info
INSERT INTO student (
  student_id,
  first_name,
  last_name
)
VALUES (
  1,
  'Andrew',
  'Bazen'
), (
  2,
  'Austen',
  'Jarosz'
), (
  3,
  'Ben',
  'Merritt'
)
ON DUPLICATE KEY UPDATE student_id=student_id;

INSERT INTO professor (
  professor_id,
  first_name,
  last_name
)
VALUES (
  1,
  'Jeffrey',
  'Hemmes'
)
ON DUPLICATE KEY UPDATE professor_id=professor_id;

INSERT INTO course (
  course_id,
  name,
  professor_id
) VALUES (
  1,
  'Distributed Systems',
  1
)
ON DUPLICATE KEY UPDATE course_id=course_id;

INSERT INTO student_course (
  course_id,
  student_id
) VALUES (
  1,
  1
), (
  1,
  2
), (
  1,
  3
)
ON DUPLICATE KEY UPDATE course_id=course_id;
"