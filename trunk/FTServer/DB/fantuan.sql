/*
MySQL Data Transfer
Source Host: localhost
Source Database: fantuan
Target Host: localhost
Target Database: fantuan
Date: 2012-10-10 0:38:46
*/

SET FOREIGN_KEY_CHECKS=0;
-- ----------------------------
-- Table structure for account
-- ----------------------------
CREATE TABLE `account` (
  `accountId` bigint(15) NOT NULL AUTO_INCREMENT,
  `accountName` varchar(255) NOT NULL DEFAULT '',
  PRIMARY KEY (`accountId`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Table structure for avatar
-- ----------------------------
CREATE TABLE `avatar` (
  `avatarId` bigint(15) NOT NULL AUTO_INCREMENT,
  `accountId` bigint(15) NOT NULL DEFAULT '0',
  `avatarName` varchar(32) NOT NULL DEFAULT '',
  `lastChannel` tinyint(4) DEFAULT '0',
  PRIMARY KEY (`avatarId`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=latin1;

-- ----------------------------
-- Procedure structure for SP_CreateAvatar
-- ----------------------------
DELIMITER $$

DROP PROCEDURE IF EXISTS `SP_CreateAvatar` $$
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP_CreateAvatar`(IN accountName_ VARCHAR(32), IN avatarName_ VARCHAR(32))
begin
     DECLARE accountId_ bigint;
     DECLARE CheckInt int;
     set CheckInt = 0;
     set accountId_ = (select accountId from account where accountName=accountName_);
     select 1 into CheckInt from avatar where avatarName=avatarName_;
     if (CheckInt = 0) then
         insert into avatar(accountId, avatarName, lastChannel) values(accountId_, avatarName_, 0);
         select last_insert_id();
     end if;
end $$

DELIMITER ;

-- ----------------------------
-- Procedure structure for SP_GetAvatarList
-- ----------------------------
DELIMITER $$

DROP PROCEDURE IF EXISTS `SP_GetAvatarList` $$
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP_GetAvatarList`(IN accountName_ VARCHAR(32))
begin
     DECLARE accountId_ bigint;
     set accountId_ = (select accountId from account where accountName=accountName_);

     if (accountId_ != 0) then
         begin
              select avatarId, accountId, avatarName from avatar where accountId=accountId_;
         end;
     else
         begin
              insert into account(accountName) values(accountName_);
         end;
     end if;
end $$

DELIMITER ;

-- ----------------------------
-- Procedure structure for SP_SelectAvatar
-- ----------------------------
DELIMITER $$

DROP PROCEDURE IF EXISTS `SP_SelectAvatar` $$
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP_SelectAvatar`(IN avatarId_ bigint)
begin
     select lastChannel from avatar where avatarId=avatarId_;
end $$

DELIMITER ;
