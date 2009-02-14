# vim:set ft=make:

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

