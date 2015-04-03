/*
  JetHelp - Simple ManPage utility for JetTask
  Copyright (C) 2015 Marius Messerschmidt, JetSpace

  Goals:
    - View Manpages, using a WebKit2 Engine
*/

#include <gtk/gtk.h>          //GTK
#include <webkit2/webkit2.h>  //WebKit Engine, to display manpages

#include <stdio.h>            //file utilies
#include <string.h>
#include <stdlib.h>


#define VERSION   "0.00"
#define AUTHOR    "Marius Messerschmidt"
#define PROJECT   "JetSpace"

#define SEARCH_DIALOG_UI "/usr/share/jethelp/search.ui"
#define MANPAGES "/usr/share/man"

#define TEMP  "/tmp/dec_man.tmp"
#define HTML_TEMP "/tmp/man.html"
#define TEMP_URL "file:///tmp/man.html"

#define CSS "file:///usr/share/jethelp/base.css"

#define USE_CMD_LINE_UNZIP

GtkWidget *win, *header, *webkit, *search;

GtkWidget *search_entry, *box;

#ifdef USE_CMD_LINE_UNZIP
gboolean unzip(char *page, int sec)
{
  char b[2500];
  snprintf(b, 2500, "gzip -dc %s/man%d/%s > %s", MANPAGES, sec, page, TEMP);
  system(b);

  return TRUE;
}
#endif

