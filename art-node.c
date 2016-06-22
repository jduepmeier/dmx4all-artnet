#include "art-node.h"

int usage(char* fn){
	printf("xlaser - Whatever\n");
	printf("Usage:\n");
	printf("\t%s <path to config file>", fn);
	return EXIT_FAILURE;
}

int config_artSubUni(const char* category, char* key, char* value, EConfig* econfig, void* user_param) {

	CONFIG* config = (CONFIG*) user_param;
	config->art_subUni = strtoul(value, NULL, 10);

	return 0;
}

int config_dmxAddress(const char* category, char* key, char* value, EConfig* econfig, void* user_param) {
	CONFIG* config = (CONFIG*) user_param;

	config->dmx_address = strtoul(value, NULL, 10);

	return 0;
}

int config_artNet(const char* category, char* key, char* value, EConfig* econfig, void* user_param) {

	CONFIG* config = (CONFIG*) user_param;

	config->art_net = strtoul(value, NULL, 10);

	return 0;
}

int config_bindhost(const char* category, char* key, char* value, EConfig* econfig, void* user_param) {

	CONFIG* config = (CONFIG*) user_param;

	config->bindhost = malloc(strlen(value) + 1);
	strncpy(config->bindhost, value, strlen(value) + 1);

	return 0;
}

int config_device(const char* category, char* key, char* value, EConfig* econfig, void* user_param) {

	CONFIG* config = (CONFIG*) user_param;
	config->device = open_device(value);

	if (!config->device) {
		return -1;
	}

	return 0;
}

int parse_config(CONFIG* config, char* filepath) {

	EConfig* econfig = econfig_init(filepath, config);

	unsigned artNetCat = econfig_addCategory(econfig, "artnet");
	unsigned dmxCat = econfig_addCategory(econfig, "dmx");
	unsigned genCat = econfig_addCategory(econfig, "general");

	econfig_addParam(econfig, artNetCat, "net", config_artNet);
	econfig_addParam(econfig, artNetCat, "subuni", config_artSubUni);

	econfig_addParam(econfig, dmxCat, "address", config_dmxAddress);

	econfig_addParam(econfig, genCat, "bindhost", config_bindhost);
	econfig_addParam(econfig, genCat, "device", config_device);

	int status = econfig_parse(econfig);
	econfig_free(econfig);

	return status;
}

int main(int argc, char** argv){
	if(argc < 2){
		exit(usage(argv[0]));
	}

	CONFIG config = {
		.dmx_address = 16,
		.bindhost = "*"
	};

	config.bindhost = malloc(2);
	config.bindhost[0] = '*';
	config.bindhost[1] = 0;

	int status = parse_config(&config, argv[1]);
	if (status < 0) {
		printf("Status: %d\n", status);
		free(config.bindhost);
		return 10;
	}

	//TODO sanity check config
	//TODO set up signal handlers

	//open artnet listener
	config.sockfd = udp_listener(config.bindhost, "6454");
	if(config.sockfd < 0){
		fprintf(stderr, "Failed to open ArtNet listener\n");
		exit(usage(argv[0]));
	}

	//run main loop
	art_node(&config);

	//TODO cleanup
	free(config.bindhost);
	close(config.device);
	return 0;
}
