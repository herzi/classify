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

#include "window.h"

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#ifndef HAVE_HILDON
#include "preferences.h"
#endif
#include "task-list.h"
#include "task-widget.h"

#include <glib/gi18n.h>

struct _CWindowPrivate {
  GtkUIManager* ui_manager;
};

#define PRIV(i) (((CWindow*)(i))->_private)

static gboolean tree_delete_selected (GtkTreeView * tree);
static void     c_window_init        (CWindow     * self);
static void     c_window_class_init  (CWindowClass* self_class);

#ifdef HAVE_HILDON
#define PARENT_TYPE HILDON_TYPE_WINDOW
#define type_name "CWindowHildon"
#else
#define PARENT_TYPE GTK_TYPE_WINDOW
#define type_name "CWindowDefault"
#endif

static GType c_window_type = 0;

GType
c_window_get_type (void)
{
  if (G_UNLIKELY (!c_window_type))
    {
      GTypeInfo const info = {
        sizeof (CWindowClass),
        NULL, NULL,
        (GClassInitFunc) c_window_class_init,
        NULL, NULL,
        sizeof (CWindow), 0,
        (GInstanceInitFunc) c_window_init,
        NULL
      };
      c_window_type = g_type_register_static (PARENT_TYPE,
                                              type_name,
                                              &info,
                                              0);
    }

  return c_window_type;
}
#undef type_name
#undef PARENT_TYPE

GtkWidget*
c_window_get_button (CWindow* self)
{
	return g_object_get_data (G_OBJECT (self), "CWindow::Button");
}

GtkWidget*
c_window_get_tree (CWindow* self)
{
	return g_object_get_data (G_OBJECT (self), "CWindow::TreeView");
}

static void
file_close_activated (GtkAction* action,
		      CWindow  * self)
{
	gtk_widget_destroy (GTK_WIDGET (self));
}

static void
edit_copy_activated (GtkAction* action,
		     CWindow  * self)
{
	if (GTK_IS_EDITABLE (gtk_window_get_focus (GTK_WINDOW (self)))) {
		gtk_editable_copy_clipboard (GTK_EDITABLE (gtk_window_get_focus (GTK_WINDOW (self))));
	} else {
		GtkTreeSelection* selection;
		GtkTreeModel    * model;
		GtkTreeView     * view;
		GtkTreeIter       iter;
		GList           * selected;

		view      = GTK_TREE_VIEW (c_window_get_tree (self));
		selection = gtk_tree_view_get_selection (view);

		if (1 != gtk_tree_selection_count_selected_rows (selection)) {
			return;
		}

		selected = gtk_tree_selection_get_selected_rows (selection, &model);
		gtk_tree_model_get_iter (model, &iter, selected->data);

		gtk_clipboard_set_text (gtk_clipboard_get_for_display (gtk_widget_get_display (GTK_WIDGET (self)),
								       GDK_SELECTION_CLIPBOARD),
					c_task_list_get_text (C_TASK_LIST (gtk_tree_view_get_model (view)),
							      &iter),
					-1);

		g_list_foreach (selected, (GFunc)gtk_tree_path_free, NULL);
		g_list_free    (selected);
	}
}

static void
edit_delete_activated (GtkAction* action,
		       CWindow  * self)
{
	tree_delete_selected (GTK_TREE_VIEW (c_window_get_tree (self)));
}

