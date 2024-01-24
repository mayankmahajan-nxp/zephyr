/*
 * Copyright (c) 2022 Trackunit Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/types.h>
#include <zephyr/device.h>
#include <zephyr/sys/ring_buffer.h>

#include <zephyr/modem/pipe.h>

#ifndef ZEPHYR_MODEM_UBX_
#define ZEPHYR_MODEM_UBX_

#ifdef __cplusplus
extern "C" {
#endif

struct modem_ubx;

/**
 * @brief Callback called when matching ubx is received
 *
 * @param ubx Pointer to ubx instance instance
 * @param argv Pointer to array of parsed arguments
 * @param argc Number of parsed arguments, arg 0 holds the exact match
 * @param user_data Free to use user data set during modem_ubx_init()
 */
typedef void (*modem_ubx_match_callback)(struct modem_ubx *ubx, char **argv, uint16_t argc,
					  void *user_data);

/**
 * @brief Modem ubx match
 */
struct modem_ubx_match {
	/** Match array */
	const uint8_t *match;
	/** Size of match */
	uint8_t match_size;
	/** Separators array */
	const uint8_t *separators;
	/** Size of separators array */
	uint8_t separators_size;
	/** Set if modem ubx instance shall use wildcards when matching */
	uint8_t wildcards : 1;
	/** Set if script shall not continue to next step in case of match */
	uint8_t partial : 1;
	/** Type of modem ubx instance */
	modem_ubx_match_callback callback;
};

#define MODEM_UBX_MATCH(_match, _separators, _callback)                                           \
	{                                                                                          \
		.match = (uint8_t *)(_match), .match_size = (uint8_t)(sizeof(_match) - 1),         \
		.separators = (uint8_t *)(_separators),                                            \
		.separators_size = (uint8_t)(sizeof(_separators) - 1), .wildcards = false,         \
		.callback = _callback,                                                             \
	}

#define MODEM_UBX_MATCH_WILDCARD(_match, _separators, _callback)                                  \
	{                                                                                          \
		.match = (uint8_t *)(_match), .match_size = (uint8_t)(sizeof(_match) - 1),         \
		.separators = (uint8_t *)(_separators),                                            \
		.separators_size = (uint8_t)(sizeof(_separators) - 1), .wildcards = true,          \
		.callback = _callback,                                                             \
	}

#define MODEM_UBX_MATCH_INITIALIZER(_match, _separators, _callback, _wildcards, _partial)         \
	{                                                                                          \
		.match = (uint8_t *)(_match),                                                      \
		.match_size = (uint8_t)(sizeof(_match) - 1),                                       \
		.separators = (uint8_t *)(_separators),                                            \
		.separators_size = (uint8_t)(sizeof(_separators) - 1),                             \
		.wildcards = _wildcards,                                                           \
		.partial = _partial,                                                               \
		.callback = _callback,                                                             \
	}

#define MODEM_UBX_MATCH_DEFINE(_sym, _match, _separators, _callback)                              \
	const static struct modem_ubx_match _sym = MODEM_UBX_MATCH(_match, _separators, _callback)

#define MODEM_UBX_MATCHES_DEFINE(_sym, ...)                                                       \
	const static struct modem_ubx_match _sym[] = {__VA_ARGS__}

/**
 * @brief Modem ubx script ubx
 */
struct modem_ubx_script_ubx {
	/** Request to send to modem */
	const uint8_t *request;
	/** Size of request */
	uint16_t request_size;
	/** Expected responses to request */
	const struct modem_ubx_match *response_matches;
	/** Number of elements in expected responses */
	uint16_t response_matches_size;
	/** Timeout before ubx script may continue to next step in milliseconds */
	uint16_t timeout;
};

#define MODEM_UBX_SCRIPT_CMD_RESP(_request, _response_match)                                      \
	{                                                                                          \
		.request = (uint8_t *)(_request),                                                  \
		.request_size = (uint16_t)(sizeof(_request) - 1),                                  \
		.response_matches = &_response_match,                                              \
		.response_matches_size = 1,                                                        \
		.timeout = 0,                                                                      \
	}

