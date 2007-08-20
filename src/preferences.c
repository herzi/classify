/* This file is part of classify
 *
 * AUTHORS
 *     Sven Herzberg  <herzi@gnome-de.org>
 *
 * Copyright (C) 2007  Sven Herzberg
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

#include "preferences.h"

#include <gtk/gtk.h>

#include <glib/gi18n.h>

static void
checkbutton_toggled_cb (GtkToggleButton* button,
			GKeyFile       * keyfile)
{
	gchar* data;
	gchar* path;
	g_key_file_set_boolean (keyfile,
				"Desktop Entry",
				"X-GNOME-Autostart-enabled",
				gtk_toggle_button_get_active (button));
	path = g_build_filename   (g_get_home_dir (),
				   ".config",
				   "autostart",
				   "classify.desktop",
				   NULL);
	data = g_key_file_to_data (keyfile, NULL, NULL);
	g_file_set_contents (path,
			     data,
			     -1,
			     NULL);
	g_free (data);
	g_free (path);
}

GtkWidget*
c_preferences_new (GtkWindow* parent)
{
	gchar* path;
	GtkWidget* checkbutton;
	GtkWidget* result = gtk_dialog_new_with_buttons (_("Preferences"),
					    parent,
					    GTK_DIALOG_MODAL | GTK_DIALOG_NO_SEPARATOR,
					    GTK_STOCK_CLOSE,
					    GTK_RESPONSE_ACCEPT,
					    NULL);
	gtk_dialog_set_default_response (GTK_DIALOG (result),
					 GTK_RESPONSE_ACCEPT);

	checkbutton = gtk_check_button_new_with_mnemonic (_("_Automatically start when logging in"));
	gtk_widget_show    (checkbutton);
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (result)->vbox),
			    checkbutton,
			    FALSE, FALSE, 0);

	GKeyFile* keyfile = g_key_file_new ();
	path = g_build_filename (g_get_home_dir (),
				 ".config",
				 "autostart",
				 "classify.desktop",
				 NULL);
	if (!g_key_file_load_from_file (keyfile, path, G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS, NULL)) {
		g_key_file_set_string  (keyfile, "Desktop Entry", "Version", "1.0");
		g_key_file_set_string  (keyfile, "Desktop Entry", "Type", "Application");
		g_key_file_set_string  (keyfile, "Desktop Entry", "Name", "Tasks Application");
		g_key_file_set_string  (keyfile, "Desktop Entry", "Comment", "Manage your daily tasks");
		g_key_file_set_string  (keyfile, "Desktop Entry", "Exec", BIN_DIR "/classify");
		g_key_file_set_string  (keyfile, "Desktop Entry", "Icon", "classify");
		g_key_file_set_string  (keyfile, "Desktop Entry", "Categories", "");
		g_key_file_set_string  (keyfile, "Desktop Entry", "OnlyShowIn", "GNOME;");
		g_key_file_set_boolean (keyfile, "Desktop Entry", "X-GNOME-Autostart-enabled", TRUE);
	}
	g_free (path);

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton),
				      g_key_file_get_boolean (keyfile, "Desktop Entry",
							      "X-GNOME-Autostart-enabled", NULL));

	g_signal_connect (checkbutton, "toggled",
			  G_CALLBACK (checkbutton_toggled_cb), keyfile);

	g_object_set_data_full (G_OBJECT (result),
				"CPreferences::Keyfile",
				keyfile,
				(GDestroyNotify)g_key_file_free);

	return result;
}