static void
clipboard_text_received_cb (GtkClipboard* clipboard,
			    gchar const * text,
			    gpointer      data)
{
	GtkTreeSelection* selection;
	GtkTreeModel    * model = NULL;
	GtkTreeView     * view;
	GtkTreePath     * path;
	GtkTreeIter       iter;
	GtkTreeIter       sibling;
	CWindow         * self = data;
        gboolean          has_sibling;
        GList           * columns;

        view        = GTK_TREE_VIEW (c_window_get_tree (self));
	selection   = gtk_tree_view_get_selection (view);
	has_sibling = 1 == gtk_tree_selection_count_selected_rows (selection);

	if (has_sibling) {
		GList* selected = gtk_tree_selection_get_selected_rows (selection, &model);
		gtk_tree_model_get_iter (model, &sibling, selected->data);
		g_list_foreach (selected, (GFunc)gtk_tree_path_free, NULL);
		g_list_free    (selected);
	} else {
		model = gtk_tree_view_get_model (view);
	}

	c_task_list_append (C_TASK_LIST (model),
			    &iter,
			    has_sibling ? &sibling : NULL,
			    text);

        path = gtk_tree_model_get_path (model, &iter);
        columns = gtk_tree_view_get_columns (view);
        gtk_tree_view_set_cursor (view,
                                  path,
                                  g_list_last (columns)->data,
                                  FALSE);
        g_list_free (columns);
        gtk_tree_path_free (path);
}

static void
edit_paste_activated (GtkAction* action,
		      CWindow  * self)
{
	if (GTK_IS_EDITABLE (gtk_window_get_focus(GTK_WINDOW (self)))) {
		gtk_editable_paste_clipboard (GTK_EDITABLE (gtk_window_get_focus(GTK_WINDOW (self))));
	} else {
		GtkClipboard* clipboard = gtk_clipboard_get_for_display (gtk_widget_get_display (GTK_WIDGET (self)),
									 GDK_SELECTION_CLIPBOARD);

		gtk_clipboard_request_text (clipboard,
					    clipboard_text_received_cb,
					    self);
	}
}

#ifndef HAVE_HILDON
static void
open_prefs (GtkAction* action,
            CWindow  * self)
{
  GtkWidget* dialog = c_preferences_new (GTK_WINDOW (self));
  gtk_dialog_run     (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
}
#endif

static int
renderer_is_text (gconstpointer a,
                  gconstpointer b)
{
  if (GTK_IS_CELL_RENDERER_TEXT (a))
    return 0;

  return 1;
}

static void
tree_edit_path (GtkTreeView* tree,
                GtkTreePath* path)
{
  GtkCellRenderer* renderer;
  GList          * columns   = gtk_tree_view_get_columns (tree);
  GList          * renderers = gtk_tree_view_column_get_cell_renderers (g_list_last (columns)->data);

  renderer = g_list_find_custom (renderers,
                                 NULL,
                                 renderer_is_text)->data;
  g_object_set (renderer, "editable", TRUE, NULL);
  gtk_tree_view_set_cursor_on_cell (tree,
                                    path,
                                    g_list_last (columns)->data,
                                    renderer,
                                    TRUE);
  g_object_set (renderer, "editable", FALSE, NULL);
  g_list_free (renderers);
  g_list_free (columns);
}

static gboolean
tree_rename_selected (GtkTreeView* view)
{
	GList* list;

	if (gtk_tree_selection_count_selected_rows (gtk_tree_view_get_selection (view)) != 1) {
		return FALSE;
	}

	list = gtk_tree_selection_get_selected_rows (gtk_tree_view_get_selection (view), NULL);
	tree_edit_path (view, list->data);
	g_list_foreach (list, (GFunc)gtk_tree_path_free, NULL);
	g_list_free    (list);

	return TRUE;
}

static void
edit_rename (GtkAction* action,
	     CWindow  * self)
{
	tree_rename_selected (GTK_TREE_VIEW (c_window_get_tree (self)));
}

static void
task_bottom_activated (GtkAction* action,
		       CWindow  * self)
{
	GtkTreeSelection* selection;
	GtkTreeIter       iter;
	GList           * selected;

	g_return_if_fail (C_IS_WINDOW (self));

	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (c_window_get_tree (self)));

	if (1 != gtk_tree_selection_count_selected_rows (selection)) {
		return;
	}

	selected = gtk_tree_selection_get_selected_rows (selection, NULL);
	g_return_if_fail (selected);
	g_return_if_fail (gtk_tree_model_get_iter (gtk_tree_view_get_model (GTK_TREE_VIEW (c_window_get_tree (self))), &iter, selected->data));
	gtk_tree_store_move_before (GTK_TREE_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (c_window_get_tree (self)))),
				    &iter,
				    NULL);
	g_list_foreach (selected, (GFunc)gtk_tree_path_free, NULL);
	g_list_free    (selected);
}

