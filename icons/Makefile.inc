# vim:set ft=automake:

icon16_DATA=icons/16x16/classify.png
icon16dir=$(datadir)/icons/hicolor/16x16/apps

icon22_DATA=icons/22x22/classify.png
icon22dir=$(datadir)/icons/hicolor/22x22/apps

icon24_DATA=icons/24x24/classify.png
icon24dir=$(datadir)/icons/hicolor/24x24/apps

icon32_DATA=icons/32x32/classify.png
icon32dir=$(datadir)/icons/hicolor/32x32/apps

icon48_DATA=icons/48x48/classify.png
icon48dir=$(datadir)/icons/hicolor/48x48/apps

if WITH_MAEMO
icon26_DATA=icons/26x26/hildon/classify.png
icon26dir=$(datadir)/icons/hicolor/26x26/hildon

icon40_DATA=icons/40x40/hildon/classify.png
icon40dir=$(datadir)/icons/hicolor/40x40/hildon

icon64_DATA=icons/64x64/hildon/classify.png
icon64dir=$(datadir)/icons/hicolor/scalable/hildon
endif

EXTRA_DIST+=\
	$(icon16_DATA) \
	$(icon22_DATA) \
	$(icon24_DATA) \
	$(icon32_DATA) \
	$(icon48_DATA) \
	icons/26x26/hildon/classify.png \
	icons/40x40/hildon/classify.png \
	icons/64x64/hildon/classify.png \
	$(NULL)

# install-data-hook is run after all install rules
install-data-hook:
	@if test -z "$(DESTDIR)"; then \
		echo "gtk-update-icon-cache -t -f '$(datadir)/icons/hicolor'"; \
		gtk-update-icon-cache -t -f "$(datadir)/icons/hicolor"; \
	else \
		echo "****" >&2; \
		echo "** Please run \"gtk-update-icon-cache -t -f '$(datadir)/icons/hicolor'\"" >&2; \
		echo "****" >&2; \
	fi