void c2html(const char *name)
{
  FILE *in, *out;

  in  = fopen(TEMP, "r");
  out = fopen(HTML_TEMP, "w");

  gboolean inname = FALSE;
  gboolean synopsis = FALSE;
  gboolean description = FALSE;
  gboolean options = FALSE;
  gboolean initialization_file = FALSE;
  gboolean bugs = FALSE;
  gboolean notes = FALSE;
  gboolean homepage = FALSE;
  gboolean author = FALSE;
  gboolean see_also = FALSE;
  gboolean other = FALSE;

  gboolean bold         = FALSE;
  gboolean italic       = FALSE;

  gboolean bold_at_end  = FALSE;
  gboolean italic_at_end= FALSE;
  gboolean new_line     = FALSE;

  //print basic infos
  fprintf(out, "<html>\n<head>\n<title>%s</title>\n<link rel=\"stylesheet\" type=\"text/css\" href=\"%s\">\n</head>\n<body>\n", name, CSS);
  fprintf(out, "<center><h1>%s</h1></center>\n", name);
  char buff[2520];
  while(fgets(buff, 2500, in) != NULL)
    {//CONVERT!

      //SKIP EMPTEY LINES
      if(strlen(buff) == 0)
        continue;

      //SKIP COMMENTS
      if(buff[0] == '.' && buff[1] == '\\')
        continue;

      //convert
      int x = 0;
      while(x < strlen(buff))
        {
          if(buff[x] == '\\')
          {
            if(buff[x+1] == '-')
              {
                buff[x] = '-';
                buff[x+1] = '\b';
              }
            else if(buff[x+1] == ' ')
              {
                buff[x] = ' ';
              }
            else if(buff[x+1] == '&')
              {
                buff[x] = ' ';
                buff[x+1] = ' ';
              }
            else if(buff[x+1] == 'f' && buff[x+2] == 'B')
            {
              buff[x]   = '<';
              buff[x+1] = 'b';
              buff[x+2] = '>';
              bold = TRUE;
            }
            else if(buff[x+1] == 'f' && buff[x+2] == 'I')
            {
              buff[x]   = '<';
              buff[x+1] = 'i';
              buff[x+2] = '>';
              italic = TRUE;
            }
            else if((buff[x+1] == 'f' && buff[x+2] == 'P' )||( buff[x+1] == 'f' && buff[x+2] == 'R'))
            {
              if(buff[x+2] == 'R')
                new_line = TRUE;

              char buff2[2550];
              strncpy(buff2, buff, 2550);
              int c = 0;
              while(c < strlen(buff2) +4)
                {
                  if(c < x)
                    buff[c] = buff2[c];
                  else if (c == x)
                  {
                    buff[x]   = '<';
                    buff[x+1] = '/';
                    if(bold == TRUE)
                      {
                        buff[x+2] = 'b';
                        bold = FALSE;
                      }
                    else if(italic == TRUE)
                      {
                        buff[x+2] = 'i';
                        italic = FALSE;
                      }
                    buff[x+3] = '>';
                    c+=3;
                  }
                  else
                    buff[c] = buff2[c-1];
                  c++;
                }

            }

          }
          if(buff[x] == '.')
            {
              if(buff[x+1] == 'B')
                {
                  buff[x]   = '<';
                  buff[x+1] = 'b';
                  buff[x+2] = '>';
                  bold_at_end = TRUE;
                }
              else if(buff[x+1] == 'I')
                {
                  buff[x]   = '<';
                  buff[x+1] = 'i';
                  buff[x+2] = '>';
                  italic_at_end = TRUE;
                }
              else if(buff[x+1] == 'b' && buff[x+2] == 'r')
                {
                  buff[x] = '<';
                  buff[x+1] = 'b';
                  buff[x+2] = 'r';
                  buff[x+3] = '>';
                  buff[x+4] = '\0';
                }
             else if(buff[x+1] == 'P' && buff[x+2] == 'P')
              {
                buff[x] = '<';
                buff[x+1] = 'b';
                buff[x+2] = 'r';
                buff[x+3] = '>';
                buff[x+4] = '<';
                buff[x+5] = 'b';
                buff[x+6] = 'r';
                buff[x+7] = '>';
                buff[x+8] = '\0';
              }
            else if(buff[x+1] == 'T' && buff[x+2] == 'P')
              {
                buff[x] = '<';
                buff[x+1] = 'b';
                buff[x+2] = 'r';
                buff[x+3] = '>';
                buff[x+4] = '<';
                buff[x+5] = 'b';
                buff[x+6] = 'r';
                buff[x+7] = '>';
                buff[x+8] = '\0';
              }
            else if(buff[x+1] == 'T' && buff[x+2] == 'H')
              {
                  buff[0] = ' ';
                  buff[1] = ' ';
                  buff[2] = ' ';
                  fprintf(out, "<div id=\"foot\">%s</div>\n", buff);
              }
            }


          x++;
        }

      //RESET SECTION
      if(buff[0] == '.' && buff[1] == 'S' && buff[2] == 'H')
        {
          inname = FALSE;
          synopsis = FALSE;
          description = FALSE;
          options = FALSE;
          initialization_file = FALSE;
          bugs = FALSE;
          notes = FALSE;
          homepage = FALSE;
          see_also = FALSE;
          author = FALSE;
          other = FALSE;
          fprintf(out, "</p>\n");
        }

      //ACTIONS!
      if(inname)
        fprintf(out, "<center><h5>%s</h5></center>\n", buff);

      if(synopsis || description || options || initialization_file || bugs || notes || author || see_also)
        fprintf(out, "%s\n", buff);
      if(homepage)
        fprintf(out, "<a href = %s>%s</a>\n", buff, buff);
      if(other)
        fprintf(out, "%s\n", buff);




      //CHECK FOR NAME SECTION
      if(strncmp(buff, ".SH NAME", 8) == 0)
        {
          fprintf(out, "<p id=\"head\">");
          inname = TRUE;
        }
      //CHECK FOR SYNOPSIS SECTION
      else if(strncmp(buff, ".SH SYNOPSIS", 12) == 0)
        {
          fprintf(out, "<h3>SYNOPSIS:</h3>\n<p id=\"synopsis\">");
          synopsis = TRUE;
        }
      else if(strncmp(buff, ".SH DESCRIPTION", 15) == 0)
        {
          fprintf(out, "<h3>DESCRIPTION:</h3>\n<p id=\"description\">");
          description = TRUE;
        }
      else if(strncmp(buff, ".SH OPTIONS", 11) == 0)
        {
          fprintf(out, "<h3>OPTIONS:</h3>\n<p id=\"options\">");
          options = TRUE;
        }
      else if(strncmp(buff, ".SH INITIALIZATION FILE", 23) == 0)
        {
          fprintf(out, "<h3>INITIALIZATION FILE:</h3>\n<p id=\"initfile\">");
          initialization_file = TRUE;
        }
      else if(strncmp(buff, ".SH NOTES", 9) == 0)
        {
          fprintf(out, "<h3>NOTES:</h3>\n<p id=\"notes\">");
          notes = TRUE;
        }
      else if(strncmp(buff, ".SH BUGS", 8) == 0)
        {
          fprintf(out, "<h3>BUGS:</h3>\n<p id=\"bugs\">");
          bugs = TRUE;
        }
      else if(strncmp(buff, ".SH HOMEPAGE", 12) == 0)
        {
          fprintf(out, "<h3>HOMEPAGE:</h3>\n<p id=\"homepage\">");
          homepage = TRUE;
        }
      else if(strncmp(buff, ".SH SEE ALSO", 12) == 0)
        {
          fprintf(out, "<h3>SEE ALSO:</h3>\n<p id=\"seealso\">");
          see_also = TRUE;
        }
      else if(strncmp(buff, ".SH AUTHOR", 10) == 0)
        {
          fprintf(out, "<h3>AUTHOR:</h3>\n<p id=\"author\">");
          author = TRUE;
        }
      else if(strncmp(buff, ".SH", 3) == 0)
        {
          fprintf(out, "<h3>OTHER:</h3>\n<p id=\"unknown\">");
          other = TRUE;
        }




      //CLOSING TAGS
      if(bold_at_end)
      {
        fprintf(out, "</b>");
        bold_at_end = FALSE;
      }
      if(italic_at_end)
      {
        fprintf(out, "</i>");
        italic_at_end = FALSE;
      }
      if(new_line)
      {
        fprintf(out, "<br>");
        new_line = FALSE;
      }

      memset(buff, 0, sizeof(buff));

    }


  //print closing
  fprintf(out, "<br><br></body>\n</html>\n");

  fclose(in);
  fclose(out);
}

