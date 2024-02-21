void help(void);
void version(void);
int isalldigit(char*);
int valid_port(char*);
int valid_host(char*);
int is_ipv4(char*);
int is_domain_name(char*);

typedef struct {
	int listen_flag;
	char* host;
	int port; 
} Flags;
