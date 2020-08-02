#ifndef MOD_EXAMPLE_H
#define MOD_EXAMPLE_H

#define log(severity, ...) switch_log_printf(SWITCH_CHANNEL_LOG, severity, __VA_ARGS__);

#define CONFIG_FILE "example.conf"

typedef struct event_s {
	char *name;
	switch_event_types_t event_type;
	switch_event_node_t *node;
} event_t;

typedef struct globals_s {
	switch_memory_pool_t *pool;
	switch_mutex_t *mutex;
	switch_api_interface_t api_interface;
	switch_hash_t *events;
} globals_t;
extern globals_t globals;

void event_handler(switch_event_t *event);
switch_status_t load_config(const char *cf);

void add_cli_api(switch_loadable_module_interface_t **module_interface, switch_api_interface_t *api_interface);
void remove_cli_api();

#endif /* MOD_EXAMPLE_H */
