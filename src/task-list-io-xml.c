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
#include <string.h>
#include <glib/gstdio.h>
#include <libxml/SAX2.h>
#include <gtk/gtk.h>

#define GETTEXT_PACKAGE "classify"
#include <glib/gi18n-lib.h>

struct ParserData {
	guint      unknown_depth;
	CTaskList* task_list;
	GList    * stack;
};

struct StackLevel {
	gpointer     data;
	GtkTreePath* path;
	GString    * string;
};

G_DEFINE_TYPE (CTaskListIOXML, c_task_list_io_xml, C_TYPE_TASK_LIST_IO);

static void
c_task_list_io_xml_init (CTaskListIOXML* self)
{}

static gchar*
task_list_io_xml_path (gchar const* path)
{
	if (g_str_has_suffix (path, ".xml")) {
		return g_strdup (path);
	} else {
		return g_strdup_printf ("%s.xml", path);
	}
}

static gboolean
task_list_io_xml_test (gchar const* path)
{
	gchar* xml_path = task_list_io_xml_path (path);
	gboolean result = g_file_test (xml_path, G_FILE_TEST_IS_REGULAR);
	g_free (xml_path);
	return result;
}

static void
sax_characters_cb (gpointer       ctx,
		   xmlChar const* text,
		   int            length)
{
	struct ParserData* pdata = ((xmlParserCtxt*)ctx)->_private;

	if (pdata->stack) {
		g_string_append_len (((struct StackLevel*)pdata->stack->data)->string,
				     (gchar*)text,
				     length);
	}
}

static void
sax_start_element_cb (gpointer       ctx,
		      xmlChar const* localname,
		      xmlChar const* prefix,
		      xmlChar const* uri,
		      int            n_namespaces,
		      xmlChar const**namespaces,
		      int            n_attributes,
		      int            n_defaulted,
		      xmlChar const**attributes)
{
	struct ParserData* pdata = ((xmlParserCtxt*)ctx)->_private;
	gpointer           data  = NULL;
	GtkTreePath      * path  = NULL;

	if (pdata->unknown_depth) {
		pdata->unknown_depth++;
		return;
	}

	if (!strcmp ((gchar*)localname, "tasks")) {
		// toplevel tasks item
	} else if (!strcmp ((gchar*)localname, "task")) {
		// task item
		GtkTreeIter  iter;
		GtkTreeIter  parent;
		gboolean     has_parent = FALSE;
		gchar      * uuid = NULL;
		gint         i;

		if (((struct StackLevel*)pdata->stack->data)->path) {
			has_parent = gtk_tree_model_get_iter (GTK_TREE_MODEL (pdata->task_list),
							      &parent,
							      ((struct StackLevel*)pdata->stack->data)->path);
		}

		for (i = 0; i < n_attributes; i++) {
			if (!strcmp ((gchar*)attributes[5*i], "uuid") && !uuid) {
				uuid = g_strndup ((gchar*)attributes[5*i+3], attributes[5*i+4] - attributes[5*i+3]);
			}
		}
		data = c_task_new_with_uuid (uuid);
		g_free (uuid);

		c_task_list_append_task (pdata->task_list,
					 &iter,
					 has_parent ? &parent : NULL,
					 data);
		path = gtk_tree_model_get_path (GTK_TREE_MODEL (pdata->task_list),
						&iter);
	} else {
		g_warning ("unknown tag <%s%s%s> read",
			   prefix ? (char const*)prefix : "",
			   prefix ? ":" : "",
			   localname);
		pdata->unknown_depth++;
		return;
	}

	pdata->stack = g_list_prepend (pdata->stack, g_slice_new0 (struct StackLevel));
	((struct StackLevel*)pdata->stack->data)->data   = data;
	((struct StackLevel*)pdata->stack->data)->string = g_string_new ("");
	((struct StackLevel*)pdata->stack->data)->path   = path;
}

