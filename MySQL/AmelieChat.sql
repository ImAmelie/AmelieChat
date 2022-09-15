CREATE DATABASE IF NOT EXISTS `AmelieChat` CHARACTER SET utf8;

USE `AmelieChat`;

CREATE TABLE `user`(
    `id` int PRIMARY KEY AUTO_INCREMENT,
    `username` VARCHAR(20) UNIQUE NOT NULL,
    `password` VARCHAR(40) NOT NULL
);

-- 以下3行用来创建用户，用户名和密码都是 chat
-- CREATE USER 'chat'@'%' IDENTIFIED BY 'chat';
-- GRANT ALL ON `AmelieChat`.* TO 'chat'@'%';
-- flush privileges;
