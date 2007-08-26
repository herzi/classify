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
#include <libxml/SAX2.h>

struct ParserData {
	guint      unknown_depth;
	CTaskList* task_list;
	GList    * stack;
};

static void
sax_characters_cb (gpointer       ctx,
		   xmlChar const* text,
		   int            length)
{
	struct ParserData* pdata = ((xmlParserCtxt*)ctx)->_private;

	if (pdata->stack) {
		g_string_append_len (pdata->stack->data,
				     text,
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

	if (pdata->unknown_depth) {
		pdata->unknown_depth++;
		return;
	}

	if (!strcmp (localname, "tasks")) {
		// toplevel tasks item
	} else if (!strcmp (localname, "task")) {
		// task item
		pdata->stack = g_list_prepend (pdata->stack, g_string_new (""));
	} else {
		g_warning ("unknown tag <%s%s%s> read",
			   prefix ? (char const*)prefix : "",
			   prefix ? ":" : "",
			   localname);
		pdata->unknown_depth++;
	}
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

	if (!strcmp (localname, "task")) {
		c_task_list_append (pdata->task_list,
				    NULL, NULL,
				    ((GString*)pdata->stack->data)->str);

		g_string_free (pdata->stack->data, TRUE);
		pdata->stack = g_list_delete_link (pdata->stack, pdata->stack);
	}
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

void
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

	xmlSAXParseFileWithData (&sax,
				 path,
				 0,
				 &pdata);
}

static gboolean
write_node (GtkTreeModel* model,
	    GtkTreePath * path,
	    GtkTreeIter * iter,
	    gpointer      data)
{
	gchar const* text;
	CTask      * task = c_task_list_get_task (C_TASK_LIST (model), iter);
	FILE       * file = data;

	text = c_task_get_text (task);
	g_return_val_if_fail (g_utf8_validate (text, -1, NULL), FALSE);

	fprintf (file, "<task>");
	for (; text && *text; text = g_utf8_next_char (text)) {
		gunichar c = g_utf8_get_char (text);

		if (c < 32) {
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

