CREATE DATABASE `fantuan` /*!40100 DEFAULT CHARACTER SET latin1 */;

DROP TABLE IF EXISTS `fantuan`.`avatar`;
CREATE TABLE  `fantuan`.`avatar` (
  `avatarId` bigint(15) NOT NULL AUTO_INCREMENT,
  `accountId` bigint(15) NOT NULL DEFAULT 0,
  `avatarName` varchar(32) NOT NULL DEFAULT '',
  PRIMARY KEY (`avatarId`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=latin1;

DROP PROCEDURE IF EXISTS `fantuan`.`SP_CreateAvatar`;
CREATE PROCEDURE  `fantuan`.`SP_CreateAvatar`(IN accountId_ bigint, IN avatarName_ VARCHAR(32), OUT avatarId_ bigint)
begin
     if not exists (select 'true' from avatar where avatarName=avatarName_) then
         insert into avatar(accountId, avatarName) values(accountId_, avatarName_);
         set avatarId_ = last_insert_id();
     end if;
end;