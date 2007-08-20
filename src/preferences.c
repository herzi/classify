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

GtkWidget*
c_preferences_new (GtkWindow* parent)
{
	GtkWidget* result = gtk_dialog_new_with_buttons (_("Preferences"),
					    parent,
					    GTK_DIALOG_MODAL | GTK_DIALOG_NO_SEPARATOR,
					    GTK_STOCK_CLOSE,
					    GTK_RESPONSE_ACCEPT,
					    NULL);
	gtk_dialog_set_default_response (GTK_DIALOG (result),
					 GTK_RESPONSE_ACCEPT);
	return result;
}