static void
sax_end_element_cb (gpointer       ctx,
		    xmlChar const* localname,
		    xmlChar const* prefix,
		    xmlChar const* uri)
{
	struct ParserData* pdata = ((xmlParserCtxt*)ctx)->_private;

	if (pdata->unknown_depth) {
		pdata->unknown_depth--;
		return;
	}

	if (!strcmp ((gchar*)localname, "task")) {
		c_task_set_text    (((struct StackLevel*)pdata->stack->data)->data,
				    ((struct StackLevel*)pdata->stack->data)->string->str);
	}

	gtk_tree_path_free (((struct StackLevel*)pdata->stack->data)->path);
	g_string_free (((struct StackLevel*)pdata->stack->data)->string, TRUE);
	g_slice_free  (struct StackLevel, pdata->stack->data);
	pdata->stack = g_list_delete_link (pdata->stack, pdata->stack);
}

static void
log_v (gpointer        ctx,
       gchar const   * message,
       va_list         argv,
       GLogLevelFlags  flags)
{
	gchar* full_msg = g_strdup_vprintf (message, argv);
	gchar* location = g_strdup_printf ("%s:%d:%d: %s",
					   xmlSAX2GetSystemId (ctx),
					   xmlSAX2GetLineNumber (ctx),
					   xmlSAX2GetColumnNumber (ctx),
					   full_msg);
	g_log  (G_LOG_DOMAIN, flags, "%s", location);
	g_free (location);
	g_free (full_msg);
}

static void
sax_warning_cb (void      * ctx,
		const char* message,
		...)
{
	va_list list;
	va_start (list, message);
	log_v    (ctx, message, list, G_LOG_LEVEL_WARNING);
	va_end   (list);
}

static void
sax_error_cb (void      * ctx,
	      const char* message,
	      ...)
{
	va_list list;
	va_start (list, message);
	log_v    (ctx, message, list, G_LOG_LEVEL_ERROR);
	va_end   (list);
}

static void
task_list_io_xml_load (CTaskList  * self,
		       gchar const* path)
{
	xmlSAXHandler sax = {
		NULL, // internalSubset
		NULL, // isStandalone
		NULL, // hasInternalSubset
		NULL, // hasExternalSubset
		NULL, // resolveEntity
		NULL, // getEntity
		NULL, // entityDecl
		NULL, // notationDecl
		NULL, // attributeDecl
		NULL, // elementDecl
		NULL, // unparsedEntityDecl
		NULL, // setDocumentLocator
		NULL, // startDocument
		NULL, // endDocument
		NULL, // startElement
		NULL, // endElement
		NULL, // reference
		sax_characters_cb,
		NULL, // ignorableWhitespace
		NULL, // processingInstruction
		NULL, // comment
		sax_warning_cb,
		sax_error_cb,
		NULL, // fatalError (won't be called)
		NULL, // getParameterEntity
		NULL, // cdataBlock
		NULL, // externalSubset
		XML_SAX2_MAGIC, // initialized
		NULL, // _private
		sax_start_element_cb,
		sax_end_element_cb,
		NULL  // serror
	};
	struct ParserData pdata = {
		0,
		self,
		NULL
	};

	gchar* xml_path = task_list_io_xml_path (path);
	xmlSAXParseFileWithData (&sax,
				 xml_path,
				 0,
				 &pdata);
	g_free (xml_path);
}

// FIXME: merge dump_nodes() and write_node()
static void dump_nodes (CTaskList  * self,
			FILE       * file,
			GtkTreeIter* iter);