static void
task_new_activated (GtkAction* action,
		    CWindow  * self)
{
	GtkTreeSelection* selection;
	GtkTreeView * tree  = GTK_TREE_VIEW  (c_window_get_tree (self));
	CTaskList   * store = C_TASK_LIST (gtk_tree_view_get_model (tree));
	GtkTreePath * path;
	GtkTreeIter   iter;

	selection = gtk_tree_view_get_selection (tree);

	if (gtk_tree_selection_count_selected_rows (selection) == 1) {
		GtkTreeIter  selected_iter;
		GList      * selected = gtk_tree_selection_get_selected_rows (selection, NULL);

		if (selected && gtk_tree_model_get_iter (GTK_TREE_MODEL (store), &selected_iter, selected->data)) {
			c_task_list_append (store,
					    &iter,
					    &selected_iter,
					    _("New Task"));
		} else {
			// FIXME: this is not supposed to happen
			c_task_list_append (store, &iter, NULL, _("New Task"));
		}

		g_list_foreach (selected, (GFunc)gtk_tree_path_free, NULL);
		g_list_free    (selected);
	} else {
		c_task_list_append (store, &iter, NULL, _("New Task"));
	}

	path = gtk_tree_model_get_path (gtk_tree_view_get_model (tree),
				        &iter);

	tree_edit_path (tree, path);

	gtk_tree_path_free (path);
}

static void
task_top_activated (GtkAction* action,
		    CWindow  * self)
{
	GtkTreeSelection* selection;
	GtkTreeIter       iter;
	GList           * selected;

	g_return_if_fail (C_IS_WINDOW (self));

	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (c_window_get_tree (self)));

	if (1 != gtk_tree_selection_count_selected_rows (selection)) {
		return;
	}

	selected = gtk_tree_selection_get_selected_rows (selection, NULL);
	g_return_if_fail (selected);
	g_return_if_fail (gtk_tree_model_get_iter (gtk_tree_view_get_model (GTK_TREE_VIEW (c_window_get_tree (self))), &iter, selected->data));
	gtk_tree_store_move_after (GTK_TREE_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (c_window_get_tree (self)))),
				   &iter,
				   NULL);
	g_list_foreach (selected, (GFunc)gtk_tree_path_free, NULL);
	g_list_free    (selected);
}

static void
view_expand_all_activated (GtkAction* action,
			   CWindow  * self)
{
	gtk_tree_view_expand_all (GTK_TREE_VIEW (c_window_get_tree (self)));
}

static void
view_collapse_all_activated (GtkAction* action,
			     CWindow  * self)
{
	gtk_tree_view_collapse_all (GTK_TREE_VIEW (c_window_get_tree (self)));
}

static gboolean
tree_button_press_event (GtkWidget     * tree,
			 GdkEventButton* event,
			 CWindow       * window)
{
	gboolean result = FALSE;

        if (event->button == 1 && event->type == GDK_2BUTTON_PRESS) {
                GtkTreeSelection* selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (tree));
                if (gtk_tree_selection_count_selected_rows (selection) == 1) {
                        GList* selected = gtk_tree_selection_get_selected_rows (selection, NULL);

                        tree_edit_path (GTK_TREE_VIEW (tree),
                                        selected->data);

                        g_list_foreach (selected, (GFunc)gtk_tree_path_free, NULL);
                        g_list_free    (selected);

                        result = TRUE;
                }
	}

	return result;
}

