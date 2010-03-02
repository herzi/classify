# vim:set ft=automake:

noinst_PROGRAMS+=test-uuid
TESTS+=test-uuid
test_uuid_LDADD=$(CLASSIFY_LIBS)
test_uuid_SOURCES=\
	test/test-uuid.c \
	$(NULL)