static void
write_node (GtkTreeModel* model,
	    GtkTreeIter * iter,
	    FILE        * file)
{
	gchar const* text;
	CTask      * task = c_task_list_get_task (C_TASK_LIST (model), iter);

	text = c_task_get_text (task);
	g_return_if_fail (g_utf8_validate (text, -1, NULL));

	fprintf (file, "<task uuid=\"%s\">", c_task_get_uuid (task));
	for (; text && *text; text = g_utf8_next_char (text)) {
		gunichar c = g_utf8_get_char (text);

		if (c < 32 && c != 10) {
			// ASCII control chars
			g_warning ("FIXME: ascii control char %d encountered, ignoring.",
				   c);
		} else if (c > 127) {
			// UTF-8 multibyte chars
			fprintf (file, "&#%d;", c);
		} else {
			gchar const* entity = NULL;

			switch (c) {
			case '"':
				entity = "quot";
				break;
			case '\'':
				entity = "apos";
				break;
			case '&':
				entity = "amp";
				break;
			case '<':
				entity = "lt";
				break;
			case '>':
				entity = "gt";
				break;
			}

			if (G_UNLIKELY (entity)) {
				fprintf (file, "&%s;", entity);
			} else {
				fprintf (file, "%c", c);
			}
		}
	}

	dump_nodes (C_TASK_LIST (model), file, iter);

	fprintf (file, "</task>");
}

static void
dump_nodes (CTaskList  * self,
	    FILE       * file,
	    GtkTreeIter* iter)
{
	GtkTreeIter  iter2;
	gboolean     result;

	for (result = gtk_tree_model_iter_nth_child (GTK_TREE_MODEL (self), &iter2, iter, 0);
	     result;
	     result = gtk_tree_model_iter_next (GTK_TREE_MODEL (self), &iter2))
	{
		write_node (GTK_TREE_MODEL (self),
			    &iter2,
			    file);
	}
}

static void
task_list_io_xml_save (CTaskList  * self,
                       gchar const* path)
{
        GMappedFile* old_version;
        gchar* xml_path = task_list_io_xml_path (path);
        FILE* file;

	/* we don't care if this fails */
	old_version = g_mapped_file_new (xml_path, FALSE, NULL);
	if (old_version) {
		gchar* backup_path = g_strdup_printf ("%s.%li", xml_path, time (NULL));
		g_file_set_contents (backup_path,
				     g_mapped_file_get_contents (old_version),
				     g_mapped_file_get_length (old_version),
				     NULL);
		g_free (backup_path);
		g_mapped_file_free (old_version);
	}

        file = fopen (xml_path, "w");
        if (!file)
          {
            int old_errno = errno;
            GtkWidget* dialog;

            if (old_errno == ENOENT)
              {
                gchar* folder = g_path_get_dirname (xml_path);
                if (g_mkdir_with_parents (folder, 0755) == 0)
                  {
                    file = fopen (xml_path, "w");
                  }

                g_free (folder);

                if (file)
                  {
                    goto cont;
                  }
              }

            /* FIXME: the user should be able to save to a different place */
            old_errno = errno;
            dialog = gtk_message_dialog_new (NULL, /* FIXME: add window */
                                             0, /* FIXME: make modal */
                                             GTK_MESSAGE_ERROR,
                                             GTK_BUTTONS_CLOSE,
                                             "%s", _("Error saving to file"));
            gtk_message_dialog_format_secondary_markup (GTK_MESSAGE_DIALOG (dialog),
                                                        _("Couldn't open file \"%s\" for writing:\n\n"
                                                          "<span style='italic'>%s</span>"),
                                                        xml_path,
                                                        strerror (old_errno));

            gtk_dialog_run (GTK_DIALOG (dialog));
            gtk_widget_destroy (dialog);

            goto cleanup;
          }
cont:
        fprintf (file, "<?xml version=\"1.0\" encoding=\"iso-8859-15\"?>\n");
        fprintf (file, "<tasks>\n");
        dump_nodes (self, file, NULL);
        fprintf (file, "</tasks>\n");
        if (fclose (file) != 0)
          {
            /* FIXME: come up with a proper fallback solution */
            g_warning ("error closing file: %s", strerror (errno));
          }
cleanup:
        g_free (xml_path);
}

static void
c_task_list_io_xml_class_init (CTaskListIOXMLClass* self_class)
{
	CTaskListIOClass* io_class = C_TASK_LIST_IO_CLASS (self_class);

	io_class->load = task_list_io_xml_load;
	io_class->save = task_list_io_xml_save;
	io_class->test = task_list_io_xml_test;
}

