all:
	@for i in */Makefile; do \
	(echo for `dirname $$i`; cd `dirname $$i` && $(MAKE) all); done

clean:
	@for i in */Makefile; do \
	(echo for `dirname $$i`; cd `dirname $$i` && $(MAKE) clean); done
