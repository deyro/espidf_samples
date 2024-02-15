set(srcs 	"bg95_at.c"
			"bg95_usart.c"
	)

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS  "."
                    REQUIRED_IDF_TARGETS esp32)