FILE *test_manpage_location(char *location, char *name)
{
  return fopen(g_build_path("/", MANPAGES, location, name, NULL), "r");
}

static gboolean open_manpage(GtkWidget *w, GdkEvent *e, gpointer p)
{


  FILE *f = NULL;
  int n = 1;
  //Scan man page dir (1-8)
  char *filename = g_strconcat(gtk_entry_get_text(GTK_ENTRY(search_entry)), ".1.gz", NULL);
  f = test_manpage_location("man1", filename);
  if(f == NULL)
    g_free(filename);
  if(f == NULL)
    {
      n = 2;
      memset(filename, 0, strlen(filename));
      filename = g_strconcat(gtk_entry_get_text(GTK_ENTRY(search_entry)), ".2.gz", NULL);
      f = test_manpage_location("man2", filename);
      if(f == NULL)
        g_free(filename);
    }
  if(f == NULL)
    {
      n = 3;
      memset(filename, 0, strlen(filename));
      filename = g_strconcat(gtk_entry_get_text(GTK_ENTRY(search_entry)), ".3.gz", NULL);
      f = test_manpage_location("man3", filename);
      if(f == NULL)
        g_free(filename);
    }
  if(f == NULL)
    {
      n = 4;
      memset(filename, 0, strlen(filename));
      filename = g_strconcat(gtk_entry_get_text(GTK_ENTRY(search_entry)), ".4.gz", NULL);
      f = test_manpage_location("man4", filename);
      if(f == NULL)
        g_free(filename);
    }
  if(f == NULL)
    {
      n = 5;
      memset(filename, 0, strlen(filename));
      filename = g_strconcat(gtk_entry_get_text(GTK_ENTRY(search_entry)), ".5.gz", NULL);
      f = test_manpage_location("man5", filename);
      if(f == NULL)
        g_free(filename);
    }
  if(f == NULL)
    {
      n = 6;
      memset(filename, 0, strlen(filename));
      filename = g_strconcat(gtk_entry_get_text(GTK_ENTRY(search_entry)), ".6.gz", NULL);
      f = test_manpage_location("man6", filename);
      if(f == NULL)
        g_free(filename);
    }
  if(f == NULL)
    {
      n = 7;
      memset(filename, 0, strlen(filename));
      filename = g_strconcat(gtk_entry_get_text(GTK_ENTRY(search_entry)), ".7.gz", NULL);
      f = test_manpage_location("man7", filename);
      if(f == NULL)
        g_free(filename);
    }
  if(f == NULL)
    {
      n = 8;
      memset(filename, 0, strlen(filename));
      filename = g_strconcat(gtk_entry_get_text(GTK_ENTRY(search_entry)), ".8.gz", NULL);
      f = test_manpage_location("man8", filename);
      if(f == NULL)
        g_free(filename);
    }

  if(f == NULL)
    {
      GtkWidget *dialog = gtk_message_dialog_new (GTK_WINDOW(box),
                                   GTK_DIALOG_MODAL | GTK_DIALOG_USE_HEADER_BAR,
                                   GTK_MESSAGE_ERROR,
                                   GTK_BUTTONS_CLOSE,
                                   "Error: can't find manpage.");
      gtk_dialog_run (GTK_DIALOG (dialog));
      gtk_widget_destroy (dialog);
      gtk_widget_destroy(box);
      return FALSE;
    }

  //we have a match
  unzip(filename, n);
  c2html(gtk_entry_get_text(GTK_ENTRY(search_entry)));

  webkit_web_view_load_uri(WEBKIT_WEB_VIEW(webkit), TEMP_URL);
  gtk_header_bar_set_subtitle (GTK_HEADER_BAR(header), filename);

  g_free(filename);
  gtk_widget_destroy(box);
  return FALSE;
}

