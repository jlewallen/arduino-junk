SUBDIRS = blink i2c_1 max7219 switch_1 httpd

all:
	@for i in $(SUBDIRS); do \
	(cd $$i; $(MAKE) all); done

clean:
	@for i in $(SUBDIRS); do \
	(cd $$i; $(MAKE) clean); done