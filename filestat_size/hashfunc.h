// see php-src/Zend/zend_hash.h for DJBX33A
inline unsigned long hash_DJBX33A(char *key, unsigned long length, unsigned long seed) {
	register unsigned long hash = seed;
	
	for (; length >= 8; length -= 8) {
		hash = ((hash << 5) + hash) + *key++;
		hash = ((hash << 5) + hash) + *key++;
		hash = ((hash << 5) + hash) + *key++;
		hash = ((hash << 5) + hash) + *key++;
		hash = ((hash << 5) + hash) + *key++;
		hash = ((hash << 5) + hash) + *key++;
		hash = ((hash << 5) + hash) + *key++;
		hash = ((hash << 5) + hash) + *key++;
	}
	switch (length) {
		case 7: hash = ((hash << 5) + hash) + *key++;
		case 6: hash = ((hash << 5) + hash) + *key++;
		case 5: hash = ((hash << 5) + hash) + *key++;
		case 4: hash = ((hash << 5) + hash) + *key++;
		case 3: hash = ((hash << 5) + hash) + *key++;
		case 2: hash = ((hash << 5) + hash) + *key++;
		case 1: hash = ((hash << 5) + hash) + *key++;
		case 0: break;
	}
	return hash;
}

inline unsigned long first_hash(char *key, unsigned long length) {
	return hash_DJBX33A(key, length, 5381); // the magic number in PHP
}

inline unsigned long check_hash1(char *key, unsigned long length) {
	return hash_DJBX33A(key, length, 0); // the magic number in Apache
}

inline unsigned long check_hash2(char *key, unsigned long length) {
	return hash_DJBX33A(key, length, 2011); // an arbitrary prime number
}
