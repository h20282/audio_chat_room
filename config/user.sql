drop database user;
create database user;
use user;
create table `user`(
    `id` INT(4) PRIMARY KEY AUTO_INCREMENT,
    `username` varchar(64) not null,
    `passwd` varchar(64) not null
);

