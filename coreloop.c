int art_node(CONFIG* config){
	fd_set readfds;
	struct timeval tv;
	int maxfd, error;

	while(!abort_signaled){

		//prepare select data
		FD_ZERO(&readfds);
		maxfd = -1;
		tv.tv_sec = 1;
		tv.tv_usec = 0;

		FD_SET(config->sockfd, &readfds);
		if(maxfd < config->sockfd){
			maxfd = config->sockfd;
		}

		error = select(maxfd + 1, &readfds, NULL, NULL, &tv);
		if(error > 0){
			if(FD_ISSET(config->sockfd, &readfds)){
				fprintf(stderr, "ArtNet Data\n");
				artnet_handler(config);
			}
		} else if(error < 0){
			perror("select");
			abort_signaled = -1;
		} else{
			fprintf(stderr, "Nothing\n");
		}
	}

	return abort_signaled;
}
