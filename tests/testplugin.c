/**
 * vim: tabstop=2:shiftwidth=2:softtabstop=2:expandtab
 *
 * testplugin.c
 * OpenVPN LDAP Authentication Plugin Test Driver
 *
 * Copyright (c) 2005 Landon Fuller <landonf@threerings.net>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of Landon Fuller nor the names of any contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <openvpn/openvpn-plugin.h>
#include <sys/types.h>
#include <unistd.h>

#define SLEEP_TIME 5

const char username_template[] = "username=";
const char password_template[] = "password=";


int main(int argc, const char *argv[]) {
	openvpn_plugin_handle_t handle;
	unsigned int type;
	const char *envp[6]; /* username, password, verb, ifconfig_pool_remote_ip, NULL */
	char username[30];
	char *password;
	int loops;
	int err;
  pid_t pid = getpid();
  char command[100];

  
	/* Grab username and password */
	printf("Username: ");
	if(!scanf("%s", username)){
    fprintf(stderr, "Could not read username\n");
    return 1;
  }

	password = getpass("Password: ");

  printf("Number of authentication loops: ");
  if(!scanf("%d", &loops)){
    fprintf(stderr, "Could not read loops number\n");
    return 1;
  }

	/* Set up username and password */
	envp[0] = malloc(sizeof(username_template) + strlen(username));
	strcpy((char *) envp[0], username_template);
	strcat((char *) envp[0], username);

	envp[1] = malloc(sizeof(password_template) + strlen(password));
	strcpy((char *) envp[1], password_template);
	strcat((char *) envp[1], password);
  free( password );
	/* Remote Pool IP */
	envp[2] = "ifconfig_pool_remote_ip=10.0.50.1";
  envp[3] = "verb=4";
  envp[4] = "auth_control_file=foobar_ctrl_file.txt";
	envp[5] = NULL;

	handle = openvpn_plugin_open_v1(&type, argv, envp);

	if (!handle)
		errx(1, "Initialization Failed!\n");

	/* Authenticate */
  for( ; loops; --loops ){
    err = openvpn_plugin_func_v1(handle, OPENVPN_PLUGIN_AUTH_USER_PASS_VERIFY, argv, envp);
    if (err == OPENVPN_PLUGIN_FUNC_ERROR) {
      printf("Authorization Failed!\n");
    } else if( err == OPENVPN_PLUGIN_FUNC_SUCCESS ) {
      printf("Authorization Succeed!\n");
    }else if ( err == OPENVPN_PLUGIN_FUNC_DEFERRED ){
      printf("Authorization Deferred!\n");
    }
  }
  printf( "Sleeping %d second waiting for threads to exits...\n", SLEEP_TIME );
  sleep( SLEEP_TIME );
  goto free_exit;
	/* Client Connect */
	err = openvpn_plugin_func_v1(handle, OPENVPN_PLUGIN_CLIENT_CONNECT, argv, envp);
	if (err != OPENVPN_PLUGIN_FUNC_SUCCESS) {
		printf("client-connect failed!\n");
	} else {
		printf("client-connect succeed!\n");
	}

	/* Client Disconnect */
	err = openvpn_plugin_func_v1(handle, OPENVPN_PLUGIN_CLIENT_DISCONNECT, argv, envp);
	if (err != OPENVPN_PLUGIN_FUNC_SUCCESS) {
		printf("client-disconnect failed!\n");
	} else {
		printf("client-disconnect succeed!\n");
	}
free_exit:
  sprintf(command, "lsof -n -p %d", pid);
  //system(command);
	openvpn_plugin_close_v1(handle);

	free((char *) envp[0]);
	free((char *) envp[1]);

  
	exit (0);
}
