DELETE FROM `test`;
INSERT INTO `test` (`id`, `name`, `fk_id`) VALUES
	('8e54e0b6-b2f2-11e2-903f-78acc0a0840e', 'a', '3949b594-b2f2-11e2-903f-78acc0a0840e'),
	('93828ee1-b2f2-11e2-903f-78acc0a0840e', 'b', '3bb262b6-b2f2-11e2-903f-78acc0a0840e'),
	('95e3a9a9-b2f2-11e2-903f-78acc0a0840e', 'c', '3cb76fe2-b2f2-11e2-903f-78acc0a0840e'),
	('9877ae8c-b2f2-11e2-903f-78acc0a0840e', 'd', '3e1f5e5c-b2f2-11e2-903f-78acc0a0840e');
	
DELETE FROM `test_fk`;
INSERT INTO `test_fk` (`id`, `asset`) VALUES
	('3949b594-b2f2-11e2-903f-78acc0a0840e', 1),
	('3bb262b6-b2f2-11e2-903f-78acc0a0840e', 2),
	('3cb76fe2-b2f2-11e2-903f-78acc0a0840e', 3),
	('3e1f5e5c-b2f2-11e2-903f-78acc0a0840e', 4);