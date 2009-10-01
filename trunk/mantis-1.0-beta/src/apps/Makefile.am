
SUBDIRS = blink_led flash_test msgs_test dev_test

if ARCH_MICRO
SUBDIRS += blink_led led_example printf_example 
endif

if PLATFORM_MICA_ANY
SUBDIRS += 
endif

if PLATFORM_TELOSB
SUBDIRS += 
endif

.PHONY: $(APPS)
$(APPS):
	@echo "Making all in $@"
	@cd $@ && make
