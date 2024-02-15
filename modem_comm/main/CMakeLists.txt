set(srcs 	"app_main.c"
			"modem_cmds.c"
	)

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "."
                    REQUIRED_IDF_TARGETS esp32)