/* This file is part of classify
 *
 * AUTHORS
 *     Sven Herzberg  <herzi@lanedo.com>
 *
 * Copyright (C) 2009  Sven Herzberg
 *
 * This work is provided "as is"; redistribution and modification
 * in whole or in part, in any medium, physical or electronic is
 * permitted without restriction.
 *
 * This work is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * In no event shall the authors or contributors be liable for any
 * direct, indirect, incidental, special, exemplary, or consequential
 * damages (including, but not limited to, procurement of substitute
 * goods or services; loss of use, data, or profits; or business
 * interruption) however caused and on any theory of liability, whether
 * in contract, strict liability, or tort (including negligence or
 * otherwise) arising in any way out of the use of this software, even
 * if advised of the possibility of such damage.
 */

#include <string.h>
#include <gtk/gtk.h>
#if defined(HAVE_G_TEST) && ! GTK_CHECK_VERSION (2,14,0)
#include <gtk/gtktestutils.h>
#else
#define g_test_init(argc,argv,...) g_log_set_always_fatal(G_LOG_LEVEL_WARNING|G_LOG_LEVEL_CRITICAL)
#define gtk_test_init(argc,argv,...) g_test_init (argc,argv, NULL); gtk_init(argc, argv)
#endif
#include "main-window.h"
#include "user-interface.h"

static gboolean
failsave_quit (gpointer unused)
{
  g_warning ("destroying the last window should stop the main loop");
  gtk_main_quit ();
  return FALSE;
}

static void
test_create_window (CUserInterface* ui)
{
  GtkWidget* widget;

  g_type_module_use (G_TYPE_MODULE (ui));

  widget = c_user_interface_get_main_window (ui);

  g_assert (GTK_IS_WIDGET (widget));
  g_assert (C_IS_MAIN_WINDOW (widget));

  g_timeout_add (30, (GSourceFunc)gtk_widget_destroy, widget);
  g_timeout_add (300, failsave_quit, NULL);
  gtk_main ();

  g_type_module_unuse (G_TYPE_MODULE (ui));
}

int
main (int   argc,
      char**argv)
{
  CUserInterface* ui;
  gchar         * filename;

  if (argc != 2)
    {
      g_warning ("Usage:\n\t%s module.la", argv[0]);
      return 1;
    }

  gtk_test_init (&argc, &argv, NULL);

  C_TYPE_MAIN_WINDOW;

  filename = g_strdup_printf (".libs" G_DIR_SEPARATOR_S "%s", argv[1]);
  filename[strlen(filename)-2] = 's';
  filename[strlen(filename)-1] = 'o';
  ui = c_user_interface_new (filename);
  g_free (filename);

  test_create_window (ui);

  return 0;
}