#define MODEM_UBX_SCRIPT_CMD_RESP_MULT(_request, _response_matches)                               \
	{                                                                                          \
		.request = (uint8_t *)(_request),                                                  \
		.request_size = (uint16_t)(sizeof(_request) - 1),                                  \
		.response_matches = _response_matches,                                             \
		.response_matches_size = ARRAY_SIZE(_response_matches),                            \
		.timeout = 0,                                                                      \
	}

#define MODEM_UBX_SCRIPT_CMD_RESP_NONE(_request, _timeout)                                        \
	{                                                                                          \
		.request = (uint8_t *)(_request),                                                  \
		.request_size = (uint16_t)(sizeof(_request) - 1),                                  \
		.response_matches = NULL,                                                          \
		.response_matches_size = 0,                                                        \
		.timeout = _timeout,                                                               \
	}

#define MODEM_UBX_SCRIPT_CMDS_DEFINE(_sym, ...)                                                   \
	const struct modem_ubx_script_ubx _sym[] = {__VA_ARGS__}

enum modem_ubx_script_result {
	MODEM_UBX_SCRIPT_RESULT_SUCCESS,
	MODEM_UBX_SCRIPT_RESULT_ABORT,
	MODEM_UBX_SCRIPT_RESULT_TIMEOUT
};

/**
 * @brief Callback called when script ubx is received
 *
 * @param ubx Pointer to ubx instance instance
 * @param result Result of script execution
 * @param user_data Free to use user data set during modem_ubx_init()
 */
typedef void (*modem_ubx_script_callback)(struct modem_ubx *ubx,
					   enum modem_ubx_script_result result, void *user_data);

/**
 * @brief Modem ubx script
 */
struct modem_ubx_script {
	/** Name of script */
	const char *name;
	/** Array of script ubxs */
	const struct modem_ubx_script_ubx *script_ubxs;
	/** Elements in array of script ubxs */
	uint16_t script_ubxs_size;
	/** Array of abort matches */
	const struct modem_ubx_match *abort_matches;
	/** Number of elements in array of abort matches */
	uint16_t abort_matches_size;
	/** Callback called when script execution terminates */
	modem_ubx_script_callback callback;
	/** Timeout in seconds within which the script execution must terminate */
	uint32_t timeout;
};

#define MODEM_UBX_SCRIPT_DEFINE(_sym, _script_ubxs, _abort_matches, _callback, _timeout)         \
	const static struct modem_ubx_script _sym = {                                             \
		.name = #_sym,                                                                     \
		.script_ubxs = _script_ubxs,                                                     \
		.script_ubxs_size = ARRAY_SIZE(_script_ubxs),                                    \
		.abort_matches = _abort_matches,                                                   \
		.abort_matches_size = ARRAY_SIZE(_abort_matches),                                  \
		.callback = _callback,                                                             \
		.timeout = _timeout,                                                               \
	}

enum modem_ubx_script_send_state {
	/* No data to send */
	MODEM_UBX_SCRIPT_SEND_STATE_IDLE,
	/* Sending request */
	MODEM_UBX_SCRIPT_SEND_STATE_REQUEST,
	/* Sending delimiter */
	MODEM_UBX_SCRIPT_SEND_STATE_DELIMITER,
};

/**
 * @brief Ubx instance internal context
 * @warning Do not modify any members of this struct directly
 */
struct modem_ubx {
	/* Pipe used to send and receive data */
	struct modem_pipe *pipe;

	/* User data passed with match callbacks */
	void *user_data;

	/* Receive buffer */
	uint8_t *receive_buf;
	uint16_t receive_buf_size;
	uint16_t receive_buf_len;

	/* Work buffer */
	uint8_t work_buf[32];
	uint16_t work_buf_len;

	/* Ubx delimiter */
	uint8_t *delimiter;
	uint16_t delimiter_size;
	uint16_t delimiter_match_len;

	/* Array of bytes which are discarded out by parser */
	uint8_t *filter;
	uint16_t filter_size;

