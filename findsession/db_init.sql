USE mirrors;

-- sess_gap
DROP TABLE IF EXISTS sess;
CREATE TABLE sess (
	ipv4 BIGINT(10),
	ipv4_0 SMALLINT(3),
	ipv4_1 SMALLINT(3),
	ipv4_2 SMALLINT(3),
	ipv4_3 SMALLINT(3),
	ipv6_0 BIGINT(10),
	ipv6_1 BIGINT(10),
	ipv6_2 BIGINT(10),
	ipv6_3 BIGINT(10),
	duration INT(10) NOT NULL,
	start_time INT(10) NOT NULL,
	requests INT(10) NOT NULL,
	traffic INT(10) NOT NULL,
	dist VARCHAR(31), 
	ua_0 VARCHAR(31)
) ENGINE=BRIGHTHOUSE;

-- sess_interval
DROP TABLE IF EXISTS sess_int;
CREATE TABLE sess_int (
	ipv4 BIGINT(10),
	ipv4_0 SMALLINT(3),
	ipv4_1 SMALLINT(3),
	ipv4_2 SMALLINT(3),
	ipv4_3 SMALLINT(3),
	ipv6_0 BIGINT(10),
	ipv6_1 BIGINT(10),
	ipv6_2 BIGINT(10),
	ipv6_3 BIGINT(10),
	duration INT(10) NOT NULL,
	start_time INT(10) NOT NULL,
	requests INT(10) NOT NULL,
	traffic INT(10) NOT NULL,
	dist VARCHAR(31), 
	ua_0 VARCHAR(31)
) ENGINE=BRIGHTHOUSE;
