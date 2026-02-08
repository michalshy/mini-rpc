#include "result.h"

#include "error.h"

namespace mini_rpc {

bool Result::ok() const {
    return data.has_value();
}

Error Result::error() const {
    return data.error();
}
} // namespace mini_rpc