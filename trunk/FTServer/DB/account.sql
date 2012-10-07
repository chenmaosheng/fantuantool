CREATE DATABASE `account` /*!40100 DEFAULT CHARACTER SET latin1 */;
DROP TABLE IF EXISTS `account`.`account`;
CREATE TABLE  `account`.`account` (
  `accountId` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `accountName` varchar(32) NOT NULL,
  `password` varchar(32) NOT NULL,
  PRIMARY KEY (`accountId`),
  UNIQUE KEY `nameIndex` (`accountName`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=latin1;