static gboolean
tree_delete_selected (GtkTreeView* tree)
{
	GtkTreeModel* model = NULL;
	GList       * selected;
	GList       * iter;

	if (gtk_tree_selection_count_selected_rows (gtk_tree_view_get_selection (tree)) == 0) {
		return FALSE;
	}

	selected = gtk_tree_selection_get_selected_rows (gtk_tree_view_get_selection (tree), &model);

	for (iter = selected; iter; iter = iter->next) {
		GtkTreePath* path = iter->data;
		iter->data = gtk_tree_row_reference_new (model, path);
		gtk_tree_path_free (path);
	}
	for (iter = selected; iter; iter = iter->next) {
		GtkTreeIter titer;
		gtk_tree_model_get_iter (model, &titer,
					 gtk_tree_row_reference_get_path (iter->data));
		gtk_tree_store_remove   (GTK_TREE_STORE (model),
					 &titer);
		gtk_tree_row_reference_free (iter->data);
	}

	g_list_free (selected);
	return TRUE;
}

static gboolean
entry_key_press_event (GtkWidget  * widget,
		       GdkEventKey* event,
		       gpointer     user_data)
{
	if ((event->state & GDK_SHIFT_MASK) != 0 &&
	    (event->keyval == GDK_Return || event->keyval == GDK_KP_Enter))
	{
		gint cursor, select;
		g_object_get (widget,
			      "cursor-position", &cursor,
			      "selection-bound", &select,
			      NULL);

		if (cursor != select) {
			gtk_editable_delete_text (GTK_EDITABLE (widget),
						  MIN (cursor, select),
						  MAX (cursor, select));
		}

		g_signal_emit_by_name (widget, "insert-at-cursor",
				       "\n");

		return TRUE;
	}

	return FALSE;
}

static void
editing_started_cb (GtkCellRenderer* renderer,
		    GtkCellEditable* editable,
		    gchar const    * path,
		    gpointer         user_data)
{
  if (GTK_IS_ENTRY (editable))
    {
#ifdef HAVE_HILDON
      hildon_gtk_im_context_show (GTK_ENTRY (editable)->im_context);
#endif
      g_signal_connect (editable, "key-press-event",
                        G_CALLBACK (entry_key_press_event), NULL);
    }
}

static void
edited_cb (GtkCellRendererText* renderer,
	   gchar* path,
	   gchar* new_text,
	   GtkTreeView        * tree)
{
	GtkTreePath* _path = gtk_tree_path_new_from_string (path);
	GtkTreeIter  iter;
	gtk_tree_model_get_iter (gtk_tree_view_get_model (tree), &iter, _path);
	c_task_list_set_text (C_TASK_LIST (gtk_tree_view_get_model (tree)), &iter, new_text);
	gtk_tree_path_free (_path);
}

static void
task_list_data_func (GtkTreeViewColumn* column,
		     GtkCellRenderer  * renderer,
		     GtkTreeModel     * model,
		     GtkTreeIter      * iter,
		     gpointer           data)
{
	g_object_set (renderer,
		      "text", c_task_list_get_text (C_TASK_LIST (model), iter),
		      NULL);
}

GtkWidget*
c_window_new (void)
{
	return g_object_new (C_TYPE_WINDOW, NULL);
}

static gboolean
all_changed (GtkTreeModel* model,
	     GtkTreePath * path,
	     GtkTreeIter * iter,
	     gpointer      data)
{
	gtk_tree_model_row_changed (model, path, iter);
	return FALSE;
}

