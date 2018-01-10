#include <libssh/libssh.h>
#include <stdlib.h>
#include <stdio.h>

int authenticate_password(ssh_session session)
{
    char *password;
    int rc;
    password = getpass("Enter your password: ");
    rc = ssh_userauth_password(session, NULL, password);
    if (rc == SSH_AUTH_ERROR)
    {
        fprintf(stderr, "Authentication failed: %s\n",
        ssh_get_error(session));
        return SSH_AUTH_ERROR;
    }
    return rc;
}


int main()
{
    ssh_session my_ssh_session;
    int verbosity = SSH_LOG_PROTOCOL;
    int port = 22;
    int rc;

    my_ssh_session = ssh_new();
    if (my_ssh_session == NULL)
    {
        exit(-1);
    }

    ssh_options_set(my_ssh_session, SSH_OPTIONS_HOST, "localhost");
    ssh_options_set(my_ssh_session, SSH_OPTIONS_LOG_VERBOSITY, &verbosity);
    ssh_options_set(my_ssh_session, SSH_OPTIONS_PORT, &port);

     rc = ssh_connect(my_ssh_session);
    if (rc != SSH_OK)
    {
        fprintf(stderr, "Error connecting to localhost: %s\n",
        ssh_get_error(my_ssh_session));
        exit(-1);
    }

    ssh_disconnect(my_ssh_session);
    ssh_free(my_ssh_session);

    return 0;
}