static gboolean search_cb(GtkWidget *w, GdkEvent *e, gpointer p)
{
  GtkBuilder *b = gtk_builder_new_from_file(SEARCH_DIALOG_UI);

  box = GTK_WIDGET(gtk_builder_get_object(GTK_BUILDER(b), "search_box"));
  GtkWidget *button = GTK_WIDGET(gtk_builder_get_object(GTK_BUILDER(b), "search_button"));
  search_entry = GTK_WIDGET(gtk_builder_get_object(GTK_BUILDER(b), "query_box"));


  g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(open_manpage), NULL);

  gtk_widget_show_all(box);
  return FALSE;
}

static gboolean destroy(GtkWidget *w, GdkEvent *e, gpointer p)
{
  gtk_main_quit();
  return FALSE;
}

static gboolean closewebview(WebKitWebView* wv, GtkWidget* w)
{
  gtk_widget_destroy(w);
  return TRUE;
}

int main(int argc, char **argv)
{
  //init GTK
  gtk_init(&argc, &argv);

  win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_default_size(GTK_WINDOW(win), 500, 600);
  g_signal_connect(G_OBJECT(win), "destroy", G_CALLBACK(destroy), NULL);

  header = gtk_header_bar_new();
  gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(header), TRUE);
  gtk_header_bar_set_title(GTK_HEADER_BAR(header), "JetHelp");

  search = gtk_button_new_with_label("Search");
  gtk_button_set_relief(GTK_BUTTON(search), GTK_RELIEF_NONE);
  gtk_container_add(GTK_CONTAINER(header), search);
  g_signal_connect(G_OBJECT(search), "clicked", G_CALLBACK(search_cb), NULL);

  gtk_window_set_titlebar(GTK_WINDOW(win), header);

  webkit = webkit_web_view_new();
  g_signal_connect(G_OBJECT(webkit), "close", G_CALLBACK(closewebview), win);

  gtk_container_add(GTK_CONTAINER(win), webkit);



  //main loop
  gtk_widget_show_all(win);
  gtk_main();
}