static void
tree_size_allocate_after (GtkWidget    * tree_widget,
			  GtkAllocation* allocation)
{
	GtkTreeViewColumn* column;
	GtkTreeView      * view;
	GList            * renderers;
	gint               wrap_width;
	gint               focus;
	gint               hspace;
	gint               expander;
	gint               text_pad;
	gint               target_width;

	view      = GTK_TREE_VIEW (tree_widget);
	column    = gtk_tree_view_get_column (view, 0);
	renderers = gtk_tree_view_column_get_cell_renderers (column);

	g_object_get (renderers->data,
		      "xpad",       &text_pad,
		      "wrap-width", &wrap_width,
		      NULL);

	gtk_widget_style_get (tree_widget,
			      "expander-size",        &expander,
			      "focus-padding",        &focus,
			      "horizontal-separator", &hspace,
			      NULL);

	target_width = allocation->width - 2 * (hspace + focus + text_pad);

	if ((gtk_tree_model_get_flags (gtk_tree_view_get_model (view)) & GTK_TREE_MODEL_LIST_ONLY) == 0) {
		if (column == gtk_tree_view_get_expander_column (view)) {
			target_width -= expander + 4 /* EXPANDER_EXTRA_PADDING */;
		}
	}

	target_width = MAX (target_width, 0);

	if (target_width != wrap_width) {
		GtkTreeModel* model;

		gtk_tree_view_column_set_sizing      (column, GTK_TREE_VIEW_COLUMN_FIXED);
		gtk_tree_view_column_set_fixed_width (column, allocation->width);

		g_object_set (renderers->data,
			      "wrap-width", target_width,
			      NULL);

		model = gtk_tree_view_get_model (view);

		gtk_tree_model_foreach (model,
					all_changed,
					NULL);
	}

	g_list_free (renderers);
}

static gboolean
tree_search_equal_func (GtkTreeModel* model,
			gint          column,
			gchar const * key,
			GtkTreeIter * iter,
			gpointer      search_data)
{
	gchar* text = g_utf8_strdown (c_task_list_get_text (C_TASK_LIST (model), iter), -1);
	gchar* key_ = g_utf8_strdown (key, -1);
	gboolean result = g_str_has_prefix (text, key);
	g_free (key_);
	g_free (text);

	return !result;
}

static void
row_has_child_toggled_cb (GtkTreeModel* model,
			  GtkTreePath * path,
			  GtkTreeIter * iter,
			  GtkTreeView * view)
{
	gtk_tree_view_expand_row (view,
				  path,
				  TRUE);
}

#ifdef HAVE_HILDON
#if !GLIB_CHECK_VERSION(2,18,0)
#define g_dgettext(dom,msg) dgettext (dom, msg)
#endif
static void
view_fullscreen (GtkAction* action,
                 GtkWidget* widget)
{
  if ((gdk_window_get_state (widget->window) & GDK_WINDOW_STATE_FULLSCREEN) == 0)
    {
      gtk_window_fullscreen (GTK_WINDOW (widget));
    }
  else
    {
      gtk_window_unfullscreen (GTK_WINDOW (widget));
    }
}

static gboolean
window_state_event (GtkWidget          * widget,
                    GdkEventWindowState* event)
{
  if ((event->changed_mask & GDK_WINDOW_STATE_FULLSCREEN) != 0)
    {
      GtkStockItem  item;
      gchar const * stock_id = (event->new_window_state & GDK_WINDOW_STATE_FULLSCREEN) ? GTK_STOCK_LEAVE_FULLSCREEN : GTK_STOCK_FULLSCREEN;
      GtkAction   * action = gtk_ui_manager_get_action (PRIV (widget)->ui_manager, "/menupopup/ViewToggleFullscreen");

      if (gtk_stock_lookup (stock_id, &item))
        {
          g_object_set (action,
                        "icon-name", "qgn_list_hw_button_view_toggle",
                        "label", g_dgettext (item.translation_domain, item.label),
                        "stock-id", NULL,
                        NULL);
        }
      else
        {
          g_object_set (action,
                        "stock-id", stock_id,
                        NULL);
        }
    }

  return FALSE;
}
#endif

