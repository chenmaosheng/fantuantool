CREATE DATABASE `fantuan` /*!40100 DEFAULT CHARACTER SET latin1 */;

DROP TABLE IF EXISTS `fantuan`.`avatar`;
CREATE TABLE  `fantuan`.`avatar` (
  `avatarId` bigint(15) NOT NULL AUTO_INCREMENT,
  `accountName` varchar(32) NOT NULL DEFAULT '',
  `avatarName` varchar(32) NOT NULL DEFAULT '',
  PRIMARY KEY (`avatarId`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=latin1;

DELIMITER $$

DROP PROCEDURE IF EXISTS `fantuan`.`SP_CreateAvatar`$$
CREATE DEFINER=`root`@`localhost` PROCEDURE  `fantuan`.`SP_CreateAvatar`(IN accountName_ VARCHAR(32), IN avatarName_ VARCHAR(32), OUT avatarId_ bigint)
begin
     if not exists (select 'true' from avatar where avatarName=avatarName_) then
         insert into avatar(accountName, avatarName) values(accountName_, avatarName_);
         set avatarId_ = last_insert_id();
     end if;
end $$

DELIMITER ;