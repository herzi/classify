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

#include "task-list-io-xml.h"

#include <errno.h>
#include <glib/gstdio.h>

static gboolean
write_node (GtkTreeModel* model,
	    GtkTreePath * path,
	    GtkTreeIter * iter,
	    gpointer      data)
{
	FILE* file = data;

	fprintf (file, "<task>");
	fprintf (file, "</task>\n");

	return FALSE;
}

void
task_list_io_xml_save (CTaskList  * self,
		       gchar const* path)
{
	FILE* file = fopen (path, "w");
	fprintf (file, "<?xml version=\"1.0\" encoding=\"iso-8859-15\"?>\n");
	fprintf (file, "<tasks>\n");
	gtk_tree_model_foreach (GTK_TREE_MODEL (self),
				write_node,
                                file);
        fprintf (file, "</tasks>\n");
        if (fclose (file) != 0)
          {
            /* FIXME: come up with a proper fallback solution */
            g_warning ("error closing file: %s", strerror (errno));
          }
}