static void
c_window_init (CWindow* self)
{
	GtkActionEntry  entries[] = {
		{"File", NULL, N_("_File")},

		{"FileClose", GTK_STOCK_CLOSE, NULL,
		 NULL, NULL, // FIXME: add tooltip
		 G_CALLBACK (file_close_activated)},

		{"Edit", NULL, N_("_Edit")},
		{"EditCopy", GTK_STOCK_COPY, NULL,
		 NULL, NULL, // FIXME: add tooltip
		 G_CALLBACK (edit_copy_activated)},
		{"EditDelete", GTK_STOCK_DELETE, NULL,
		 "Delete", NULL, // FIXME: add tooltip
		 G_CALLBACK (edit_delete_activated)},
		{"EditPaste", GTK_STOCK_PASTE, NULL,
		 NULL, NULL, // FIXME: add tooltip
		 G_CALLBACK (edit_paste_activated)},
#ifndef HAVE_HILDON
		{"EditPreferences", GTK_STOCK_PREFERENCES, NULL,
		 NULL, NULL, // FIXME: add tooltip
		 G_CALLBACK (open_prefs)},
#endif
		{"EditRename", NULL, N_("_Rename"),
		 "F2", NULL, // FIXME: add tooltip
		 G_CALLBACK (edit_rename)},

		{"TaskBottom", GTK_STOCK_GOTO_BOTTOM, N_("To _Bottom"),
		 NULL, NULL, // FIXME: add tooltip
		 G_CALLBACK (task_bottom_activated)},
		{"TaskNew", GTK_STOCK_ADD, NULL,
		 "<Ctrl>n", NULL, // FIXME: add tooltip
		 G_CALLBACK (task_new_activated)},
		{"TaskTop", GTK_STOCK_GOTO_TOP, N_("To _Top"),
		 NULL, NULL, // FIXME: add tooltip
		 G_CALLBACK (task_top_activated)},

		{"View", NULL, N_("_View")},
		{"ViewExpandAll", NULL, N_("_Expand All"),
		 NULL, NULL, // FIXME: add tooltip
		 G_CALLBACK (view_expand_all_activated)},
		{"ViewCollapseAll", NULL, N_("_Collapse All"),
		 NULL, NULL, // FIXME: add tooltip
                 G_CALLBACK (view_collapse_all_activated)}
#ifdef HAVE_HILDON
                ,{"ViewToggleFullscreen", GTK_STOCK_FULLSCREEN, NULL,
                 "F6", NULL, // FIXME: add tooltip
                 G_CALLBACK (view_fullscreen)}
#endif
	};
	GtkCellRenderer* renderer;
	GtkActionGroup* group;
	CTaskList   * store;
	GtkWidget   * swin;
	GtkWidget   * tree;
	GtkWidget   * vbox  = gtk_vbox_new (FALSE, 0);
        GError      * error = NULL;

        PRIV (self) = G_TYPE_INSTANCE_GET_PRIVATE (self, C_TYPE_WINDOW, CWindowPrivate);

        PRIV (self)->ui_manager = gtk_ui_manager_new ();
        gtk_window_add_accel_group  (GTK_WINDOW (self),
                                     gtk_ui_manager_get_accel_group (PRIV (self)->ui_manager));

        gtk_window_set_default_size (GTK_WINDOW (self),
				     400, 300);
	gtk_window_set_title        (GTK_WINDOW (self),
                                     _("List of Tasks"));
	g_signal_connect (self, "destroy",
                          G_CALLBACK (gtk_main_quit), NULL);

        group = gtk_action_group_new ("main-group");
        gtk_action_group_add_actions (group, entries, G_N_ELEMENTS (entries), self);

#ifdef HAVE_HILDON
        GtkStockItem  item;
        if (gtk_stock_lookup (GTK_STOCK_ADD, &item))
          {
            g_object_set (gtk_action_group_get_action (group, "TaskNew"),
                          "icon-name", "qgn_indi_gene_plus",
                          "label", g_dgettext (item.translation_domain, item.label),
                          "stock-id", NULL,
                          NULL);
          }
        if (gtk_stock_lookup (GTK_STOCK_GOTO_BOTTOM, &item))
          {
            g_object_set (gtk_action_group_get_action (group, "TaskBottom"),
                          "icon-name", "qgn_indi_arrow_down",
                          "label", g_dgettext (item.translation_domain, item.label),
                          "stock-id", NULL,
                          NULL);
          }
        if (gtk_stock_lookup (GTK_STOCK_GOTO_TOP, &item))
          {
            g_object_set (gtk_action_group_get_action (group, "TaskTop"),
                          "icon-name", "qgn_indi_arrow_up",
                          "label", g_dgettext (item.translation_domain, item.label),
                          "stock-id", NULL,
                          NULL);
          }
        if (gtk_stock_lookup (GTK_STOCK_COPY, &item))
          {
            g_object_set (gtk_action_group_get_action (group, "EditCopy"),
                          "icon-name", "qgn_list_gene_copy",
                          "label", g_dgettext (item.translation_domain, item.label),
                          "stock-id", NULL,
                          NULL);
          }
        if (gtk_stock_lookup (GTK_STOCK_PASTE, &item))
          {
            g_object_set (gtk_action_group_get_action (group, "EditPaste"),
                          "icon-name", "qgn_list_gene_paste",
                          "label", g_dgettext (item.translation_domain, item.label),
                          "stock-id", NULL,
                          NULL);
          }
        if (gtk_stock_lookup (GTK_STOCK_DELETE, &item))
          {
            g_object_set (gtk_action_group_get_action (group, "EditDelete"),
                          "icon-name", "qgn_toolb_gene_deletebutton",
                          "label", g_dgettext (item.translation_domain, item.label),
                          "stock-id", NULL,
                          NULL);
          }
        if (gtk_stock_lookup (GTK_STOCK_CLOSE, &item))
          {
            g_object_set (gtk_action_group_get_action (group, "FileClose"),
                          "icon-name", "qgn_toolb_gene_close",
                          "label", g_dgettext (item.translation_domain, item.label),
                          "stock-id", NULL,
                          NULL);
          }
        if (gtk_stock_lookup (GTK_STOCK_FULLSCREEN, &item))
          {
            g_object_set (gtk_action_group_get_action (group, "ViewToggleFullscreen"),
                          "icon-name", "qgn_list_hw_button_view_toggle",
                          "label", g_dgettext (item.translation_domain, item.label),
                          "stock-id", NULL,
                          NULL);
          }
#endif

        gtk_ui_manager_insert_action_group (PRIV (self)->ui_manager,
                                            group,
                                            0);
        g_object_unref (group);

#ifdef HAVE_HILDON
        gtk_ui_manager_add_ui_from_string  (PRIV (self)->ui_manager,
                                            "<ui>"
                                              "<popup name='menupopup'>"
						"<menuitem action='TaskNew'/>"
						"<separator/>"
						"<menu action='Edit'>"
						  "<menuitem action='EditCopy'/>"
						  "<menuitem action='EditPaste'/>"
                                                  "<menuitem action='EditDelete'/>"
                                                  "<separator/>"
                                                /*  "<menuitem action='EditRename'/>" */ // FIXME: doesn't work yet
                                                "</menu>"
                                                "<separator/>"
                                                "<menuitem action='ViewToggleFullscreen'/>"
                                                "<menuitem action='ViewExpandAll'/>"
                                                "<menuitem action='ViewCollapseAll'/>"
                                                "<separator/>"
                                                "<menuitem action='FileClose' />"
					      "</popup>"
					    "</ui>",
					    -1,
					    &error);

        hildon_window_set_menu (HILDON_WINDOW (self),
                                GTK_MENU (gtk_ui_manager_get_widget (PRIV (self)->ui_manager, "/menupopup")));
#else
        gtk_ui_manager_add_ui_from_string  (PRIV (self)->ui_manager,
                                            "<ui>"
						"<menubar name='menubar'>"
							"<menu action='File'>"
								"<menuitem action='TaskNew'/>"
								"<separator/>"
								"<menuitem action='FileClose' />"
							"</menu>"
							"<menu action='Edit'>"
								"<menuitem action='EditCopy'/>"
								"<menuitem action='EditPaste'/>"
								"<menuitem action='EditDelete'/>"
								"<separator/>"
								"<menuitem action='EditRename'/>"
								"<separator/>"
								"<menuitem action='EditPreferences' />"
							"</menu>"
							"<menu action='View'>"
								"<menuitem action='ViewExpandAll'/>"
								"<menuitem action='ViewCollapseAll'/>"
							"</menu>"
						"</menubar>"
					    "</ui>",
					    -1,
					    &error);

        gtk_box_pack_start (GTK_BOX (vbox),
                            gtk_ui_manager_get_widget (PRIV (self)->ui_manager, "/menubar"),
                            FALSE,
                            FALSE,
                            0);
#endif
        gtk_ui_manager_add_ui_from_string  (PRIV (self)->ui_manager,
                                            "<ui>"
						"<toolbar name='toolbar'>"
							"<toolitem action='TaskNew'/>"
							"<separator/>"
							"<toolitem action='TaskTop'/>"
							"<toolitem action='TaskBottom'/>"
						"</toolbar>"
					    "</ui>",
					    -1,
					    &error);

	if (error) {
		g_warning ("Error setting up the user interface: %s",
			   error->message);
		g_error_free (error);
		error = NULL;
	}

#ifdef HAVE_HILDON
        hildon_window_add_toolbar (HILDON_WINDOW (self),
                                   GTK_TOOLBAR (gtk_ui_manager_get_widget (PRIV (self)->ui_manager, "/toolbar")));
#else
        gtk_box_pack_start (GTK_BOX (vbox),
                            gtk_ui_manager_get_widget (PRIV (self)->ui_manager, "/toolbar"),
                            FALSE,
			    FALSE,
			    0);
#endif

	swin = gtk_scrolled_window_new (NULL, NULL);
	//gtk_scrolled_window_set_policy      (GTK_SCROLLED_WINDOW (swin),
	//				     GTK_POLICY_NEVER,
	//				     GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (swin),
					     GTK_SHADOW_IN);
	gtk_widget_show (swin);
	gtk_box_pack_start_defaults (GTK_BOX (vbox), swin);

	tree = c_task_widget_new ();
	gtk_tree_view_set_search_equal_func (GTK_TREE_VIEW (tree),
					     tree_search_equal_func,
					     NULL, NULL);
	gtk_tree_view_set_search_column     (GTK_TREE_VIEW (tree),
					     0);
	g_signal_connect (tree, "button-press-event",
			  G_CALLBACK (tree_button_press_event), self);
	g_signal_connect (tree, "size-allocate",
			        G_CALLBACK (tree_size_allocate_after), NULL);
	g_object_set_data_full (G_OBJECT (self),
				"CWindow::TreeView",
				g_object_ref_sink (tree),
				g_object_unref);
	store = c_task_list_new_default ();
	gtk_tree_view_set_model  (GTK_TREE_VIEW (tree),
				  GTK_TREE_MODEL (store));
	g_signal_connect_after   (store, "row-has-child-toggled",
				  G_CALLBACK (row_has_child_toggled_cb), tree);
	g_object_unref (store);
	gtk_tree_view_expand_all (GTK_TREE_VIEW (tree));
	gtk_widget_show (tree);
	gtk_container_add (GTK_CONTAINER (swin), tree);

	renderer = gtk_cell_renderer_text_new ();
	g_object_set     (renderer,
			  "wrap-mode", PANGO_WRAP_WORD_CHAR,
			  NULL);
	g_signal_connect (renderer, "editing-started",
			  G_CALLBACK (editing_started_cb), NULL);
	g_signal_connect (renderer, "edited",
			  G_CALLBACK (edited_cb), tree);
	gtk_tree_view_insert_column_with_data_func (GTK_TREE_VIEW (tree),
						    -1,
						    _("Task"),
						    renderer,
						    task_list_data_func,
						    NULL, NULL);

        /* FIXME: remove this and properly use the private data */
	g_object_set_data_full (G_OBJECT (self),
                                "CWindow::UIManager",
                                PRIV (self)->ui_manager,
                                g_object_unref);

	gtk_widget_show    (vbox);
        gtk_container_add  (GTK_CONTAINER (self),
                            vbox);

#ifdef HAVE_HILDON
        g_signal_connect (self, "window-state-event",
                          G_CALLBACK (window_state_event), NULL);
#endif
}

static void
c_window_class_init (CWindowClass* self_class)
{
  g_type_class_add_private (self_class, sizeof (CWindowPrivate));
}

