#include <stdlib.h>
#include "nrf_log.h"
#include "runtime_error_impl.h"
#include "module/data_structures/stack.h"

typedef struct {
    uint16_t id;
    const char* message;
    int err_code;
} error_t;

static uint8_t s_id_counter_;
static bool s_is_stack_init_ = false;
static instance_t* s_stack_;
#define ERRORS_LEN 255
static error_t* s_errors_;
static bool s_is_any_errors = false;

void runtime_error_init(void) {
    s_stack_ = stack_ctx_alloc_instance(100);
    s_errors_ = malloc(sizeof(error_t) * ERRORS_LEN);
    s_is_stack_init_ = true;
}

void runtime_error(const char* m, int p) {
    if (!s_is_stack_init_) {
        runtime_error_init();
    }

    error_t e = {
            .id = s_id_counter_,
            .message = m,
            .err_code = p
    };
    s_errors_[s_id_counter_] = e;
    s_id_counter_++;
    stack_ctx_instance_push(s_stack_, e.id);
    s_is_any_errors = true;
}

void runtime_error_stacktrace(void) {
    uint8_t id;
    while (!stack_ctx_instance_is_empty(s_stack_)) {
        id = stack_ctx_instance_pop(s_stack_);
        NRF_LOG_INFO("%s\n", s_errors_[id].message);
    }
}