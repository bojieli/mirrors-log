USE mirrors;

DROP TABLE IF EXISTS files;
CREATE TABLE files (
	id INT(10),
	url VARCHAR(255),
	url_0 VARCHAR(255),
	url_1 VARCHAR(255),
	url_2 VARCHAR(255),
	url_3 VARCHAR(255),
	url_4 VARCHAR(255),
	url_5 VARCHAR(255),
	url_6 VARCHAR(255),
	url_7 VARCHAR(255),
	url_8 VARCHAR(255),
	url_9 VARCHAR(255),
	filename VARCHAR(255),
	filename_noext VARCHAR(255),
	extension VARCHAR(7),
	filesize BIGINT(20)
) ENGINE=BRIGHTHOUSE;
