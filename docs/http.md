# HTTP/1 message-head parser

`<lh/http.h>` is an allocation-free protocol brick. It parses request/status
lines and fields from byte views. It does not open sockets, read files, manage
threads, decode a body or decide which methods a service allows.

```c
#include <lh/http.h>

const char input[] = "GET /maps/example.bsp HTTP/1.1\r\nHost: example\r\n\r\n";
lh_str_view_t bytes = lh_str_view_initializer(input, sizeof input - 1);
lh_http_parser_t parser;
lh_http_request_t request;
lh_http_header_t headers[16];
lh_http_parser_init(&parser);
lh_http_parse_status_t status = lh_http_parser_request(&parser, bytes, &request, headers, 16);
/* On complete: request.method/target, headers[0..header_count), get_consumed(). */
```

For a response use `lh_http_parser_response` and `lh_http_response_t`.
Both interfaces return complete/incomplete/error. Empty input is incomplete.
On incomplete, append bytes to the input and retry from its beginning. The
parser does not retain the input or an incremental cursor between calls.

Only inspect parsed output on complete. All views point into the caller's
original buffer and expire if it moves, changes or is freed. The header array
also belongs to the caller. Names retain their spelling; duplicate fields
stay separate. Consumers must compare field names without ASCII case and
apply their own duplicate/framing rules.

`get_consumed` gives the byte offset immediately after the terminating empty
line. It does not include a body or a pipelined message. A file service can
require this to equal the received byte count and reject Content-Length and
Transfer-Encoding; a general HTTP client must implement body framing itself.
Do not treat successful head parsing as a full HTTP validation policy.

Syntax follows the message-head rules of
[RFC 9112](https://www.rfc-editor.org/rfc/rfc9112.html) with strict CRLF and
rejection of obsolete folding. Targets remain raw ASCII URI text; percent
decoding and path validation are separate operations. Values preserve
non-ASCII octets without interpreting their encoding. HTTP/1 minor versions
are single digits; supported protocol versions remain application policy.

Default budgets are 8192 head bytes including CRLF and 4096 line-text bytes
excluding CRLF. Use `lh_http_parser_set_limits` for other nonzero budgets.
Storage capacity independently limits field count. Errors report a category
and input offset. Header bytes already inspected before incomplete/error may
have populated storage; those partial values are not a successful result.

The implementation is original LH code, not a renamed vendored parser.
It uses only LH types and local byte operations, without runtime/OS calls,
allocation, locale, CPU intrinsics or mutable globals. Link normal `lh`, or
the small `lh::http_parser` target when the rest of LH is unnecessary.

The standalone `lh_test_http_parser` target runs even without GoogleTest.
`LH_HTTP_PARSER_SANITIZE=ON` enables ASan/UBSan for that test with supported
desktop compilers. This is not an STM32 hardware certification.
