#include <stdio.h>
#include <gtk/gtk.h>
#ifndef _COMMON_H
#define _COMMON_H

#define BUF_SIZE 128
#define SERVER_PORT 32000
#define SERVER_IP "127.0.0.1"

GtkBuilder *builder;
GtkWidget *loginWindow;
GtkWidget *registerWindow;
GtkWidget *chatWindow;
GtkWidget *usernameEntry;
GtkWidget *passwordEntry;
GtkWidget *sendEntry;
GtkWidget *button;
GtkWidget *fileButton;
GtkWidget *sendFileButton;
GtkWidget *label;
GtkWidget *eventBox;

const gchar *a, *b, *c;

GtkWidget *table;
GtkWidget *image;
GtkWidget *gtk_entry_new(void);

GtkWidget *textview1;
GtkWidget *textview2;
GtkTextBuffer *textbuffer1;
GtkTextBuffer *textbuffer2;

GtkWidget *data[2];

int socket_fd, connect_fd;
struct ChatMsg
{
	int who;
	int Flag;
	char Msg[50];
};
struct LoginPack
{
	int flag;
	char username[20];
	char password[20];
};
char buf[BUF_SIZE];
char u[5];
gchar *chatmsg;

#endif

void destroy(GtkWidget *widget, gpointer data)
{
	gtk_main_quit();
}

void go_register(GtkWidget *widget)
{
	gtk_widget_hide(loginWindow);
	gtk_widget_show_all(registerWindow);
}

void go_login(GtkWidget *widget)
{
	gtk_widget_hide(registerWindow);
	gtk_widget_show_all(loginWindow);
}

void go_login1(GtkWidget *widget)
{
	gtk_widget_hide(chatWindow);
	gtk_widget_show_all(loginWindow);
}

void go_chat(GtkWidget *widget)
{
	gtk_widget_hide(loginWindow);
	gtk_widget_show_all(chatWindow);
}

void sendMsg()
{

	struct ChatMsg chatmessage;
	int size;

	GtkTextIter start, end;
	textbuffer1 = gtk_text_view_get_buffer(GTK_TEXT_VIEW(sendEntry));
	textbuffer2 = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview1));
	/* get start Iter and end Iter of buffer */
	gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(textbuffer1), &start, &end);

	const GtkTextIter s = start, e = end;
	chatmsg = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(textbuffer1), \ 
			& s,&e, FALSE); /*get text of textbuffer */
	gtk_text_buffer_delete(GTK_TEXT_BUFFER(textbuffer1), &start, &end);
	g_print("%s\n", chatmsg);
	gtk_text_buffer_insert_at_cursor(textbuffer2, chatmsg, -1);
	gtk_text_buffer_insert_at_cursor(textbuffer2, "\n", -1);
	chatmessage.Flag = 100;
	if (atoi(u) == 123)
	{
		chatmessage.who = 456;
	}
	else
	{
		chatmessage.who = 123;
	}
	//printf("who = %d\n",atoi(u));
	strcpy(chatmessage.Msg, chatmsg);
	size = sizeof(chatmessage);
	memcpy(buf, &chatmessage, size);
	printf("Recv Msg = %s\n", buf);
	//textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview1));
}
void sendFile(){
 c = gtk_file_chooser_button_get_title(fileButton);
 g_print("%s\n", c);
}

void login_window_init()
{

	/* get graphics from login.glade */
	builder = gtk_builder_new();
	gtk_builder_add_from_file(builder, "loginWindow.glade", NULL);

	loginWindow = GTK_WIDGET(gtk_builder_get_object(builder, "loginWindow"));
	g_signal_connect(G_OBJECT(loginWindow), "destroy", G_CALLBACK(gtk_main_quit), NULL);

	button = GTK_WIDGET(gtk_builder_get_object(builder, "loginButton"));
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(go_chat), NULL);

	usernameEntry = GTK_WIDGET(gtk_builder_get_object(builder, "unameEntry"));
	passwordEntry = GTK_WIDGET(gtk_builder_get_object(builder, "passwordEntry"));

	button = GTK_WIDGET(gtk_builder_get_object(builder, "registerButton"));
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(go_register), NULL);
}

void register_window_init()
{

	/* get graphics from login.glade */
	builder = gtk_builder_new();
	gtk_builder_add_from_file(builder, "registerWindow.glade", NULL);

	registerWindow = GTK_WIDGET(gtk_builder_get_object(builder, "registerWindow"));
	g_signal_connect(G_OBJECT(registerWindow), "destroy", G_CALLBACK(gtk_main_quit), NULL);

	button = GTK_WIDGET(gtk_builder_get_object(builder, "submitButton"));
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(gtk_main_quit), NULL);

	usernameEntry = GTK_WIDGET(gtk_builder_get_object(builder, "unameRegister"));
	passwordEntry = GTK_WIDGET(gtk_builder_get_object(builder, "passwordRegister"));

	button = GTK_WIDGET(gtk_builder_get_object(builder, "backButton"));
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(go_login), NULL);
}

void chat_window_init()
{

	/* get graphics from login.glade */
	builder = gtk_builder_new();
	gtk_builder_add_from_file(builder, "chatWindow.glade", NULL);

	chatWindow = GTK_WIDGET(gtk_builder_get_object(builder, "chatwindow"));
	g_signal_connect(G_OBJECT(chatWindow), "destroy", G_CALLBACK(gtk_main_quit), NULL);

	textview1 = GTK_WIDGET(gtk_builder_get_object(builder, "messageText"));

	textview2 = GTK_WIDGET(gtk_builder_get_object(builder, "userText"));

	sendEntry = GTK_WIDGET(gtk_builder_get_object(builder, "sendEntry"));

	fileButton = GTK_WIDGET(gtk_builder_get_object(builder, "fileButton"));

	sendFileButton = GTK_WIDGET(gtk_builder_get_object(builder, "sendFileButton"));
	g_signal_connect(G_OBJECT(sendFileButton), "clicked", G_CALLBACK(sendFile), NULL);

	//g_signal_connect(G_OBJECT(fileButton), "clicked", G_CALLBACK(gtk_main_quit), NULL);

	button = GTK_WIDGET(gtk_builder_get_object(builder, "sendButton"));
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(sendMsg), NULL);
}

int main(int argc, char *argv[])
{

	gtk_init(&argc, &argv);

	login_window_init();
	register_window_init();
	chat_window_init();
	g_object_unref(G_OBJECT(builder));
	gtk_widget_hide(registerWindow);
	gtk_widget_show_all(loginWindow);

	gtk_main();
	return 0;
}