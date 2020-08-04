#include <switch.h>
#include "mod_example.h"

globals_t globals;

SWITCH_MODULE_LOAD_FUNCTION(mod_example_load);
SWITCH_MODULE_SHUTDOWN_FUNCTION(mod_example_shutdown);

//#define SWITCH_MODULE_DEFINITION_EX(name, load, shutdown, runtime, flags)
//static const char modname[] = #name ;
SWITCH_MODULE_DEFINITION(mod_example, mod_example_load, mod_example_shutdown, NULL);

//switch_status_t mod_example_load (switch_loadable_module_interface_t **module_interface, switch_memory_pool_t *pool);
SWITCH_MODULE_LOAD_FUNCTION(mod_example_load) {
    memset(&globals, 0, sizeof(globals));
    globals.pool = pool;

    switch_mutex_init(&globals.mutex, SWITCH_MUTEX_DEFAULT, globals.pool);
    switch_core_hash_init(&globals.events);

    /* create a loadable module interface structure named with modname */
    /* the module interface defines the different interfaces that this module has defined */
    *module_interface = switch_loadable_module_create_module_interface(pool, modname);

    /* create an api interface for the module command line interface (cli) */
    add_cli_api(module_interface, &globals.api_interface);

    load_config(CONFIG_FILE);

    return SWITCH_STATUS_SUCCESS;
}

SWITCH_MODULE_SHUTDOWN_FUNCTION(mod_example_shutdown) {
    remove_cli_api();

    switch_mutex_lock(globals.mutex);
    for (switch_hash_index_t *events_hi = switch_core_hash_first(globals.events); events_hi; events_hi = switch_core_hash_next(&events_hi)) {
        event_t *event = NULL;
        void *val;
        switch_core_hash_this(events_hi, NULL, NULL, &val);
        event = (event_t *)val;
        switch_event_unbind(&event->node);
    }
    switch_mutex_unlock(globals.mutex);

    switch_core_hash_destroy(&globals.events);
    switch_event_unbind_callback(event_handler);
    switch_mutex_destroy(globals.mutex);
    
    return SWITCH_STATUS_SUCCESS;
}

SWITCH_STANDARD_API(exec_api_cmd)
{
    char *argv[1024] = {0};
    int argc = 0;
    char *mycmd = NULL;
	static const char usage_string[] = "USAGE:\nexample status\n";

    if (zstr(cmd)) {
        stream->write_function(stream, "%s\n", usage_string);
        return SWITCH_STATUS_SUCCESS;
    }

    mycmd = strdup(cmd);

    if (!(argc = switch_separate_string(mycmd, ' ', argv, (sizeof(argv) / sizeof(argv[0])))) || !argv[0]) {
		stream->write_function(stream, "%s", usage_string);
		goto done;
	}

    if (!strncasecmp(argv[0], "status", 6)) {
        switch_mutex_lock(globals.mutex);
        for (switch_hash_index_t *events_hi = switch_core_hash_first(globals.events); events_hi; events_hi = switch_core_hash_next(&events_hi)) {
            event_t *event = NULL;
            void *val;
            switch_core_hash_this(events_hi, NULL, NULL, &val);
            event = (event_t *)val;
            stream->write_function(stream, "Event: %s\n", event->name);
        }
        switch_mutex_unlock(globals.mutex);
    }

done:
    switch_safe_free(mycmd);
    return SWITCH_STATUS_SUCCESS;
}

#define EXAMPLE_DESC "Example API"
#define EXAMPLE_SYNTAX "example status"
void add_cli_api(switch_loadable_module_interface_t **module_interface, switch_api_interface_t *api_interface)
{
	SWITCH_ADD_API(api_interface, "example", EXAMPLE_DESC, exec_api_cmd, EXAMPLE_SYNTAX);
	switch_console_set_complete("add example status");
}

void remove_cli_api() { switch_console_set_complete("del example"); }


void event_handler(switch_event_t *event)
{
    char *buf;
    switch_event_serialize_json(event, &buf);
    log(SWITCH_LOG_DEBUG, "%s\n", buf);
    switch_safe_free(buf);
}

switch_status_t load_config(const char *cf)
{
    switch_xml_t cfg, xml, events, param;

    xml = switch_xml_open_cfg(cf, &cfg, NULL);

    if ((events = switch_xml_child(cfg, "events"))) {
        for (param = switch_xml_child(events, "param"); param; param = param->next) {
            char *var = NULL;
            char *val = NULL;
            var = (char *)switch_xml_attr_soft(param, "name");
            val = (char *)switch_xml_attr_soft(param, "value");
            if (!strncasecmp(var, "event", 5) && !zstr(val)) {
                event_t *event = NULL;
                event = (event_t *)switch_core_alloc(globals.pool, sizeof(event_t));
                event->name = switch_core_strdup(globals.pool, val);
                switch_name_event(event->name, &event->event_type);
                switch_event_bind_removable("example", event->event_type, SWITCH_EVENT_SUBCLASS_ANY, event_handler, NULL, &event->node);
                switch_core_hash_insert_locked(globals.events, event->name, event, globals.mutex);
                log(SWITCH_LOG_INFO, "Added new event: %s\n", event->name);
            }
        }
    }
    switch_xml_free(xml);
    return SWITCH_STATUS_SUCCESS;
}