	/* Parsed arguments */
	uint8_t **argv;
	uint16_t argv_size;
	uint16_t argc;

	/* Matches
	 * Index 0 -> Response matches
	 * Index 1 -> Abort matches
	 * Index 2 -> Unsolicited matches
	 */
	const struct modem_ubx_match *matches[3];
	uint16_t matches_size[3];

	/* Script execution */
	const struct modem_ubx_script *script;
	const struct modem_ubx_script *pending_script;
	struct k_work script_run_work;
	struct k_work_delayable script_timeout_work;
	struct k_work script_abort_work;
	uint16_t script_ubx_it;
	atomic_t script_state;
	enum modem_ubx_script_result script_result;
	struct k_sem script_stopped_sem;

	/* Script sending */
	uint16_t script_send_request_pos;
	uint16_t script_send_delimiter_pos;
	struct k_work_delayable script_send_work;
	struct k_work_delayable script_send_timeout_work;

	/* Match parsing */
	const struct modem_ubx_match *parse_match;
	uint16_t parse_match_len;
	uint16_t parse_arg_len;
	uint16_t parse_match_type;

	/* Process received data */
	struct k_work_delayable process_work;
	k_timeout_t process_timeout;
};

/**
 * @brief Ubx configuration
 */
struct modem_ubx_config {
	/** Free to use user data passed with modem match callbacks */
	void *user_data;
	/** Receive buffer used to store parsed arguments */
	uint8_t *receive_buf;
	/** Size of receive buffer should be longest line + longest match */
	uint16_t receive_buf_size;
	/** Delimiter */
	uint8_t *delimiter;
	/** Size of delimiter */
	uint8_t delimiter_size;
	/** Bytes which are discarded by parser */
	uint8_t *filter;
	/** Size of filter */
	uint8_t filter_size;
	/** Array of pointers used to point to parsed arguments */
	uint8_t **argv;
	/** Elements in array of pointers */
	uint16_t argv_size;
	/** Array of unsolicited matches */
	const struct modem_ubx_match *unsol_matches;
	/** Elements in array of unsolicited matches */
	uint16_t unsol_matches_size;
	/** Delay from receive ready event to pipe receive occurs */
	k_timeout_t process_timeout;
};

/**
 * @brief Initialize modem pipe ubx instance
 * @param ubx Ubx instance
 * @param config Configuration which shall be applied to Ubx instance
 * @note Ubx instance must be attached to pipe
 */
int modem_ubx_init(struct modem_ubx *ubx, const struct modem_ubx_config *config);

/**
 * @brief Attach modem ubx instance to pipe
 * @param ubx Ubx instance
 * @param pipe Pipe instance to attach Ubx instance to
 * @returns 0 if successful
 * @returns negative errno code if failure
 * @note Ubx instance is enabled if successful
 */
int modem_ubx_attach(struct modem_ubx *ubx, struct modem_pipe *pipe);

/**
 * @brief Run script asynchronously
 * @param ubx Ubx instance
 * @param script Script to run
 * @returns 0 if script successfully started
 * @returns -EBUSY if a script is currently running
 * @returns -EPERM if modem pipe is not attached
 * @returns -EINVAL if arguments or script is invalid
 * @note Script runs asynchronously until complete or aborted.
 */
int modem_ubx_run_script_async(struct modem_ubx *ubx, const struct modem_ubx_script *script);

/**
 * @brief Run script
 * @param ubx Ubx instance
 * @param script Script to run
 * @returns 0 if successful
 * @returns -EBUSY if a script is currently running
 * @returns -EPERM if modem pipe is not attached
 * @returns -EINVAL if arguments or script is invalid
 * @note Script runs until complete or aborted.
 */
int modem_ubx_run_script(struct modem_ubx *ubx, const struct modem_ubx_script *script);

/**
 * @brief Abort script
 * @param ubx Ubx instance
 */
void modem_ubx_script_abort(struct modem_ubx *ubx);

/**
 * @brief Release pipe from ubx instance
 * @param ubx Ubx instance
 */
void modem_ubx_release(struct modem_ubx *ubx);

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_MODEM_UBX_ */
